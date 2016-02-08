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

#ifndef __DC_Composer_AssetManager_H__
#define __DC_Composer_AssetManager_H__

#include "Project.h"
#include "../Importers/AssetImporter.h"

DC_BEGIN_COMPOSER

	//! Manages the project cache & available assets.
	class AssetManager : public QObject {
	friend class Project;

        Q_OBJECT

	public:

		//! Returns asset bundle.
		const Assets::Assets&		assets( void ) const;
        Assets::Assets&             assets( void );

		//! Registers the mapping from extension to asset type.
		void						registerExtension( const String& ext, Assets::AssetType type );

		//! Returns asset type by extension.
		Assets::AssetType			assetTypeFromExtension( const String& ext ) const;

	private:

									//! Constructs AssetManager instance.
									AssetManager( QObject* parent, const Io::Path& path, AssetFileSystemModelQPtr assetFileSystem );

		//! Returns asset cache path.
		Io::Path					cacheFileFromUuid( const String& uuid ) const;

		//! Returns asset cache folder.
		Io::Path					cacheFolderFromUuid( const String& uuid ) const;

		//! Creates new asset instance by a specified extension.
		Assets::AssetHandle			createAssetForFile( const FileInfo& fileInfo );

        //! Parses an asset from a meta data.
        Assets::AssetHandle         parseAssetFromData( const Io::KeyValue& kv );

        //! Creates an asset instance with specified asset type and id.
        Assets::AssetHandle         createAsset( Assets::AssetType type, const Assets::AssetId& id );

    private slots:

        //! Adds an asset to cache.
		void						addAssetFile( const FileInfo& file );

		//! Removes an asset from cache.
		void						removeAssetFromCache( const QString& uuid, const FileInfo& file );

		//! Performs caching of a changed asset.
		bool						updateAssetCache( const QString& uuid, const FileInfo& file );

	private:

		//! Alias the asset importer factory type.
		typedef AbstractFactory<Importers::AssetImporter, String> AssetImporterFactory;

        //! Alias the asset format factory type.
        typedef AbstractFactory<Assets::AbstractAssetFileFormat, Assets::AssetType> AssetFormatFactory;

		//! Alias the ext to asset type mapping.
		typedef Map<String, Assets::AssetType> AssetTypes;

		Io::Path					m_path;				//!< Root cache folder path.
		AssetFileSystemModelQPtr    m_assetFileSystem;	//!< Asset file system model to use.
		AssetImporterFactory		m_assetImporters;	//!< Asset importer factory.
        AssetFormatFactory          m_assetFormats;     //!< Asset format factory.
		AssetTypes					m_assetTypes;		//!< Registered asset types.
		Assets::Assets		        m_assets;			//!< Asset bundle.
	};

DC_END_COMPOSER

#endif	/*	!__DC_Composer_AssetManager_H__	*/