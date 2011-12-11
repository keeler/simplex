#include "Camera.hpp"
#include "GL/glut.h"

#define PI_OVER_180 0.0174532925f

Camera::Camera() :
	mPosition( Vector3f() ),
	mOrientation( Quaternion() )
{
}

Camera::Camera( const Vector3f & position, float yawDegrees, float pitchDegrees ) :
	mPosition( position ),
	mOrientation( Quaternion() )
{
	this->rotateYaw( yawDegrees );
	this->rotatePitch( pitchDegrees );
}

Camera::~Camera()
{
}

// For each of the move functions, apply the camera's current
// rotation state to the default direction vector corresponding
// to the desired direction of movement. E.g. the default "forward"
// for OpenGL faces negative z
void Camera::moveForward( float amount )
{
	// Rotate the default forward vector
	Vector3f currentForward = mOrientation * Vector3f( 0.0f, 0.0f, -1.0f );
	mPosition += currentForward * amount;
	mFrustum.setPosition( mPosition );
}

void Camera::moveBackward( float amount )
{
	Vector3f currentBackward = mOrientation * Vector3f( 0.0f, 0.0f, 1.0f );
	mPosition += currentBackward * amount;
	mFrustum.setPosition( mPosition );
}

void Camera::moveRight( float amount )
{
	Vector3f currentRight = mOrientation * Vector3f( 1.0f, 0.0f, 0.0f );
	mPosition += currentRight * amount;
	mFrustum.setPosition( mPosition );
}

void Camera::moveLeft( float amount )
{
	Vector3f currentLeft = mOrientation * Vector3f( -1.0f, 0.0f, 0.0f );
	mPosition += currentLeft * amount;
	mFrustum.setPosition( mPosition );
}

void Camera::moveUp( float amount )
{
	Vector3f currentUp = mOrientation * Vector3f( 0.0f, 1.0f, 0.0f );
	mPosition += currentUp * amount;
	mFrustum.setPosition( mPosition );
}

void Camera::moveDown( float amount )
{
	Vector3f currentDown = mOrientation * Vector3f( 0.0f, -1.0f, 0.0f );
	mPosition += currentDown * amount;
	mFrustum.setPosition( mPosition );
}

void Camera::rotateYaw( float yawDegrees )
{
	// Multiply by -1 so that positive value of yawDegrees rotates right,
	// negative rotates left
	yawDegrees *= -1.0f;
	Quaternion yawRotation( Vector3f( 0.0f, 1.0f, 0.0f ), yawDegrees );
	mOrientation = mOrientation * yawRotation;
	mFrustum.setOrientation( mOrientation );
}

void Camera::rotatePitch( float pitchDegrees )
{
	// Multiply by -1 so that positive value of pitchDegrees rotates up,
	// negative rotates down
	pitchDegrees *= -1.0f;
	Quaternion pitchRotation( Vector3f( 1.0f, 0.0f, 0.0f ), pitchDegrees );
	mOrientation = mOrientation * pitchRotation;
	mFrustum.setOrientation( mOrientation );
}

// Actually does the world translation/rotation; the above only set them
void Camera::look() const
{
	Vector3f axis;
	float angle;
	mOrientation.getAxisAndAngle( axis, angle );

	glRotatef( -angle, axis[0], axis[1], axis[2] );
	glTranslatef( -mPosition[0], -mPosition[1], -mPosition[2] );
}

void Camera::perspective( float fovy, float aspectRatio, float nearClip, float farClip )
{
	gluPerspective( fovy, aspectRatio, nearClip, farClip );
	mFrustum = Frustum( fovy, aspectRatio, nearClip, farClip, -mPosition, mOrientation.getConjugate() );
}

