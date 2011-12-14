#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include "Vector3f.hpp"
#include "Quaternion.hpp"

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

