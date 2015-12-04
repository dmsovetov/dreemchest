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

#ifndef __DC_Composer_Qt_DocumentDock_H__
#define __DC_Composer_Qt_DocumentDock_H__

#include "UserInterface.h"

namespace Ui {

	//! Subclass of a QDockWidget.
	class QDocumentDock : public QDockWidget {

		Q_OBJECT

	public:

											//! Constructs the QDocumentDock instance.
											QDocumentDock( DocumentDock* document, const QString& title, QWidget* parent );
	private:

		//! Handles the closed event.
		virtual void						closeEvent( QCloseEvent *e ) Q_DECL_OVERRIDE;

	private slots:

		//! Handles the visibility changed signal
		void								visibilityChanged( bool visible );

	private:

		DocumentDock*						m_document;	//!< Parent document dock.
	};

	//! Document dock Qt implementation.
	class DocumentDock : public UserInterface<IDocumentDock, QDocumentDock> {
	friend class QDocumentDock;
	public:

											//! Constructs the DocumentDock instance.
											DocumentDock( IMainWindowWPtr mainWindow, Editors::AssetEditorPtr assetEditor, const String& title, QWidget* parent );

		//! Returns the rendering frame used for this document dock.
		virtual IRenderingFrameWPtr			renderingFrame( void );

		//! Sets the rendering frame.
		virtual void						setRenderingFrame( IRenderingFramePtr value );

		//! Returns an attached asset editor.
		virtual Editors::AssetEditorWPtr	assetEditor( void ) const;

	private:

		//! Sets this document as active.
		void								activateDocument( void );

		//! Closes this document.
		bool								closeDocument( void );

	private:

		IMainWindowWPtr						m_mainWindow;	//!< Parent main window.
		Editors::AssetEditorPtr				m_assetEditor;	//!< Asset editor attached to this document dock.
	};

} // namespace Ui

#endif	/*	!__DC_Composer_Qt_DocumentDock_H__	*/