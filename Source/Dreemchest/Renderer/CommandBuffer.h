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

#ifndef __DC_Renderer_CommandBuffer_H__
#define __DC_Renderer_CommandBuffer_H__

#include "RenderState.h"

DC_BEGIN_DREEMCHEST

namespace Renderer
{
    //! A command buffer that is generated by render pass and executed by RVM.
    class CommandBuffer
    {
    friend class RenderFrame;
    public:

        //! A single render operation.
        struct OpCode
        {
            //! An op-code type.
            enum Type
            {
                  DrawIndexed           //!< Draws a list of primitives using an index buffer.
                , DrawPrimitives        //!< Draws a list of primitives from an active vertex buffer.
                , Clear                 //!< Clears a render target.
                , Execute               //!< Executes a command buffer.
                , RenderTarget          //!< Begins rendering to a viewport.
                , UploadConstantBuffer  //!< Uploads data to a constant buffer.
                , UploadVertexBuffer    //!< Uploads data to a vertex buffer.
                , AcquireRenderTarget   //!< Acquires an intermediate render target.
                , ReleaseRenderTarget   //!< Releases an intermediate render target.
                , CreateInputLayout     //!< Creates a new input layout from a vertex declaration.
                , CreateVertexBuffer    //!< Creates a new vertex buffer object.
                , CreateIndexBuffer     //!< Creates a new index buffer object.
                , CreateConstantBuffer  //!< Creates a new constant buffer object.
                , CreateTexture         //!< Creates a new texture.
            };

            Type                                type;                       //!< An op code type.
            u64                                 sorting;                    //!< A sorting key.
            union
            {
                struct
                {
                    PrimitiveType               primitives;                 //!< A primitive type to be rendered.
                    s32                         first;                      //!< First index or primitive.
                    s32                         count;                      //!< A total number of indices or primitives to use.
                    const StateBlock*           states[MaxStateStackDepth]; //!< States from this stack are applied before a rendering command.
                } drawCall;

                struct
                {
                    u8                          mask;                       //!< A clear mask.
                    f32                         color[4];                   //!< A color buffer clear value.
                    f32                         depth;                      //!< A depth buffer clear value.
                    s32                         stencil;                    //!< A stencil buffer clear value.
                } clear;

                struct
                {
                    TransientResourceId         id;                         //!< A render target resource to be activated.
                    f32                         viewport[4];                //!< A viewport value to be set.
                    const CommandBuffer*        commands;                   //!< A command buffer to be executed after setting a viewport.
                } renderTarget;

                struct
                {
                    TransientResourceId         id;                         //!< An intermediate render target handle.
                    s32                         width;                      //!< A requested render target width.
                    s32                         height;                     //!< A requested render target height.
                    PixelFormat                 format;                     //!< A requested render target format.
                } intermediateRenderTarget;

                struct
                {
                    const CommandBuffer*        commands;                   //!< A command buffer to be executed.
                } execute;

                struct
                {
                    PersistentResourceId        id;                         //!< A target buffer handle.
                    const void*                 data;                       //!< A source data point.
                    s32                         size;                       //!< A total number of bytes to upload.
                } upload;
                
                struct
                {
                    PersistentResourceId        id;                         //!< Handle to an input layout being constructed.
                    u8                          format;                     //!< Vertex format used by an input layout constructor.
                } createInputLayout;
                
                struct
                {
                    PersistentResourceId        id;                         //!< Handle to a buffer object being constructed.
                    const void*                 data;                       //!< Data that should be uploaded to a buffer after construction.
                    s32                         size;                       //!< A buffer size.
                    const void*                 userData;                   //!< Used by a constant buffer constructor.
                } createBuffer;
                
                struct
                {
                    PersistentResourceId        id;                         //!< Handle to a texture being constructed.
                    const void*                 data;                       //!< A texture data that should be uploaded after construction.
                    u16                         width;                      //!< A texture width.
                    u16                         height;                     //!< A texture height.
                    PixelFormat                 format;                     //!< A texture format.
                } createTexture;
            };
        };

        //! Returns a total number of recorded commands.
        s32                         size( void ) const;

        //! Returns a command at specified index.
        const OpCode&               opCodeAt( s32 index ) const;
        
        //! Clears a command buffer.
        void                        reset( void );

        //! Emits a render target clear command.
        void                        clear( const Rgba& clearColor, u8 clearMask );

        //! Emits a command buffer execution command.
        void                        execute( const CommandBuffer& commands );

        //! Emits an acquire intermediate render target command.
        TransientRenderTarget       acquireRenderTarget( s32 width, s32 height, PixelFormat format );

        //! Emits a release an intermediate render target command.
        void                        releaseRenderTarget( TransientRenderTarget id );

        //! Emits a rendering to a viewport of a specified render target command.
        CommandBuffer&              renderToTarget( RenderFrame& frame, TransientRenderTarget id, const Rect& viewport = Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );
        
        //! Emits a rendering to a viewport.
        CommandBuffer&              renderToTarget( RenderFrame& frame, const Rect& viewport = Rect( 0.0f, 0.0f, 1.0f, 1.0f ) );

        //! Emits a constant buffer upload command.
        void                        uploadConstantBuffer( ConstantBuffer_ id, const void* data, s32 size );

        //! Emits a vertex buffer upload command.
        void                        uploadVertexBuffer( VertexBuffer_ id, const void* data, s32 size );

        //! Emits a draw indexed command that inherits all rendering states from a state stack.
        void                        drawIndexed( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateStack& stateStack );
        
        //! Emits a draw indexed command with a single render state block.
        void                        drawIndexed( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock* stateBlock );

        //! Emits a draw primitives command that inherits all rendering states from a state stack.
        void                        drawPrimitives( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateStack& stateStack );
        
        //! Emits a draw primitives command that inherits all rendering states from a state stack.
        void                        drawPrimitives( u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock* stateBlock );

    protected:

                                    //! Constructs a CommandBuffer instance.
                                    CommandBuffer( void );
        
        //! Emits a draw call command.
        void                        emitDrawCall( OpCode::Type type, u32 sorting, PrimitiveType primitives, s32 first, s32 count, const StateBlock** states, s32 stateCount );
        
        //! Pushes a new command to a buffer.
        void                        push(const OpCode& opCode);

    private:

        Array<OpCode>               m_commands;             //!< An array of recorded commands.
        u8                          m_renderTargetIndex;    //!< An intermediate render target index relative to a current stack offset.
    };

    // ** CommandBuffer::size
    NIMBLE_INLINE s32 CommandBuffer::size( void ) const
    {
        return static_cast<s32>( m_commands.size() );
    }

    // ** CommandBuffer::opCodeAt
    NIMBLE_INLINE const CommandBuffer::OpCode& CommandBuffer::opCodeAt( s32 index ) const
    {
        NIMBLE_ABORT_IF( index < 0 || index >= size(), "index is out of range" );
        return m_commands[index];
    }

} // namespace Renderer

DC_END_DREEMCHEST

#endif    /*    !__DC_Renderer_CommandBuffer_H__    */
