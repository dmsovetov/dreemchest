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

#ifndef __DC_Scene_RenderSystem_H__
#define __DC_Scene_RenderSystem_H__

#include "../SceneSystem.h"

#include "../../Components/Rendering.h"	//!< Include the rendering components here
#include "../../Components/Transform.h"	//!< Everything that is rendered should have a transform component, so include this file here

DC_BEGIN_DREEMCHEST

namespace Scene {

	//! A helper struct to wrap HAL & 2D renderer instances.
	struct Renderers {
		Renderer::HalPtr		m_hal;			//!< Rendering HAL.
		Renderer::Renderer2DPtr	m_renderer2d;	//!< 2D renderer.

								//! Constructs empty Renderers instance.
								Renderers( void )
									{}

								//! Constructs Renderer instance.
								Renderers( const Renderer::HalPtr& hal )
									: m_hal( hal ) {}

								//! Constructs Renderer2D instance.
								Renderers( const Renderer::Renderer2DPtr& renderer2d )
									: m_renderer2d( renderer2d ) {}
	};
	 
	//! Base class for all render systems.
	class RenderSystemBase : public Ecs::EntitySystem {
	public:

								//! Constructs RenderSystemBase instance
								RenderSystemBase( Ecs::Entities& entities, const String& name, const Ecs::Aspect& aspect, const Renderers& renderers )
									: EntitySystem( entities, name, aspect ), m_renderers( renderers ) {}

		//! Adds a new render pass to this system.
		template<typename T>
		void					addPass( void );

	protected:

		//! Renders all nested render passes using the camera.
		virtual void			process( u32 currentTime, f32 dt, Ecs::EntityPtr& entity );

	protected:

		//! Container type to store nested render passes.
		typedef List<RenderPassBasePtr> RenderPasses;

		Renderers				m_renderers;	//!< Renderers wraper.
		RenderPasses			m_passes;		//!< Render passes.
	};

	// ** RenderSystemBase::addPass
	template<typename T>
	void RenderSystemBase::addPass( void )
	{
		m_passes.push_back( DC_NEW T( m_entities, m_renderers ) );
	}

	//! Generic render system to subclass user-defined rendering systems from
	template<typename TRender>
	class RenderSystem : public RenderSystemBase {
	public:

								//! Constructs RenderSystem instance
								RenderSystem( Ecs::Entities& entities, const String& name, const Renderers& renderers )
									: RenderSystemBase( entities, name, Ecs::Aspect::all<Camera, Transform, TRender>(), renderers ) {}
	};

	//! Generic single pass renderer
	template<typename TRenderer, typename TPass>
	class SinglePassRenderer : public RenderSystem<TRenderer> {
	public:

								SinglePassRenderer( Ecs::Entities& entities, const Renderers& renderers )
									: RenderSystem( entities, "SinglePassRenderer", renderers ) { addPass<TPass>(); }
	};

} // namespace Scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_RenderSystem_H__    */