#ifndef _MATH_HPP_
#define _MATH_HPP_

#include <cmath>
#include <iostream>

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

		friend std::ostream &operator<<( std::ostream & os, const Vector3f & vector );

	private:
		float mComponents[3];
};

class Quaternion
{
	public:
		Quaternion();
		Quaternion( float W, float X, float Y, float Z );
		Quaternion( const Vector3f & axis, float angle );
		~Quaternion();

		Quaternion operator*( const Quaternion & rhs ) const { return Quaternion( w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z, w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y, w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x, w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w ); };
		// Multiplying a quaternion q with a vector v applies the q-rotation to v
		// To apply a quaternion-rotation to a vector, you need to multiply the
		// vector by the quaternion and its conjugate
		Vector3f operator*( const Vector3f & vector ) const;
		Quaternion & operator=( const Quaternion & rhs );
		// Use this to get info necessary to call glRotatef()
		void getAxisAndAngle( Vector3f & axis, float & angle ) const;

		void normalize();
		Quaternion getConjugate() const { return Quaternion( w, -x, -y, -z ); };

		friend std::ostream &operator<<( std::ostream & os, const Quaternion & quaternion );
	
	private:
		float w;
		float x;
		float y;
		float z;
};

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

class Frustum
{
	public:
		Frustum();
		Frustum( float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation );

		bool isPointInFrustum( const Vector3f & aPoint ) const;
		bool isSphereInFrustum( const Vector3f & aPoint, float aRadius ) const;

		float getFov() const { return mHorizFieldOfView; };
		float getAspectRatio() const { return mAspectRatio; };
		float getNearClip() const { return mNearClip; };
		float getFarClip() const { return mFarClip; };
		Vector3f getPosition() const { return mPosition; };
		Quaternion getOrientation() const { return mOrientation; };
		const Vector3f * getCorners() const { return mCorners; };
		const Vector3f * getNormals() const { return mNormals; };

		void setFov( float horizontalFov ) { mHorizFieldOfView = horizontalFov; Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation ); };
		void setAspectRatio( float aspectRatio ) { mAspectRatio = aspectRatio; Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation ); };
		void setNearClip( float nearClip ) { mNearClip = nearClip; Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation ); };
		void setFarClip( float farClip ) { mFarClip = farClip; Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation ); };
		void setPosition( const Vector3f & position ) { mPosition = position; Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation ); };
		void setOrientation( const Quaternion & orientation ) { mOrientation = orientation; Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation ); };

		void draw( const Vector3f & color ) { drawWireframe( mCorners, color ); };

		enum Corner
		{
			NTR,	// Near Top Right
			NTL,	// Near Top Left
			NBR,	// Near Bottom Right
			NBL,	// Near Bottom Left
			FTR,	// Far Top Right
			FTL,	// Far Top Left
			FBR,	// Far Bottom Right
			FBL		// Far Bottom Left
		};
		enum Normal
		{
			NEAR,
			TOP,
			RIGHT,
			FAR,
			BOTTOM,
			LEFT,
		};

	private:
		static void calculateCornersAndNormals( Vector3f corners[], Vector3f frustumNormals[], float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation );
		// 0 --> Forward
		// 1 --> Right
		// 2 --> Up
		static void calculateOrthogonalAxes( Vector3f orthogonalAxes[], const Quaternion & orientation );
		static void drawWireframe( Vector3f corners[], const Vector3f & color );

		float mHorizFieldOfView;    // Horizontal FOV for frustum
		float mAspectRatio;
		float mNearClip;
		float mFarClip;

		Vector3f   mPosition;
		Quaternion mOrientation;

		Vector3f mCorners[8];
		Vector3f mNormals[6];
};

#endif
