#include "Vector3f.hpp"
#include <iostream>
#include <cmath>

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

float & Vector3f::operator[]( int index )
{
	return mComponents[index];
}

float Vector3f::operator[]( int index ) const
{
	return mComponents[index];
}

Vector3f Vector3f::operator*( float scale ) const
{
	return Vector3f( mComponents[0] * scale, mComponents[1] * scale, mComponents[2] * scale );
}

Vector3f Vector3f::operator/( float scale ) const
{
	return Vector3f( mComponents[0] / scale, mComponents[1] / scale, mComponents[2] / scale );
}

Vector3f Vector3f::operator+( const Vector3f & other ) const
{
    return Vector3f( mComponents[0] + other.mComponents[0],
                   mComponents[1] + other.mComponents[1],
                   mComponents[2] + other.mComponents[2] );
}

Vector3f Vector3f::operator-( const Vector3f & other ) const
{
    return Vector3f( mComponents[0] - other.mComponents[0],
                   mComponents[1] - other.mComponents[1],
                   mComponents[2] - other.mComponents[2] );
}

Vector3f Vector3f::operator-() const
{
	return Vector3f( -mComponents[0], -mComponents[1], -mComponents[2] );
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

const Vector3f & Vector3f::operator+=( const Vector3f & other )
{
	mComponents[0] += other.mComponents[0];
	mComponents[1] += other.mComponents[1];
	mComponents[2] += other.mComponents[2];
	return *this;
}

const Vector3f & Vector3f::operator-=( const Vector3f & other )
{
	mComponents[0] -= other.mComponents[0];
	mComponents[1] -= other.mComponents[1];
	mComponents[2] -= other.mComponents[2];
	return *this;
}

float Vector3f::magnitude() const
{
	return sqrt( mComponents[0] * mComponents[0] +
                 mComponents[1] * mComponents[1] +
                 mComponents[2] * mComponents[2] );
}

float Vector3f::magnitudeSquared() const
{
	return mComponents[0] * mComponents[0] +
           mComponents[1] * mComponents[1] +
           mComponents[2] * mComponents[2];
}

Vector3f Vector3f::normalize() const
{
	float mag = sqrt( mComponents[0] * mComponents[0] +
                      mComponents[1] * mComponents[1] +
                      mComponents[2] * mComponents[2] );
	return Vector3f( mComponents[0] / mag, mComponents[1] / mag, mComponents[2] / mag );
}

float Vector3f::dot( const Vector3f & other ) const
{
	return mComponents[0] * other.mComponents[0] +
           mComponents[1] * other.mComponents[1] +
           mComponents[2] * other.mComponents[2];
}

Vector3f Vector3f::cross( const Vector3f & other ) const
{
	return Vector3f( mComponents[1] * other.mComponents[2] - mComponents[2] * other.mComponents[1],
				   mComponents[2] * other.mComponents[0] - mComponents[0] * other.mComponents[2],
				   mComponents[0] * other.mComponents[1] - mComponents[1] * other.mComponents[0] );
}

std::ostream &operator<<( std::ostream & os, const Vector3f & vector )
{
    std::cout << '(' << vector[0] << ", " << vector[1] << ", " << vector[2] << ')';
	return os;
}

