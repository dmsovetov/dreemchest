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

#ifndef __Composer_Qt_AbstractTreeModel_H__
#define __Composer_Qt_AbstractTreeModel_H__

#include "UserInterface.h"

namespace Ui {

	//! Basic tree model.
	class QAbstractTreeModel : public QAbstractItemModel {

		Q_OBJECT

	public:

								//! Constructs QAbstractTreeModel instance.
								explicit QAbstractTreeModel( int columnCount = 1, QObject* parent = NULL );
		virtual					~QAbstractTreeModel( void );

		//! Clears the model.
		void					clear( void );

		//! Returns the row count inside the specified parent index.
		virtual int				rowCount( const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;

		//! Returns the column count inside the specified parent index.
		virtual int				columnCount( const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;

	protected:

		//! Returns the model index for specified row & column.
		virtual QModelIndex		index( int row, int column, const QModelIndex& parent = QModelIndex() ) const Q_DECL_OVERRIDE;

		//! Returns the parent item index for a specified child.
		virtual QModelIndex		parent( const QModelIndex& child ) const Q_DECL_OVERRIDE;

		//! Returns supported drop actions.
		virtual Qt::DropActions	supportedDropActions( void ) const Q_DECL_OVERRIDE;

		//! Returns the MIME data from index.
		virtual QMimeData*		mimeData( const QModelIndexList& indexes ) const Q_DECL_OVERRIDE;

		//! Handles the drop operation.
		virtual bool			dropMimeData( const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent ) Q_DECL_OVERRIDE;

		//! Handles move operation.
		virtual bool			moveRows( const QModelIndex& sourceParent, int sourceRow, int count, const QModelIndex& destinationParent, int destinationChild ) Q_DECL_OVERRIDE;

	protected:

		//! Basic tree item.
		class TreeItem {
		public:
								//! Constructs TreeItem instance
								TreeItem( TreeItem* parent = NULL );
			virtual				~TreeItem( void );

			//! Returns the tree item row.
			int					row( void ) const;

			//! Clears the tree item children.
			void				clear( void );

			//! Returns child item at index.
			TreeItem*			child( int index ) const;

			//! Returns parent item.
			TreeItem*			parent( void ) const;

			//! Returns the total number of children.
			int					childCount( void ) const;

			//! Append a new child.
			void				addChild( TreeItem* item );

			//! Removes child.
			void				removeChild( TreeItem* item );

			//! Sets the item row.
			void				setRow( int value );

			//! Moves this item from one parent to another.
			void				setParent( TreeItem* parent, int row );

		private:

			TreeItem*			m_parent;	//!< Parent tree item.
			QVector<TreeItem*>	m_children;	//!< Item children.
		};

		//! Constructs new item instance.
		virtual TreeItem*		createItem( void ) const = 0;

		//! Processes the item movement action.
		virtual bool			moveItem( TreeItem* sourceParent, TreeItem* destinationParent, TreeItem* item, int destinationRow ) const;

		//! Removes item from a model.
		virtual void			removeItem( TreeItem* item );

		//! Adds item to a model.
		virtual void		addItem( TreeItem* item, TreeItem* parent );

		//! Returns the tree item instance by index.
		TreeItem*				itemAtIndex( const QModelIndex& index ) const;

		//! Returns the item's parent index.
		QModelIndex				parentIndexFromItem( const TreeItem* item ) const;

		//! Returns the item's index.
		QModelIndex				indexFromItem( const TreeItem* item ) const;

		//! Returns root item.
		TreeItem*				root( void ) const;

	private:

		int						m_columnCount;	//!< The total number of columns per item.
		mutable TreeItem		m_root;			//!< Root tree item.
	};

	//! Generic tree model.
	template<typename TItemData>
	class QGenericTreeModel : public QAbstractTreeModel {
	public:

								//! Constructs QGenericTreeModel instance.
								explicit QGenericTreeModel( int columnCount = 1, QObject* parent = NULL )
									: QAbstractTreeModel( columnCount, parent ) {}

		//! Returns the item data at specified index.
		const TItemData&		dataAt( const QModelIndex& index ) const;

	public:

		//! Subclass the TreeItem type and embed the item data.
		class Item : public TreeItem {
		public:

								//! Constructs Item instance.
								Item( TreeItem* parent = NULL )
									: TreeItem( parent ) {}

			//! Returns item data.
			const TItemData&	data( void ) const { return m_data; }
			TItemData&			data( void ) { return m_data; }

		protected:

			TItemData			m_data;	//! Actual item data.
		};

		//! Creates the tree item instance.
		virtual TreeItem*		createItem( void ) const Q_DECL_OVERRIDE;

		//! Creates the item with data,
		Item*					createItem( const TItemData& data ) const;

		//! Handles the item movement with a type-casted item.
		virtual bool			moveItem( TreeItem* sourceParent, TreeItem* destinationParent, TreeItem* item, int destinationRow ) const Q_DECL_OVERRIDE;

		//! Returns item at index.
		Item*					itemAtIndex( const QModelIndex& index ) const;

		//! Handles the type-casted item movement.
		virtual bool			moveItem( Item* sourceParent, Item* destinationParent, Item* item, int destinationRow ) const { return true; }
	};

	// ** QGenericTreeModel::dataAt
	template<typename TItemData>
	const TItemData& QGenericTreeModel<TItemData>::dataAt( const QModelIndex& index ) const
	{
		static TItemData kInvalid;

		if( Item* item = itemAtIndex( index ) ) {
			return item->data();
		}

		return kInvalid;
	}

	// ** QGenericTreeModel::createItem
	template<typename TItemData>
	QAbstractTreeModel::TreeItem* QGenericTreeModel<TItemData>::createItem( void ) const
	{
		return new Item;
	}

	// ** QGenericTreeModel::createItem
	template<typename TItemData>
	typename QGenericTreeModel<TItemData>::Item* QGenericTreeModel<TItemData>::createItem( const TItemData& data ) const
	{
		Item* item = static_cast<Item*>( createItem() );
		item->data() = data;
		return item;
	}

	// ** QGenericTreeModel::itemAtIndex
	template<typename TItemData>
	typename QGenericTreeModel<TItemData>::Item* QGenericTreeModel<TItemData>::itemAtIndex( const QModelIndex& index ) const
	{
		return static_cast<Item*>( QAbstractTreeModel::itemAtIndex( index ) );
	}

	// ** QGenericTreeModel::moveItem
	template<typename TItemData>
	bool QGenericTreeModel<TItemData>::moveItem( TreeItem* sourceParent, TreeItem* destinationParent, TreeItem* item, int destinationRow ) const
	{
		return moveItem( static_cast<Item*>( sourceParent ), static_cast<Item*>( destinationParent ), static_cast<Item*>( item ), destinationRow );
	}

} // namespace Ui

#endif	/*	!__Composer_Qt_AbstractTreeModel_H__	*/