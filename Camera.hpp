#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Math.hpp"

class Camera
{
	public:
		Camera();
		Camera( const Vector3f & position, float yawDegrees, float pitchDegrees );
		~Camera();

		void setPosition( const Vector3f & position ) { mPosition = position; };

		Frustum getFrustum() const { return mFrustum; };

		void moveForward( float amount );
		void moveBackward( float amount );
		void moveRight( float amount );
		void moveLeft( float amount );
		void moveUp( float amount );
		void moveDown( float amount );

		// Positive rotates right, negative rotates left
		void rotateYaw( float yawDegrees );
		// Positive rotates up, negative rotates down
		void rotatePitch( float pitchDegrees );

		// Actually does the world translation/rotation; move*() and rotate*() only
		// set the private member variables
		void look() const;
		void perspective( float fovy, float aspectRatio, float nearClip, float farClip );
		
	private:
		Vector3f   mPosition;
		Quaternion mOrientation;
		Frustum    mFrustum;
};

#endif
