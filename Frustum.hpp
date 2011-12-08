#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include "Vector3f.hpp"
#include "Quaternion.hpp"

class Frustum
{
	public:
		Frustum( float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation );

	private:
		static void calculateCornersAndNormals( Vector3f corners[], Vector3f frustumNormals[], float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation );
		// 0 --> Forward
		// 1 --> Right
		// 2 --> Up
		static void calculateOrthogonalAxes( Vector3f orthogonalAxes[], const Quaternion & orientation );
		
		float mFieldOfViewY;	// In OpenGL, y is to the right, so this is basically the horizontal FOV for frustum
		float mAspectRatio;
		float mNearClip;
		float mFarClip;

		Vector3f   mPosition;
		Quaternion mOrientation;

		Vector3f mCorners[8];
		Vector3f mNormals[6];

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
			FAR,
			LEFT,
			RIGHT,
			TOP,
			BOTTOM
		};
};

#endif

