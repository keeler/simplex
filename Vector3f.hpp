#ifndef _VECTOR3F_H_
#define _VECTOR3F_H_

#include <iostream>

class Vector3f
{
	public:
		Vector3f();
		Vector3f( float x, float y, float z );

		float & operator[]( int index );
		float operator[]( int index ) const;

		Vector3f operator*( float scale ) const;
		Vector3f operator/( float scale ) const;
		Vector3f operator+( const Vector3f & other ) const;
		Vector3f operator-( const Vector3f & other ) const;
		Vector3f operator-() const;

		const Vector3f & operator*=( float scale );
		const Vector3f & operator/=( float scale );
		const Vector3f & operator+=( const Vector3f & other );
		const Vector3f & operator-=( const Vector3f & other );

		float magnitude() const;
		float magnitudeSquared() const;
		Vector3f normalize() const;
		float dot( const Vector3f & other ) const;
		Vector3f cross( const Vector3f & other ) const;
		
	private:
		float mComponents[3];
};

std::ostream &operator<< (std::ostream & os, const Vector3f & vector);

#endif

