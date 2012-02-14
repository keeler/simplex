#ifndef PLANE_HPP
#define PLANE_HPP

#include "Vector3f.hpp"

class Plane
{
	public:
		Plane( const Vector3f & normal, const Vector3f & point );

		Vector3f getNormal() const { return mNormal; };

		bool isInPositiveHalfSpace( const Vector3f & point ) const { return ( mNormal.dot( point ) > mPlaneConstant ); };
		bool isInNegativeHalfSpace( const Vector3f & point ) const { return ( mNormal.dot( point ) < mPlaneConstant ); };
		bool isInPlane( const Vector3f & point ) const { return ( mNormal.dot( point ) == mPlaneConstant ); }

	private:
		Vector3f mNormal;
		float    mPlaneConstant;	// Often referred to as "d", ax + by + cz = d
};

#endif
