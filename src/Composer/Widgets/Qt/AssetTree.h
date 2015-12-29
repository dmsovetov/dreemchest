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

#ifndef __DC_Composer_Qt_AssetTree_H__
#define __DC_Composer_Qt_AssetTree_H__

#include "Widget.h"

DC_BEGIN_COMPOSER

namespace Ui {

	//! Asset selector widget.
	class QAssetSelector : public QWidget {

		Q_OBJECT
		Q_PROPERTY( Scene::AssetWPtr value READ value WRITE setValue NOTIFY valueChanged USER true )

	Q_SIGNALS:

		//! Emitted when the selected asset was changed.
		void				valueChanged( void );

	public:

							//! Constructs QAssetSelector widget.
							QAssetSelector( u32 mask = ~0, QWidget* parent = NULL );

		//! Returns the selected asset.
		Scene::AssetWPtr	value( void ) const;

		//! Sets the selected asset.
		void				setValue( const Scene::AssetWPtr& value );

	private:

		//! Event filter to handle drop events.
		virtual bool		eventFilter( QObject* target, QEvent* e ) Q_DECL_OVERRIDE;

	private:

		QLineEdit*			m_line;		//!< Asset selector line edit.
		QToolButton*		m_button;	//!< Asset selector button.
		u32					m_mask;		//!< Accepted asset types.
		Scene::AssetWPtr	m_asset;	//!< The selected asset.
	};

	//! Subclass of a QTreeView to extend the context menu & key press behaviour.
	class QAssetTree : public QTreeView {

		Q_OBJECT

	public:

									//! Constructs asset tree bound to a specified path.
									QAssetTree( Project::ProjectWPtr project );

		//! Returns the selected items.
		FileInfoArray				selection( void ) const;

		//! Expands the selected items.
		void						expandSelectedItems( void );

		//! Sets the parent asset tree.
		void						setParent( IAssetTreeWPtr value );

		//! Sets asset tree model.
		void						setModel( AssetsModelWPtr value );

        //! Returns asset model.
        AssetsModelWPtr             model( void ) const;

	protected:

		//! Handles the deletion and renaming items.
		virtual void				keyPressEvent( QKeyEvent* e ) Q_DECL_OVERRIDE;

		//! Handles the context menu requests.
		virtual void				contextMenuEvent( QContextMenuEvent* e ) Q_DECL_OVERRIDE;

		//! Resets the selection changed flag.
		virtual void				mousePressEvent( QMouseEvent* e ) Q_DECL_OVERRIDE;

		//! Handles the asset selection.
		virtual void				mouseReleaseEvent( QMouseEvent* e ) Q_DECL_OVERRIDE;

		//! Binds the selected asset to an object inspector.
		void						bindToInspector( const QModelIndexList& selection );

	private slots:

		//! Handles the doubleClicked signal.
		void						itemDoubleClicked( const QModelIndex& index );

		//! Handles the selectionChanged signal of QItemSelectionModel.
		void						selectionChanged( const QItemSelection& selected, const QItemSelection& deselected );

	private:

		IAssetTreeWPtr				m_parent;			//!< Parent AssetTree instance.
		Project::ProjectWPtr		m_project;			//!< Parent project instance.
		FilteredAssetsModelPtr	    m_proxy;			//!< Filtered assets model to be used.
		bool						m_selectionChanged;	//!< This flag indicates that selection was changed.
	};

	//! Asset tree widget.
	class AssetTree : public PrivateInterface<IAssetTree, QAssetTree> {
	public:

									//! Constructs asset tree bound to a specified path.
									AssetTree( Project::ProjectWPtr project );

		//! Returns the selected items.
		virtual FileInfoArray		selection( void ) const DC_DECL_OVERRIDE;

		//! Expands the selected items.
		virtual void				expandSelectedItems( void ) DC_DECL_OVERRIDE;

		//! Sets asset tree model.
		virtual void				setModel( AssetsModelWPtr value ) DC_DECL_OVERRIDE;
	};

} // namespace Ui

DC_END_COMPOSER

#endif	/*	!__DC_Composer_Qt_AssetTree_H__	*/