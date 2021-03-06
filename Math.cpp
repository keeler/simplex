#include "Math.hpp"
#include <GL/glut.h>

#define PI_OVER_180 0.0174532925f

/***********************************************************
 * Vector Class Methods
 **********************************************************/
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

/***********************************************************
 * Quaternion Class Methods
 **********************************************************/
Quaternion::Quaternion() :
	w( 1.0f ),
	x( 0.0f ),
	y( 0.0f ),
	z( 0.0f )
{
	// Already normalized
}

Quaternion::Quaternion( float W, float X, float Y, float Z ) :
	w( W ),
	x( X ),
	y( Y ),
	z( Z )
{
	this->normalize();
}

Quaternion::Quaternion( const Vector3f & axis, float angle )
{
	angle *= PI_OVER_180;
	angle *= 0.5f;

	float s = sin( angle );

	Vector3f normalAxis = axis.normalize();

	w = cos( angle );
	x = normalAxis[0] * s;
	y = normalAxis[1] * s;
	z = normalAxis[2] * s;

	this->normalize();
}

Quaternion::~Quaternion()
{
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
 
	Quaternion resultQuaternion = *this * ( vectorQuaternion * getConjugate() );
 
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

std::ostream &operator<<( std::ostream & os, const Quaternion & quaternion )
{
	std::cout << '(' << quaternion.w << ", " << quaternion.x << ", " << quaternion.y << ", " << quaternion.z << ')';

	return os;
}

/***********************************************************
 * Plane Class Methods
 **********************************************************/
Plane::Plane( const Vector3f & normal, const Vector3f & point ) :
	mNormal( normal )
{
	mPlaneConstant = normal.dot( point );
}

/***********************************************************
 * Frustum Class Methods
 **********************************************************/
Frustum::Frustum() :
	mHorizFieldOfView( 0.0f ),
	mAspectRatio( 0.0f ),
	mNearClip( 0.0f ),
	mFarClip( 0.0f ),
	mPosition( Vector3f() ),
	mOrientation( Quaternion() )
{
	Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation );
}

Frustum::Frustum( float fovy, float aspectRatio, float nearClip, float farClip, const Vector3f & position, const Quaternion & orientation ) :
	mHorizFieldOfView( fovy ),
	mAspectRatio( aspectRatio ),
	mNearClip( nearClip ),
	mFarClip( farClip ),
	mPosition( position ),
	mOrientation( orientation )
{
	Frustum::calculateCornersAndNormals( mCorners, mNormals, mHorizFieldOfView, mAspectRatio, mNearClip, mFarClip, mPosition, mOrientation );
}

// Works as a "radar" test, see Lighthouse3D.com
bool Frustum::isPointInFrustum( const Vector3f & aPoint ) const
{
	Vector3f point = aPoint - mPosition;    // Translate such that mPosition is origin
	
	Vector3f orthogonalAxes[3];
	calculateOrthogonalAxes( orthogonalAxes, mOrientation );

	float distAlongFrustum = point.dot( orthogonalAxes[0] );
	if( mNearClip <= distAlongFrustum && distAlongFrustum <= mFarClip )
	{
		float widthLimit = tan( 0.5 * mHorizFieldOfView * PI_OVER_180 ) * distAlongFrustum;
		float distAlongWidth = point.dot( orthogonalAxes[1] );
		if( -widthLimit <= distAlongWidth && distAlongWidth <= widthLimit )
		{
			float heightLimit = widthLimit * mAspectRatio;
			float distAlongHeight = point.dot( orthogonalAxes[2] );
			if( -heightLimit <= distAlongHeight && distAlongHeight <= heightLimit )
			{
				return true;
			}
		}
	}

	return false;
}

