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

#ifndef __DC_Composer_VisualEditor_H__
#define __DC_Composer_VisualEditor_H__

#include "AssetEditor.h"

DC_BEGIN_COMPOSER

namespace Editors {

	//! Cursor interface emits events when mouse pressed/released/moved.
	class Cursor : public event::RefCountedEventEmitter {
	public:

		//! This event is emitted when mouse is pressed.
		struct Pressed {
						Pressed( CursorWPtr cursor, u8 buttons )
							: cursor( cursor ), buttons( buttons ) {}
			CursorWPtr	cursor;
			FlagSet8	buttons;
		};

		//! This event is emitted when mouse is released.
		struct Released {
						Released( CursorWPtr cursor, u8 buttons )
							: cursor( cursor ), buttons( buttons ) {}
			CursorWPtr	cursor;
			FlagSet8	buttons;
		};

		//! Returns cursor X position.
		s32				x( void ) const;

		//! Returns cursor Y position.
		s32				y( void ) const;

		//! Returns cursor view ray.
		const Ray&		ray( void ) const;

		//! Returns button mask.
		const FlagSet8&	buttons( void ) const;

		//! Set the cursor position.
		void			setPosition( s32 x, s32 y );

		//! Sets button mask.
		void			setButtons( const FlagSet8& mask );

		//! sets the cursor view ray.
		void			setRay( const Ray& value );

	private:

		s32				m_x;		//!< Cursor X position.
		s32				m_y;		//!< Cursor Y position.
		Ray				m_ray;		//!< Cursor view ray.
		FlagSet8		m_buttons;	//!< Mouse buttons mask.
	};

	//! Base class for all visual asset editors.
	class VisualEditor : public AssetEditor {
	friend class RenderingFrameDelegate;
	public:

										//! Constructs the VisualEditor instance.
										VisualEditor( void );

		//! Performs the visual editor initialization.
		virtual bool					initialize( Project::ProjectWPtr project, const Scene::AssetPtr& asset, Ui::IDocumentWPtr document ) DC_DECL_OVERRIDE;

		//! Returns rendering HAL.
		Renderer::HalWPtr				hal( void ) const;

		//! Returns cursor binding.
		CursorWPtr						cursor( void ) const;

		//! Returns the background color.
		const Rgba&						backgroundColor( void ) const;

	protected:

		//! Update the editor scene.
		virtual void					update( f32 dt );

		//! Handles the viewport resize.
		virtual void					handleResize( s32 width, s32 height );

		//! Handles the mouse press event.
		virtual void					handleMousePress( s32 x, s32 y, u8 button );

		//! Handles the mouse release event.
		virtual void					handleMouseRelease( s32 x, s32 y, u8 button );

		//! Handles the mouse move event.
		virtual void					handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons );

		//! Handles the mouse wheel event.
		virtual void					handleMouseWheel( s32 delta ) {}

		//! Handles the key press event.
		virtual void					handleKeyPress( Platform::Key key ) {}

		//! Handles the key release event.
		virtual void					handleKeyRelease( Platform::Key key ) {}

		//! Handles the drag enter event.
		virtual bool					handleDragEnter( IMimeDataWPtr mime ) { return false; }

		//! Handles the drag move event.
		virtual void					handleDragMove( IMimeDataWPtr mime, s32 x, s32 y ) {}

		//! Handles the drop event.
		virtual void					handleDrop( IMimeDataWPtr mime, s32 x, s32 y ) {}

		//! Handles the context menu event.
		virtual void					handleContextMenu( Ui::IMenuWPtr menu );

		//! Handles the focus in event.
		virtual void					handleFocusIn( void );

		//! Handles the focus out event.
		virtual void					handleFocusOut( void );

		//! Begins the frame rendering.
		virtual void					beginFrameRendering( void );

		//! Performs actual frame rendering.
		virtual void					render( f32 dt ) {}

		//! Ends the frame rendering.
		virtual void					endFrameRendering( void );

	private:

		Renderer::HalPtr				m_hal;						//!< Rendering HAL instance.
		Rgba							m_backgroundColor;			//!< The background color.
		Ui::IRenderingFrameDelegatePtr	m_renderingFrameDelegate;	//!< Rendering frame delegate.
		CursorPtr						m_cursor;					//!< Mouse cursor binding.
	};

	//! Visual editor rendering frame delegate.
	class RenderingFrameDelegate : public Ui::IRenderingFrameDelegate {
	public:

										//! Constructs the RenderingFrameDelegate instance.
										RenderingFrameDelegate( VisualEditorWPtr editor );

		//! Handles the update event.
		virtual void					handleUpdate( f32 dt ) DC_DECL_OVERRIDE;

		//! Handles the resize event.
		virtual void					handleResize( s32 width, s32 height ) DC_DECL_OVERRIDE;

		//! Handles the mouse pressed event.
		virtual void					handleMousePress( s32 x, s32 y, u8 button ) DC_DECL_OVERRIDE;

		//! Handles the mouse released event.
		virtual void					handleMouseRelease( s32 x, s32 y, u8 button ) DC_DECL_OVERRIDE;

		//! Handles the mouse moved event.
		virtual void					handleMouseMove( s32 x, s32 y, s32 dx, s32 dy, u8 buttons ) DC_DECL_OVERRIDE;

		//! Handles the mouse wheel event.
		virtual void					handleMouseWheel( s32 delta ) DC_DECL_OVERRIDE;

		//! Handles the focus in event.
		virtual void					handleFocusIn( void ) DC_DECL_OVERRIDE;

		//! Handles the focus out event.
		virtual void					handleFocusOut( void ) DC_DECL_OVERRIDE;

		//! Handles the key pressed event.
		virtual void					handleKeyPress( Platform::Key key ) DC_DECL_OVERRIDE;

		//! Handles the key released event.
		virtual void					handleKeyRelease( Platform::Key key ) DC_DECL_OVERRIDE;

		//! Handles the context menu event.
		virtual void					handleContextMenu( Ui::IMenuWPtr menu ) DC_DECL_OVERRIDE;

		//! Handles the drag enter event.
		virtual bool					handleDragEnter( IMimeDataWPtr mime ) DC_DECL_OVERRIDE;

		//! Handles the drag move event.
		virtual void					handleDragMove( IMimeDataWPtr mime, s32 x, s32 y ) DC_DECL_OVERRIDE;

		//! Handles the drop move event.
		virtual void					handleDrop( IMimeDataWPtr mime, s32 x, s32 y ) DC_DECL_OVERRIDE;

	private:

		VisualEditorWPtr				m_editor;	//!< Parent visual editor.
	};

} // namespace Editors

DC_END_COMPOSER

#endif	/*	!__DC_Composer_VisualEditor_H__	*/