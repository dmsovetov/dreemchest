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

#ifndef __DC_Scene_Systems_Transofrm_H__
#define __DC_Scene_Systems_Transofrm_H__

#include "SceneSystem.h"

#include "../Components/Transform.h"

DC_BEGIN_DREEMCHEST

namespace scene {

	//! The Rotator2D system
	class Rotator2DSystem : public SceneSystem2<Rotator2D, Transform2D> {
	public:

							//! Constructs a Renderer instance.
							Rotator2DSystem( ecs::Entities& entities )
								: SceneSystem2( entities, "Rotator2D" ) {}

		//! Renders a single entity with a rotator
		virtual void		process( u32 currentTime, f32 dt, SceneObject& sceneObject, Rotator2D& rotator, Transform2D& transform );
	};

} // namespace scene

DC_END_DREEMCHEST

#endif    /*    !__DC_Scene_Systems_Transofrm_H__    */