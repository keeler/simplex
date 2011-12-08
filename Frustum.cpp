#include "Frustum.hpp"
#include <iostream>
#include <cstdlib>
#include <cmath>

#define PI_OVER_180 0.0174532925f

Frustum::Frustum( float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation ) :
	mFieldOfViewY( fovy ),
	mAspectRatio( aspectRatio ),
	mNearClip( nearClip ),
	mFarClip( farClip ),
	mPosition( position ),
	mOrientation( orientation )
{
	Frustum::calculateCornersAndNormals( mCorners, mNormals, mFieldOfViewY, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation );
}

void Frustum::calculateCornersAndNormals( Vector3f corners[], Vector3f frustumNormals[], float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation )
{
	float nearHalfWidth = tan( 0.5 * fovy * PI_OVER_180 ) * nearClip;
	float nearHalfHeight = nearHalfWidth / aspectRatio;
	float farHalfWidth = tan( 0.5 * fovy * PI_OVER_180 ) * farClip;
	float farHalfHeight = farHalfWidth / aspectRatio;

	Vector3f orthogonalAxes[3];
	calculateOrthogonalAxes( orthogonalAxes, orientation );

	// Calculate corners
	Vector3f nearCenter = position + orthogonalAxes[0] * nearClip;
	Vector3f farCenter = position + orthogonalAxes[0] * farClip;

	corners[NTR] = nearCenter + orthogonalAxes[2] * nearHalfHeight + orthogonalAxes[1] * nearHalfWidth;
	corners[NTL] = nearCenter + orthogonalAxes[2] * nearHalfHeight - orthogonalAxes[1] * nearHalfWidth;
	corners[NBR] = nearCenter - orthogonalAxes[2] * nearHalfHeight + orthogonalAxes[1] * nearHalfWidth;
	corners[NBL] = nearCenter - orthogonalAxes[2] * nearHalfHeight - orthogonalAxes[1] * nearHalfWidth;
	corners[FTR] = farCenter + orthogonalAxes[2] * farHalfHeight + orthogonalAxes[1] * farHalfWidth;
	corners[FTL] = farCenter + orthogonalAxes[2] * farHalfHeight - orthogonalAxes[1] * farHalfWidth;
	corners[FBR] = farCenter - orthogonalAxes[2] * farHalfHeight + orthogonalAxes[1] * farHalfWidth;
	corners[FBL] = farCenter - orthogonalAxes[2] * farHalfHeight - orthogonalAxes[1] * farHalfWidth;

	// Calculate normals
	Vector3f nearBottomRight = corners[NBR];
	Vector3f farTopLeft = corners[FTL];

	frustumNormals[NEAR]    = ( corners[NTR] - nearBottomRight ).cross( corners[NBL] - nearBottomRight ).normalize();
	frustumNormals[BOTTOM]  = ( corners[NBL] - nearBottomRight ).cross( corners[FBR] - nearBottomRight ).normalize();
	frustumNormals[RIGHT]   = ( corners[FBR] - nearBottomRight ).cross( corners[NTR] - nearBottomRight ).normalize();
	frustumNormals[FAR]     = ( corners[FTR] - farTopLeft ).cross( corners[FBL] - farTopLeft ).normalize();
	frustumNormals[TOP]     = ( corners[NTL] - farTopLeft ).cross( corners[FTR] - farTopLeft ).normalize();
	frustumNormals[LEFT]    = ( corners[FBL] - farTopLeft ).cross( corners[NTL] - farTopLeft ).normalize();
}

void Frustum::calculateOrthogonalAxes( Vector3f orthogonalAxes[], const Quaternion & orientation )
{
	orthogonalAxes[0] = orientation * Vector3f( 0.0f, 0.0f, -1.0f );
	orthogonalAxes[1] = orientation * Vector3f( 1.0f, 0.0f, 0.0f );
	orthogonalAxes[2] = orientation * Vector3f( 0.0f, 1.0f, 0.0f );
}

