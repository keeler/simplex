#ifndef ORIENTED_BOUNDING_BOX_HPP
#define ORIENTED_BOUNDING_BOX_HPP

#include "Vector3f.hpp"
#include "Quaternion.hpp"
#include <iostream>

// Notes on orientation of corners and axes.
// Axes are such that x is out, y is right, and z is up.
// These correspond to mOrthogonalAxes indices 0, 1, and 2.
// The corners are labelled as they are indexed in the array,
// based on this axis orientation. Using an awesome ACSII cube.
// What up, bitches.
//
//           (5) _________________________ (4)
//              / _____________________  /|
//             / / ___________________/ / |
//            / / /| |               / /  |
//           / / / | |              / / . |
//          / / /| | |             / / /| |
//         / / / | | |            / / / | |
//        / / /  | | |       (7) / / /| | |
//       / /_/__________________/ / / | | |
//  (6) /________________________/ /  | | |
//      | ______________________ | |  | | |
//      | | |    | | |_________| | |__| | |
//      | | |    | |___________| | |____| | (3)
//      | | |   / / ___________| | |_  / /
//      | | |  / / / (0)       | | |/ / /
//      | | | / / /            | | | / /
//      | | |/ / /             | | |/ /
//      | | | / /              | | ' /
//      | | |/_/_______________| |  /
//      | |____________________| | /
//  (1) |________________________|/ (2)

class OrientedBoundingBox
{
    public:
        OrientedBoundingBox();
        // Center, 3 normalized orthogonal axes indicating orientation, and 3 edge half lengths
        // The order of the edge half lengths should match the respective dimensions to which
        // they correspond.
        OrientedBoundingBox( const Vector3f & center, const Vector3f & edgeHalfLengths, const Quaternion & orientation );

        Vector3f getCenter() const { return mCenter; };
        void setCenter( const Vector3f & newCenter ) { mCenter = newCenter; };

		// Max distance to a corner to center
        float getRadius() const { return mRadius; };

		bool getCollisionState() const { return mHasCollided; };
		void setCollisionState( bool newState ) { mHasCollided = newState; };

        void rotate( const Vector3f & axis, float degrees );

        bool isPointInside( const Vector3f & point ) const;
        bool collisionWith( const OrientedBoundingBox & otherBox ) const;

        void move( const Vector3f & velocity ) { mCenter += velocity; };
        void draw( const Vector3f & color ) const;
        
        // Needed for collisionWith( OBB ) and drawing
        static void calculateCornerPoints( Vector3f corners[], const Vector3f & center, const Vector3f & edgeHalfLengths, const Quaternion & orientation );
        // Used for collision detection
        static void calculateOrthogonalAxes( Vector3f axes[], const Quaternion & orientation );

    private:
        // radius = sqrt( a^2 + b^2 + c^2 ), need to multiply the halfEdgeLengths by 2 to get a, b, and c
        void calculateRadius() { mRadius = ( mEdgeHalfLengths * 2 ).magnitude() / 2; };

        Vector3f   mCenter;
        Vector3f   mEdgeHalfLengths;
        Quaternion mOrientation;
        
        bool mHasCollided;

        float mRadius;    // Maximum distance to a corner
};

#endif

