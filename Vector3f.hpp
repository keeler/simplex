#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <iostream>
#include <cmath>

class Vector3f
{
	public:
		Vector3f();
		Vector3f( float x, float y, float z );

		float & operator[]( int index ) { return mComponents[index]; };
		float operator[]( int index ) const { return mComponents[index]; };

		Vector3f operator*( float scale ) const { return Vector3f( mComponents[0] * scale, mComponents[1] * scale, mComponents[2] * scale ); };
		Vector3f operator/( float scale ) const { return Vector3f( mComponents[0] / scale, mComponents[1] / scale, mComponents[2] / scale ); };
		Vector3f operator+( const Vector3f & rhs ) const { return Vector3f( mComponents[0] + rhs.mComponents[0], mComponents[1] + rhs.mComponents[1], mComponents[2] + rhs.mComponents[2] ); };
		Vector3f operator-( const Vector3f & rhs ) const { return Vector3f( mComponents[0] - rhs.mComponents[0], mComponents[1] - rhs.mComponents[1], mComponents[2] - rhs.mComponents[2] ); };
		Vector3f operator-() const { return Vector3f( -mComponents[0], -mComponents[1], -mComponents[2] ); };

		const Vector3f & operator*=( float scale );
		const Vector3f & operator/=( float scale );
		const Vector3f & operator+=( const Vector3f & rhs );
		const Vector3f & operator-=( const Vector3f & rhs );

		float magnitude() const { return sqrt( mComponents[0] * mComponents[0] + mComponents[1] * mComponents[1] + mComponents[2] * mComponents[2] ); };
		float magnitudeSquared() const { return mComponents[0] * mComponents[0] + mComponents[1] * mComponents[1] + mComponents[2] * mComponents[2]; };
		Vector3f normalize() const;
		float dot( const Vector3f & rhs ) const { return mComponents[0] * rhs.mComponents[0] + mComponents[1] * rhs.mComponents[1] + mComponents[2] * rhs.mComponents[2]; };
		Vector3f cross( const Vector3f & rhs ) const { return Vector3f( mComponents[1] * rhs.mComponents[2] - mComponents[2] * rhs.mComponents[1], mComponents[2] * rhs.mComponents[0] - mComponents[0] * rhs.mComponents[2], mComponents[0] * rhs.mComponents[1] - mComponents[1] * rhs.mComponents[0] ); };
		
	private:
		float mComponents[3];
};

std::ostream &operator<<( std::ostream & os, const Vector3f & vector );

#endif

