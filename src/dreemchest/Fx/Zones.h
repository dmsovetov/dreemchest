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

#ifndef __DC_Fx_ParticleZones_H__
#define __DC_Fx_ParticleZones_H__

#include "Parameter.h"

DC_BEGIN_DREEMCHEST

namespace Fx {

    //! Base class for zone types.
    class Zone : public RefCounted {
    public:

		//! The point with position and direction generated by a zone.
		struct Point {
			Vec3			position;	//!< Point position.
			Vec3			direction;	//!< Direction at point.

							//! Constructs Point instance.
							Point( const Vec3& position, const Vec3& direction = Vec3( 0.0f, 0.0f, 0.0f ) )
								: position( position ), direction( direction ) {}
		};

        virtual             ~Zone( void ) {}

		//! Creates the zone by type.
        static ZonePtr      create( ZoneType type );

		//! Returns the zone type.
        virtual ZoneType	type( void ) const											= 0;

		//! Generates the random point inside the zone.
        virtual Point       generateRandomPoint( f32 scalar, const Vec3& center ) const	= 0;
    };

    //! Disk zone generates points between the inner and outer radii.
    class DiskZone : public Zone {
    public:

							//! Constructs the DiskZone instance.
							DiskZone( f32 innerRadius = 0.0f, f32 outerRadius = 0.0f );

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point inside the disk.
        virtual Point       generateRandomPoint( f32 scalar, const Vec3& center ) const;

    private:

        FloatParameter		m_innerRadius;	//!< Zone inner radius.
        FloatParameter		m_outerRadius;	//!< Zone outer radius.
    };

	//! Box zone generates points inside the box.
	class BoxZone : public Zone {
	public:

							//! Constructs the BoxZone instance.
							BoxZone( f32 width = 0.0f, f32 height = 0.0f, f32 depth = 0.0f );

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point inside the box.
        virtual Point       generateRandomPoint( f32 scalar, const Vec3& center ) const;

    private:

        FloatParameter		m_width;	//!< Zone width.
        FloatParameter		m_height;	//!< Zone height.
		FloatParameter		m_depth;	//!< Zone depth.
	};

	//! Hemi-sphere zone generates point on the upper hemisphere.
	class HemiSphereZone : public Zone {
	public:

							//! Constructs the HemiSphereZone instance.
							HemiSphereZone( f32 radius = 0.0f );

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point inside the hemisphere.
        virtual Point       generateRandomPoint( f32 scalar, const Vec3& center ) const;

	private:

		 FloatParameter		m_radius;	//!< Hemisphere raius.
	};

	//! Sphere zone generates point inside the sphere or on it's shell.
	class SphereZone : public Zone {
	public:

							//! Constructs the SphereZone instance.
							SphereZone( f32 radius = 0.0f );

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point inside the sphere.
        virtual Point       generateRandomPoint( f32 scalar, const Vec3& center ) const;

	private:

		 FloatParameter		m_radius;	//!< Hemisphere raius.
	};

    //! Line zone generates points on a line segment.
    class LineZone : public Zone {
    public:

							//! Constructs the LineZone instance.
							LineZone( f32 length = 0.0f );

        //! Returns the zone type.
        virtual ZoneType	type( void ) const;

		//! Generates the random point on line segment.
        virtual Point       generateRandomPoint( f32 scalar, const Vec3& center ) const;

    private:

        FloatParameter		m_angle;	//!< Line segment rotation.
        FloatParameter		m_length;	//!< Line segment length.
    };

} // namespace Fx

DC_END_DREEMCHEST

#endif		/*	!__DC_Fx_ParticleZones_H__	*/