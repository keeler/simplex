#include "Vector3f.hpp"
#include <iostream>

Vector3f::Vector3f()
{
	mComponents[0] = 0.0f;
	mComponents[1] = 0.0f;
	mComponents[2] = 0.0f;
}

Vector3f::Vector3f( float x, float y, float z )
{
	mComponents[0] = x;
	mComponents[1] = y;
	mComponents[2] = z;
}

const Vector3f & Vector3f::operator*=( float scale )
{
	mComponents[0] *= scale;
	mComponents[1] *= scale;
	mComponents[2] *= scale;
	return *this;
}

const Vector3f & Vector3f::operator/=( float scale )
{
	mComponents[0] /= scale;
	mComponents[1] /= scale;
	mComponents[2] /= scale;
	return *this;
}

const Vector3f & Vector3f::operator+=( const Vector3f & rhs )
{
	mComponents[0] += rhs.mComponents[0];
	mComponents[1] += rhs.mComponents[1];
	mComponents[2] += rhs.mComponents[2];
	return *this;
}

const Vector3f & Vector3f::operator-=( const Vector3f & rhs )
{
	mComponents[0] -= rhs.mComponents[0];
	mComponents[1] -= rhs.mComponents[1];
	mComponents[2] -= rhs.mComponents[2];
	return *this;
}

Vector3f Vector3f::normalize() const
{
	float mag = this->magnitude();
	return Vector3f( mComponents[0] / mag, mComponents[1] / mag, mComponents[2] / mag );
}

std::ostream &operator<<( std::ostream & os, const Vector3f & vector )
{
    std::cout << '(' << vector[0] << ", " << vector[1] << ", " << vector[2] << ')';
	return os;
}

