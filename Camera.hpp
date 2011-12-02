#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Vector3f.hpp"
#include "Quaternion.hpp"

class Camera
{
	public:
		Camera();
		Camera( const Vector3f & position, float yawDegrees, float pitchDegreeszz );
		~Camera();

		void setPosition( const Vector3f & position ) { mPosition = position; };

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
		
	private:
		Vector3f mPosition;
		Quaternion mRotation;
};

#endif
