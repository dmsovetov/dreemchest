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

#ifndef __DC_Ecs_Component_H__
#define __DC_Ecs_Component_H__

#include "../Ecs.h"

DC_BEGIN_DREEMCHEST

namespace Ecs {

	//! A base class for internal system data attached to a components.
	struct InternalBase : public RefCounted {
	};

	//! A template class used for declaring system-specific internal data.
	template<typename T>
	struct Internal : public InternalBase {
		typedef StrongPtr<T> Ptr;	//!< The internal data pointer.
	};

	//! Internal component flags.
	enum ComponentFlags {
		IsEnabled = BIT( 31 )	//!< Indicates that a component is enabled.
	};

	//! A base class for all components.
	/*!
	A component is all the data for one aspect of the entity. Component is just a plain
	data and doesn't contain any processing logic.
	*/
	class ComponentBase : public Io::Serializable {
	friend class Entity;
	public:

                                    ClassEnableTypeInfoSuper( ComponentBase, Io::Serializable )

									//! Constructs ComponentBase instance.
									ComponentBase( void )
										: m_flags( IsEnabled ) {}

		//! Sets the internal data.
		template<typename T>
		void						setInternal( InternalBase* value );

		//! Returns the internal data.
		template<typename T>
		typename Internal<T>::Ptr	internal( void ) const;

		//! Returns component flags.
		u32							flags( void ) const;

		//! Sets component flags.
		void						setFlags( u32 value );

		//! Returns true if component is enabled.
		bool						isEnabled( void ) const;

        //! Returns parent entity instance.
        EntityWPtr                  entity( void ) const;

    #if DC_ECS_ENTITY_CLONING
        virtual ComponentPtr        deepCopy( void ) const;
    #endif  /*  DC_ECS_ENTITY_CLONING   */

	#ifndef DC_ECS_NO_SERIALIZATION
		//! Reads component from a storage.
		virtual void		        read( const Io::Storage* storage ) DC_DECL_OVERRIDE;

		//! Writes component to a storage.
		virtual void		        write( Io::Storage* storage ) const DC_DECL_OVERRIDE;

		//! Writes this component to a key-value archive.
		virtual void                serialize( SerializationContext& ctx, Io::KeyValue& ar ) const;

		//! Reads this component from a key-value archive.
		virtual void		        deserialize( SerializationContext& ctx, const Io::KeyValue& value );
	#endif	/*	!DC_ECS_NO_SERIALIZATION	*/

	protected:

		//! Sets the component's enabled flag.
		void						setEnabled( bool value );

        //! Sets the component's parent entity.
        void                        setParentEntity( const EntityWPtr& value );

	protected:

		//! Container type to store internal system state inside a component.
		typedef Map< TypeIdx, StrongPtr<InternalBase> > InternalDataHolder;

        EntityWPtr                  m_entity;   //!< Entity instance this component is attached to.
		InternalDataHolder			m_internal;	//!< The internal data.
		FlagSet32					m_flags;	//!< Component flags.
	};

	// ** ComponentBase::setInternal
	template<typename T>
	inline void ComponentBase::setInternal( InternalBase* value )
	{
		m_internal[TypeIndex<T>::idx()] = value;
	}

	// ** ComponentBase::internal
	template<typename T>
	inline typename Internal<T>::Ptr ComponentBase::internal( void ) const
	{
		InternalDataHolder::const_iterator i = m_internal.find( TypeIndex<T>::idx() );

		if( i != m_internal.end() ) {
			return i->second;
		}

		return typename Internal<T>::Ptr();
	}

#if DC_ECS_ENTITY_CLONING
    // ** ComponentBase::deepCopy
    inline ComponentPtr ComponentBase::deepCopy( void ) const
    {
        return ComponentPtr();
    }
#endif  /*  DC_ECS_ENTITY_CLONING   */

#ifndef DC_ECS_NO_SERIALIZATION
	// ** ComponentBase::read
	inline void ComponentBase::read( const Io::Storage* storage )
	{
	    Io::KeyValue ar;
        ar.read( storage );

        SerializationContext ctx( NULL );
        deserialize( ctx, ar );
	}

	// ** ComponentBase::read
	inline void ComponentBase::write( Io::Storage* storage ) const
	{
        SerializationContext ctx( NULL );
        Io::KeyValue ar;

        serialize( ctx, ar );
	    ar.write( storage );
	}

	// ** ComponentBase::serialize
	inline void ComponentBase::serialize( SerializationContext& ctx, Io::KeyValue& ar ) const
	{
        ar = Io::KeyValue::kNull;
		log::warn( "Component::serialize : is not implemented for '%s'\n", typeName() );
	}

	// ** ComponentBase::deserialize
	inline void ComponentBase::deserialize( SerializationContext& ctx, const Io::KeyValue& value )
	{
		log::warn( "Component::deserialize : is not implemented for '%s'\n", typeName() );
	}
#endif	/*	!DC_ECS_NO_SERIALIZATION	*/

	// ** ComponentBase::flags
	inline u32 ComponentBase::flags( void ) const
	{
		return m_flags;
	}

	// ** ComponentBase::setFlags
	inline void ComponentBase::setFlags( u32 value )
	{
		m_flags = value;
	}

	// ** ComponentBase::isEnabled
	inline bool ComponentBase::isEnabled( void ) const
	{
		return m_flags.is( IsEnabled );
	}

	// ** ComponentBase::setEnabled
	inline void ComponentBase::setEnabled( bool value )
	{
		m_flags.set( IsEnabled, value );
	}

    // ** ComponentBase::entity
    inline EntityWPtr ComponentBase::entity( void ) const
    {
        return m_entity;
    }

    // ** ComponentBase::setParentEntity
    inline void ComponentBase::setParentEntity( const EntityWPtr& value )
    {
        DC_BREAK_IF( value.valid() && m_entity.valid() && m_entity != value );
        m_entity = value;
    }

	//! Generic component class.
	template<typename T>
	class Component : public ComponentBase {
	public:

		                        ClassEnableTypeInfoSuper( T, ComponentBase )


		//! Weak pointer type.
		typedef WeakPtr<T>		WPtr;

		//! Strong pointer type.
		typedef StrongPtr<T>	Ptr;

		static const Bitset&	bit( void ) { static Bitset result = Bitset::withSingleBit( TypeIndex<T>::idx() ); return result; }

    #if DC_ECS_ENTITY_CLONING
        virtual ComponentPtr    deepCopy( void ) const DC_DECL_OVERRIDE;
    #endif  /*  DC_ECS_ENTITY_CLONING   */
	};

#if DC_ECS_ENTITY_CLONING
    // ** Component::deepCopy
    template<typename T>
    ComponentPtr Component<T>::deepCopy( void ) const
    {
        T* instance = DC_NEW T;
        *instance = *static_cast<const T*>( this );
        instance->setParentEntity( NULL );
        return instance;
    }
#endif  /*  DC_ECS_ENTITY_CLONING   */

} // namespace Ecs

DC_END_DREEMCHEST

#endif	/*	!__DC_Ecs_Component_H__	*/