bool Frustum::isSphereInFrustum( const Vector3f & aPoint, float aRadius ) const
{
	float tanAngle = tan( 0.5 * mHorizFieldOfView * PI_OVER_180 );
	Vector3f point = aPoint - mPosition;

	Vector3f orthogonalAxes[3];
	calculateOrthogonalAxes( orthogonalAxes, mOrientation );

	float distAlongFrustum = point.dot( orthogonalAxes[0] );
	if( mNearClip - aRadius <= distAlongFrustum && distAlongFrustum <= mFarClip + aRadius )
	{
		// Just like point test, except it accounts for the sphere's radius as well
		float widthLimit = tanAngle * distAlongFrustum + aRadius / cos( 0.5 * mHorizFieldOfView * PI_OVER_180 );
		float distAlongWidth = point.dot( orthogonalAxes[1] );
		if( -widthLimit <= distAlongWidth && distAlongWidth <= widthLimit )
		{
			float heightLimit = widthLimit * mAspectRatio + aRadius / cos( atan( tanAngle * mAspectRatio ) );
			float distAlongHeight = point.dot( orthogonalAxes[2] );
			if( -heightLimit <= distAlongHeight && distAlongHeight <= heightLimit )
			{
				return true;
			}
		}
	}

	return false;
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
	Vector3f nearCenter = position + orthogonalAxes[0] * nearClip;    // Center of near clipping plane
	Vector3f farCenter = position + orthogonalAxes[0] * farClip;    // Center of far clipping plane

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

void Frustum::drawWireframe( Vector3f corners[], const Vector3f & color )
{
	// Draw wireframe mode
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glBegin( GL_QUADS );
        glColor3f( color[0], color[1], color[2] );

		// Near face
		glVertex3f( corners[NTR][0], corners[NTR][1], corners[NTR][2] );
		glVertex3f( corners[NTL][0], corners[NTL][1], corners[NTL][2] );
        glVertex3f( corners[NBL][0], corners[NBL][1], corners[NBL][2] );
        glVertex3f( corners[NBR][0], corners[NBR][1], corners[NBR][2] );

		// Far face
		glVertex3f( corners[FTR][0], corners[FTR][1], corners[FTR][2] );
		glVertex3f( corners[FTL][0], corners[FTL][1], corners[FTL][2] );
        glVertex3f( corners[FBL][0], corners[FBL][1], corners[FBL][2] );
        glVertex3f( corners[FBR][0], corners[FBR][1], corners[FBR][2] );

		// Right face
		glVertex3f( corners[NTR][0], corners[NTR][1], corners[NTR][2] );
		glVertex3f( corners[FTR][0], corners[FTR][1], corners[FTR][2] );
        glVertex3f( corners[FBR][0], corners[FBR][1], corners[FBR][2] );
        glVertex3f( corners[NBR][0], corners[NBR][1], corners[NBR][2] );

		// Left face
		glVertex3f( corners[FTL][0], corners[FTL][1], corners[FTL][2] );
		glVertex3f( corners[NTL][0], corners[NTL][1], corners[NTL][2] );
        glVertex3f( corners[NBL][0], corners[NBL][1], corners[NBL][2] );
        glVertex3f( corners[FBL][0], corners[FBL][1], corners[FBL][2] );

		// Top face
		glVertex3f( corners[FTR][0], corners[FTR][1], corners[FTR][2] );
		glVertex3f( corners[FTL][0], corners[FTL][1], corners[FTL][2] );
        glVertex3f( corners[NTL][0], corners[NTL][1], corners[NTL][2] );
        glVertex3f( corners[NTR][0], corners[NTR][1], corners[NTR][2] );

		// Bottom face
		glVertex3f( corners[FBR][0], corners[FBR][1], corners[FBR][2] );
		glVertex3f( corners[FBL][0], corners[FBL][1], corners[FBL][2] );
        glVertex3f( corners[NBL][0], corners[NBL][1], corners[NBL][2] );
        glVertex3f( corners[NBR][0], corners[NBR][1], corners[NBR][2] );
	glEnd();

	// Make it so we draw with filled polygons again
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}
