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

#ifndef __DC_Introspection_Property_H__
#define __DC_Introspection_Property_H__

#include "Introspection.h"

DC_BEGIN_DREEMCHEST

namespace Introspection {

    //! Contains property meta-information like description, serialization flags, min/max range etc.
    struct PropertyInfo {
        CString             description;    //!< Property description string.

                            //! Constructs an empty PropertyInfo instance.
                            PropertyInfo( void );

                            //! Construct PropertyInfo instance.
                            PropertyInfo( CString description );
    };

    //! The Property class provides meta-data about a property.
    class Property : public Member {
    public:

                                //! Constructs the Property instance.
                                Property( CString name, const Type* type, const PropertyInfo& info );

        //! This type can be type casted to a property.
        virtual const Property* asProperty( void ) const DC_DECL_OVERRIDE;
        virtual Property*       asProperty( void ) DC_DECL_OVERRIDE;

        //! Returns the property value type.
        const Type*             type( void ) const;

        //! Returns property information.
        const PropertyInfo&     info( void ) const;
    
        //! Sets the property value.
        virtual void            set( Instance instance, const Variant& value ) = 0;

        //! Sets the property value only if it will be changed.
        virtual bool            update( Instance instance, const Variant& value ) = 0;

        //! Gets the property value.
        virtual Variant         get( Instance instance ) const = 0;

    private:

        const Type*             m_type; //!< The property value type.
        PropertyInfo            m_info; //!< Property information.
    };

    namespace Private {

        //! Generic property bound to a specified type.
        template<typename TObject, typename TValue, typename TPropertyValue>
        class Property : public ::DC_DREEMCHEST_NS Introspection::Property {
        public:

            //! The property getter.
            typedef TPropertyValue  ( TObject::*Getter )( void ) const;

            //! The property setter.
            typedef void            ( TObject::*Setter )( TPropertyValue );

                                    //! Constructs the Property instance.
                                    Property( CString name, Getter getter, Setter setter, const PropertyInfo& info );

        protected:

            //! Sets the property value.
            virtual void            set( Instance instance, const Variant& value ) DC_DECL_OVERRIDE;

            //! Sets the property value only if it will be changed.
            virtual bool            update( Instance instance, const Variant& value ) DC_DECL_OVERRIDE;

            //! Gets the property value.
            virtual Variant         get( Instance instance ) const DC_DECL_OVERRIDE;

        private:

            Getter                  m_getter;   //!< The property getter.
            Setter                  m_setter;   //!< The property setter.
        };

        // ** Property::Property
        template<typename TObject, typename TValue, typename TPropertyValue>
        Property<TObject, TValue, TPropertyValue>::Property( CString name, Getter getter, Setter setter, const PropertyInfo& info )
            : :: DC_DREEMCHEST_NS Introspection::Property( name, Type::fromClass<TValue>(), info )
            , m_getter( getter )
            , m_setter( setter )
        {
        }

        // ** Property::set
        template<typename TObject, typename TValue, typename TPropertyValue>
        void Property<TObject, TValue, TPropertyValue>::set( Instance instance, const Variant& value )
        {
            TValue v = value.as<TValue>();
            (reinterpret_cast<TObject*>( instance )->*m_setter)( v );
        }

        // ** Property::update
        template<typename TObject, typename TValue, typename TPropertyValue>
        bool Property<TObject, TValue, TPropertyValue>::update( Instance instance, const Variant& value )
        {
            TObject* object = reinterpret_cast<TObject*>( instance );

            TValue v = value.as<TValue>();
            TValue o = (object->*m_getter)();

            if( v == o ) {
                return false;
            }

            (object->*m_setter)( v );
            return true;
        }

        // ** Property::get
        template<typename TObject, typename TValue, typename TPropertyValue>
        Variant Property<TObject, TValue, TPropertyValue>::get( Instance instance ) const
        {
            TValue v = (reinterpret_cast<TObject*>( instance )->*m_getter)();
            return Variant::fromValue<TValue>( v );
        }

    } // namespace Private

} // namespace Introspection

DC_END_DREEMCHEST

#endif    /*    !__DC_Introspection_Property_H__    */