#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "Vector3f.hpp"

class Quaternion
{
	public:
		Quaternion();
		Quaternion( float w, float x, float y, float z );
		Quaternion( const Vector3f & axis, float angle );
		~Quaternion();

		Quaternion operator*( const Quaternion & other ) const;
		// Multiplying a quaternion q with a vector v applies the q-rotation to v
		// To apply a quaternion-rotation to a vector, you need to multiply the
		// vector by the quaternion and its conjugate
		Vector3f operator*( const Vector3f & vector ) const;
		Quaternion & operator=( const Quaternion & rhs );
		// Use this to get info necessary to call glRotatef()
		void getAxisAndAngle( Vector3f & axis, float & angle ) const;

		void normalize();
		Quaternion getConjugate() const;

		friend std::ostream &operator<<( std::ostream & os, const Quaternion & quaternion );
	
	private:
		float w;
		float x;
		float y;
		float z;
};

#endif

