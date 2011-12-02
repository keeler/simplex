#include "Quaternion.hpp"
#include <cmath>

using std::cout;
using std::endl;

#define PI_OVER_180 0.0174532925f

Quaternion::Quaternion()
{
	w = 1.0f;
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;

	normalize();
}

Quaternion::Quaternion( float w, float x, float y, float z )
{
	this->w = w;
	this->x = x;
	this->y = y;
	this->z = z;

	normalize();
}

Quaternion::Quaternion( const Vector3f & axis, float angle )
{
	angle *= PI_OVER_180;
	angle /= 2.0f;

	float s = sin( angle );

	Vector3f normalAxis = axis.normalize();

	w = cos( angle );
	x = normalAxis[0] * s;
	y = normalAxis[1] * s;
	z = normalAxis[2] * s;

	normalize();
}

Quaternion::~Quaternion()
{
}

Quaternion Quaternion::operator*( const Quaternion & other ) const
{
	return Quaternion( w * other.w - x * other.x - y * other.y - z * other.z,
					   w * other.x + x * other.w + y * other.z - z * other.y,
	                   w * other.y - x * other.z + y * other.w + z * other.x,
	                   w * other.z + x * other.y - y * other.x + z * other.w );
}

// Multiplying a quaternion q with a vector v applies the q-rotation to v
// To apply a quaternion-rotation to a vector, you need to multiply the
// vector by the quaternion and its conjugate
Vector3f Quaternion::operator*( const Vector3f & vector ) const
{
	Vector3f normalizedVector = vector.normalize();
 
	Quaternion vectorQuaternion;
	vectorQuaternion.x = normalizedVector[0];
	vectorQuaternion.y = normalizedVector[1];
	vectorQuaternion.z = normalizedVector[2];
	vectorQuaternion.w = 0.0f;
 
	Quaternion resultQuaternion;
	resultQuaternion = vectorQuaternion * getConjugate();
	resultQuaternion = *this * resultQuaternion;
 
	return Vector3f( resultQuaternion.x, resultQuaternion.y, resultQuaternion.z );
}

Quaternion & Quaternion::operator=( const Quaternion & rhs )
{
	if( this == &rhs )
	{
		return *this;
	}
	else
	{
		w = rhs.w;
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;

		return *this;
	}
}

// Use this to get info necessary to call glRotatef()
void Quaternion::getAxisAndAngle( Vector3f & axis, float & angle ) const
{
	angle = ( acos( w ) * 2.0f ) / PI_OVER_180;    // Convert to degrees

	float scale = sqrt( x * x + y * y + z * z );
	axis[0] = x / scale;
	axis[1] = y / scale;
	axis[2] = z / scale;
}

void Quaternion::normalize()
{
	float magnitude = sqrt( w * w + x * x + y * y + z * z );
	w /= magnitude;
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
}

Quaternion Quaternion::getConjugate() const
{
	return Quaternion( w, -x, -y, -z );
}

std::ostream &operator<<( std::ostream & os, const Quaternion & quaternion )
{
	cout << '(' << quaternion.w << ", " << quaternion.x << ", "
		 << quaternion.y << ", " << quaternion.z << ')';

	return os;
}

