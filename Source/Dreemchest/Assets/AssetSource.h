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

#ifndef __DC_Assets_AssetSource_H__
#define __DC_Assets_AssetSource_H__

#include "Assets.h"

DC_BEGIN_DREEMCHEST

namespace Assets {

    //! Base class for all asset sources.
    class Source {
    public:

        virtual         ~Source( void ) {}

        //! Loads data to a specified asset data handle.
        virtual bool    parse( Assets& assets, Handle asset ) = 0;

        //! Returns the last modification timestamp of an asset source.
        virtual u32     lastModified( void ) const = 0;
    };

    //! Asset file source used for loading assets from files.
    class AbstractFileSource : public Source {
    public:

                        //! Constructs AbstractFileSource instance.
                        AbstractFileSource( void );

        //! Opens the file stream and loads data from it.
        virtual bool    parse( Assets& assets, Handle asset ) DC_DECL_OVERRIDE;

        //! Returns the last file modification time stamp.
        virtual u32     lastModified( void ) const DC_DECL_OVERRIDE;

        //! Sets the last file modification timestamp.
        void            setLastModified( u32 value );

        //! Returns the source asset file name.
        const String&   fileName( void ) const;

        //! Sets the source asset file name.
        void            setFileName( const String& value );

    protected:

        //! This virtual method is used to dispatch the loading process to actual asset loading implementation.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, Handle asset ) = 0;

    private:

        String          m_fileName;     //!< Source file name to load asset from.
        u32             m_lastModified; //!< Timestamp when the file was modified last time.
    };

    //! Generic base class for all asset file sources.
    template<typename TAsset>
    class FileSource : public AbstractFileSource {
    protected:

        //! Type casts an asset handle and dispatches the loading process to an implementation.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, Handle asset ) DC_DECL_OVERRIDE;

        //! Performs an asset data parsing from a stream.
        virtual bool    parseFromStream( Io::StreamPtr stream, Assets& assets, TAsset& asset ) = 0;
    };

    // ** FileSource::parseFromStream
    template<typename TAsset>
    bool FileSource<TAsset>::parseFromStream( Io::StreamPtr stream, Assets& assets, Handle asset )
    {
        WriteLock<TAsset> lock = asset.writeLock<TAsset>();
        TAsset&           data = *lock;
        bool result = parseFromStream( stream, assets, data );
        return result;
    }

} // namespace Assets

DC_END_DREEMCHEST

#endif    /*    !__DC_Assets_AssetSource_H__    */