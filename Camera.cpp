#include "Camera.hpp"
#include "GL/glut.h"

#define PI_OVER_180 0.0174532925f

Camera::Camera()
{
	mPosition = Vector3f();
	mRotation = Quaternion();
}

Camera::Camera( Vector3f position, float yawDegrees, float pitchDegrees )
{
	mPosition = position;

	mRotation = Quaternion();
	rotateYaw( yawDegrees );
	rotatePitch( pitchDegrees );
}

Camera::~Camera()
{
}

void Camera::setPosition( const Vector3f & position )
{
	mPosition = position;
}

// For each of the move functions, apply the camera's current
// rotation state to the default direction vector corresponding
// to the desired direction of movement. E.g. the default "forward"
// for OpenGL faces negative z
void Camera::moveForward( float amount )
{
	Vector3f defaultForward( 0.0f, 0.0f, -1.0f );
	Vector3f currentForward = mRotation * defaultForward;

	mPosition += currentForward * amount;
}

void Camera::moveBackward( float amount )
{
	Vector3f defaultBackward( 0.0f, 0.0f, 1.0f );
	Vector3f currentBackward = mRotation * defaultBackward;

	mPosition += currentBackward * amount;
}

void Camera::moveRight( float amount )
{
	Vector3f defaultRight( 1.0f, 0.0f, 0.0f );
	Vector3f currentRight = mRotation * defaultRight;

	mPosition += currentRight * amount;
}

void Camera::moveLeft( float amount )
{
	Vector3f defaultLeft( -1.0f, 0.0f, 0.0f );
	Vector3f currentLeft = mRotation * defaultLeft;

	mPosition += currentLeft * amount;
}

void Camera::moveUp( float amount )
{
	Vector3f defaultUp( 0.0f, 1.0f, 0.0f );
	Vector3f currentUp = mRotation * defaultUp;

	mPosition += currentUp * amount;
}

void Camera::moveDown( float amount )
{
	Vector3f defaultDown( 0.0f, -1.0f, 0.0f );
	Vector3f currentDown = mRotation * defaultDown;

	mPosition += currentDown * amount;
}

void Camera::rotateYaw( float yawDegrees )
{
	// Multiply by -1 so that positive value of yawDegrees rotates right,
	// negative rotates left
	yawDegrees *= -1.0f;// * PI_OVER_180;    // Convert to radians
	Quaternion yawRotation( Vector3f( 0.0f, 1.0f, 0.0f ), yawDegrees );
	mRotation = mRotation * yawRotation;
}

void Camera::rotatePitch( float pitchDegrees )
{
	// Multiply by -1 so that positive value of pitchDegrees rotates up,
	// negative rotates down
	pitchDegrees *= -1.0f;// * PI_OVER_180;    // Convert to radians
	Quaternion pitchRotation( Vector3f( 1.0f, 0.0f, 0.0f ), pitchDegrees );
	mRotation = mRotation * pitchRotation;
}

// Actually does the world translation/rotation; the above only set them
void Camera::look() const
{
	Vector3f axis;
	float angle;
	mRotation.getAxisAndAngle( axis, angle );

	glRotatef( -angle, axis[0], axis[1], axis[2] );
	glTranslatef( -mPosition[0], -mPosition[1], -mPosition[2] );
}

