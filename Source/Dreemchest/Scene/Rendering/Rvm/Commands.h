/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#ifndef __DC_Scene_Rvm_Commands_H__
#define __DC_Scene_Rvm_Commands_H__

#include "../RenderingContext.h"
#include "RasterizationOptions.h"

#define RVM_SORT( value )       (static_cast<u64>( value ) << 32)
#define RVM_SEQUENCE( value )   (static_cast<u64>( value ) << (64 - 8))

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rvm command buffer that is generated by render passes and executed by rvm.
    class Commands {
    public:

        //! Render operation is a 64-bit unsigned integer that contains a 48-bit sorting key in higher bits and 16-bit command index in lower bits.
        typedef u64 Rop;

        //! Available command types
        enum {
              OpPushRenderTarget    //!< Begins rendering to target by pushing it to a stack and setting the viewport.
            , OpPopRenderTarget     //!< Ends rendering to target by popping it from a stack.
            , OpSetRasterOptions    //!< Setups the rasterization options.
            , OpConstantColor       //!< Sets the constant color value.
            , OpShader              //!< Sets the shader to be used for a lighting model.
            , OpProgramInput        //!< Sets the program input value.
            , OpDrawIndexed         //!< Performs rendering of an indexed renderable.
        };

        //! Template class to declare commands.
        template<s32 TCommandId>
        struct Command {
            Command( void ) : id( TCommandId ) {};
            s32 id;
        };

        //! Pushes a new render target to a stack
        struct PushRenderTarget : public Command<OpPushRenderTarget> {
            const RenderTarget*         instance;       //!< Render target instance to be pushed.
            f32                         vp[16];         //!< The view-projection matrix to be set for a render target.
            u32                         viewport[4];    //!< The viewport inside this render target.
        };

        //! Pops a render target from a stack
        struct PopRenderTarget : public Command<OpPopRenderTarget> {
        };

        //! Sets the rasterization options for a set of blending modes.
        struct SetRasterOptions : public Command<OpSetRasterOptions> {
            u8                          modes;
            RasterizationOptions        options;
        };

        //! Performs an indexed draw call
        struct DrawIndexed : public Command<OpDrawIndexed> {
            u16                         technique;      //!< Rendering technique index.
            u16                         renderable;     //!< Mesh index.
            u8                          mode;           //!< Rendering mode.
            const Matrix4*              transform;      //!< Instance transform.
            Renderer::TriangleFace      culling;        //!< Triangle face culling.
        };

        //! Sets the shader to be used for a set of material lighting models.
        struct Shader : public Command<OpShader> {
            u8                          models;         //!< The bitmask of models that are eligible to use this shader.
            s32                         shader;         //!< Shader instance index.       
        };

        //! Sets the constant color program input.
        struct ConstantColor : public Command<OpConstantColor> {
            f32                         color[4];       //!< The constant color value.
        };

        //! Program input value data.
        struct ProgramInput : public Command<OpProgramInput> {
            Program::Input              location;       //!< Program input index.
            f32                         value[4];       //!< The program input value.
        };

                                        //! Constructs the Commands instance.
                                        Commands( void );

        //! Sorts the command buffer.
        void                            sort( void );

        //! Clears the command buffer.
        void                            clear( void );

        //! Dumps the generated command buffer to a log.
        void                            dump( void ) const;

        //! Returns the total number of render operations.
        s32                             size( void ) const;

        //! Pushes a single draw call instruction.
        NIMBLE_INLINE DrawIndexed*      emitDrawCall( u32 sortingKey, const Matrix4* transform, s32 renderable, s32 technique, u8 mode );

        //! Returns a rop at specified index.
        s32                             opCodeAt( s32 index ) const;

        //! Returns a command at specified index.
        template<typename TCommand>
        const TCommand&                 commandAt( s32 index ) const;

        //! Emits the command to push a render target.
        void                            emitPushRenderTarget( RenderTargetWPtr renderTarget, const Matrix4& viewProjection, const Rect& viewport );

        //! Emits the command to pop a render target.
        void                            emitPopRenderTarget( void );

        //! Emits the rasterization options command.
        void                            emitRasterOptions( u8 renderingModes, const RasterizationOptions& options );

        //! Emits the lighting shader setup command.
        void                            emitLightingShader( u8 models, s32 shader );

        //! Emits the constant color operation.
        void                            emitConstantColor( const Rgba& value );

        //! Emits the program input value operation.
        void                            emitProgramInput( Program::Input location, const Vec4& value );

    private:

        //! Forward declaration of a command user data.
        struct UserData;

        //! Forward declaration of a render target state data.
        struct RenderTargetState;

        //! Allocates a command instance.
        template<typename T>
        T*                              allocateCommand( u64 sortingKey );

        //! Begins new sequence by returning current sequence number (also post-increments the sequence number).
        u8                              beginSequence( void );

        //! Ends current sequence by incrementing the sequence number and returning result.
        u8                              endSequence( void );

    private:

        u8                              m_sequence;     //!< Current sequence number of render operation.
        IndexAllocator<Rop>             m_operations;   //!< Allocated instructions.
        LinearAllocator                 m_commands;     //!< This linear allocator is used for storing all commands.
    };

    // ** Commands::commandAt
    template<typename TCommand>
    const TCommand& Commands::commandAt( s32 index ) const
    {
        const u8* pointer = m_commands.data() + static_cast<u32>( m_operations[index] & 0xFFFFFFFF );
        return *reinterpret_cast<const TCommand*>( pointer );
    }

    // ** Commands::allocateCommand
    template<typename T>
    T* Commands::allocateCommand( u64 sortingKey )
    {
        u8* pointer = m_commands.allocate( sizeof( T ) );
        u32 offset  = pointer - m_commands.data();
        s32 idx     = m_operations.allocate();
        m_operations[idx] = RVM_SORT( sortingKey ) | offset;
        return DC_NEW( pointer ) T;
    }

    // ** Commands::emitDrawCall
    NIMBLE_INLINE Commands::DrawIndexed* Commands::emitDrawCall( u32 sortingKey, const Matrix4* transform, s32 renderable, s32 technique, u8 mode )
    {
        DC_BREAK_IF( (~4194303 & sortingKey) != 0, "sorting key bitmask overflow" );

        DrawIndexed* cmd = allocateCommand<DrawIndexed>( m_sequence << 22 | sortingKey );
        cmd->mode = mode;
        cmd->technique = technique;
        cmd->renderable = renderable;
        cmd->transform = transform;
        cmd->culling = Renderer::TriangleFaceBack;
        return cmd;
    }

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_Commands_H__    */