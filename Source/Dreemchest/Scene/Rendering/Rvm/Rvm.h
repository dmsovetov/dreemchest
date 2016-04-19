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

#ifndef __DC_Scene_Rvm_H__
#define __DC_Scene_Rvm_H__

#include "../RenderScene.h"

DC_BEGIN_DREEMCHEST

namespace Scene {

    //! Rendering virtual machine.
    class Rvm : public RefCounted {
    public:

        //! Displays a frame captured by a render scene.
        void                    display( const RenderFrame& frame );

        //! Creates an Rvm instance.
        static RvmPtr           create( RenderingContextWPtr context );

    private:

                                //! Constructs an Rvm instance.
                                Rvm( RenderingContextWPtr context );

        //! Executes a single command buffer.
        void                    execute( const RenderFrame& frame, const RenderCommandBuffer& commands );

        //! Unrolls a state stack an applies all state changes.
        void                    applyStates( const RenderFrame& frame, const RenderStateBlock* const * states, s32 count );

        //! Sets an alpha testing state.
        void                    switchAlphaTest( const RenderFrame& frame, const RenderState& state );

        //! Sets a depth state.
        void                    switchDepthState( const RenderFrame& frame, const RenderState& state );

        //! Sets a blending state.
        void                    switchBlending( const RenderFrame& frame, const RenderState& state );

        //! Sets a render target.
        void                    switchRenderTarget( const RenderFrame& frame, const RenderState& state );

        //! Binds a shader program to a pipeline.
        void                    switchShader( const RenderFrame& frame, const RenderState& state );

        //! Binds a constant buffer to a pipeline.
        void                    switchConstantBuffer( const RenderFrame& frame, const RenderState& state );

        //! Binds a vertex buffer to a pipeline.
        void                    switchVertexBuffer( const RenderFrame& frame, const RenderState& state );

    private:

        //! State switcher function callback.
        typedef void ( Rvm::*StateSwitch )( const RenderFrame&, const RenderState& );

        Renderer::HalWPtr       m_hal;                                      //!< Rendering HAL to be used.
        RenderingContextWPtr    m_context;                                  //!< Parent rendering context.
        StateSwitch             m_stateSwitches[RenderState::TotalStates];  //!< Function callbacks to switch states.            
    };

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Rvm_H__    */