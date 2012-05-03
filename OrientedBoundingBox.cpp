#include "OrientedBoundingBox.hpp"
#include <cmath>
#include <cstdlib>
#include <GL/glut.h>

#define PI_OVER_180 0.0174532925f

OrientedBoundingBox::OrientedBoundingBox() :
    mCenter( Vector3f( 0.0f, 0.0f, 0.0f ) ),
    mEdgeHalfLengths( Vector3f( 1.0f, 1.0f, 1.0f ) ),
    mOrientation( Quaternion() ),
    mHasCollided( false )
{
    this->calculateRadius();
}

OrientedBoundingBox::OrientedBoundingBox( const Vector3f & center, const Vector3f & edgeHalfLengths, const Quaternion & orientation ) :
    mCenter( center ),
    mEdgeHalfLengths( edgeHalfLengths ),
    mOrientation( orientation ),
    mHasCollided( false )
{
    this->calculateRadius();
}

void OrientedBoundingBox::rotate( const Vector3f & axis, float degrees )
{
	degrees *= -1.0f;
	Quaternion deltaRotation( axis, degrees );
	mOrientation = mOrientation * deltaRotation;
}

bool OrientedBoundingBox::isPointInside( const Vector3f & point ) const
{
    // Transform the point so that the center of this box is the origin
    Vector3f transformedPoint = point - mCenter;

	Vector3f orthogonalAxes[3];
	OrientedBoundingBox::calculateOrthogonalAxes( orthogonalAxes, mOrientation );
	
    // Check if the scalar projection of the point along each axis is
    // within the bounds of the box
    if( fabs( orthogonalAxes[0].dot( transformedPoint ) ) <= mEdgeHalfLengths[0] &&
        fabs( orthogonalAxes[1].dot( transformedPoint ) ) <= mEdgeHalfLengths[1] &&
        fabs( orthogonalAxes[2].dot( transformedPoint ) ) <= mEdgeHalfLengths[2] )
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Use the separating plane theorem to test whether two oriented bounding boxes
// intersect. If they do, then we have a collision
bool OrientedBoundingBox::collisionWith( const OrientedBoundingBox & otherBox ) const
{
    // First do a sphere check, will save A LOT of time if the boxes are close but not touching
    // Try taking this out and seeing how dramatically it slows down
    // We're also saving a sqrt() by calling magnitudeSquared()
    float minCollisionDistance = otherBox.mRadius + mRadius;
    if( ( otherBox.mCenter - mCenter ).magnitudeSquared() > ( minCollisionDistance * minCollisionDistance ) )
    {
        return false;
    }

    Vector3f thisCorners[8];
    Vector3f otherCorners[8];
    Vector3f thisOrthogonalAxes[3];
    Vector3f otherOrthogonalAxes[3];

    OrientedBoundingBox::calculateCornerPoints( thisCorners, mCenter, mEdgeHalfLengths, mOrientation );
    OrientedBoundingBox::calculateCornerPoints( otherCorners, otherBox.mCenter, otherBox.mEdgeHalfLengths, otherBox.mOrientation );

    OrientedBoundingBox::calculateOrthogonalAxes( thisOrthogonalAxes, mOrientation );
    OrientedBoundingBox::calculateOrthogonalAxes( otherOrthogonalAxes, otherBox.mOrientation );

    // These corner points should be precalculated already
    // Each face is determined by it's normal & a point in it, since it's basically a plane
    Plane thisPlanes[6] = { Plane( -thisOrthogonalAxes[0], thisCorners[0] ),
                            Plane( -thisOrthogonalAxes[1], thisCorners[0] ),
                            Plane( -thisOrthogonalAxes[2], thisCorners[0] ),
                            Plane(  thisOrthogonalAxes[0], thisCorners[7] ),
                            Plane(  thisOrthogonalAxes[1], thisCorners[7] ),
                            Plane(  thisOrthogonalAxes[2], thisCorners[7] ) };
    Plane otherPlanes[6] = { Plane( -otherOrthogonalAxes[0], otherCorners[0] ),
                             Plane( -otherOrthogonalAxes[1], otherCorners[0] ),
                             Plane( -otherOrthogonalAxes[2], otherCorners[0] ),
                             Plane(  otherOrthogonalAxes[0], otherCorners[7] ),
                             Plane(  otherOrthogonalAxes[1], otherCorners[7] ),
                             Plane(  otherOrthogonalAxes[2], otherCorners[7] ) };

    // The method of separating axes for obb collision detection works like this:
    // 1) The corners of box A are checked against each side of box B.
    // 2) The check consists on detevoid calculateOrthogonalAxes( Vector3f axes[] ) constrmining on which side (positive/negative) a corner
    // of box A is in regard to a side of box B.
    // 3) To determine this, corner coordinates of A are projected on the outward-pointing
    // normals of the sides on B.
    // 4) If, for a side, all normalized distances are positive, the boxes cannot be
    // in collision, i.e. a separating axis has been found.
    // 5) As A is checked against B, B is also checked against A.

    // Check corners of box 1 against faces of box 2.
    for( int i = 0; i < 6; i++ )    // 6 is number of faces to check against
    {
        bool allPositive = true;
        for( int j = 0; j < 8; j++ )    // 8 is number of corners to check
        {
            // A negative projection has been found, no need to keep checking.
            // It does not mean that there is a collision though.
            if( otherPlanes[i].isInNegativeHalfSpace( thisCorners[j] ) )
            {
                allPositive = false;
                break;
            }
        }
        // A separating axis has been found, a collision cannot occur.
        if( allPositive )
        {
            return false;
        }
    }
    
    // Check corners of box 2 against faces of box 1.
    for( int i = 0; i < 6; i++ )
    {
        bool allPositive = true;
        for( int j = 0; j < 8; j++ )
        {
            // A negative projection has been found, no need to keep checking.
            // It does not mean that there is a collision though.
            if( thisPlanes[i].isInNegativeHalfSpace( otherCorners[j] ) )
            {
                allPositive = false;
                break;
            }
        }
        // A separating axis has been found, a collision cannot occur.
        if( allPositive )
        {
            return false;
        }
    }
    
    // No separating axis has been found, a collision MUST exist.
    return true;
}

void OrientedBoundingBox::draw( const Vector3f & color ) const
{
    Vector3f corners[8];
    OrientedBoundingBox::calculateCornerPoints( corners, mCenter, mEdgeHalfLengths, mOrientation );
    
    Vector3f orthogonalAxes[3];
    OrientedBoundingBox::calculateOrthogonalAxes( orthogonalAxes, mOrientation );

    // These corner points are already in global reference, so we don't need to transform them
	glBegin( GL_QUADS );
        glColor3f( color[0], color[1], color[2] );
		// Front
        glNormal3f( orthogonalAxes[0][0], orthogonalAxes[0][1], orthogonalAxes[0][2] );
		glVertex3f( corners[1][0], corners[1][1], corners[1][2] );
		glVertex3f( corners[6][0], corners[6][1], corners[6][2] );
        glVertex3f( corners[7][0], corners[7][1], corners[7][2] );
        glVertex3f( corners[2][0], corners[2][1], corners[2][2] );

		// Right
        glNormal3f( orthogonalAxes[1][0], orthogonalAxes[1][1], orthogonalAxes[1][2] );
		glVertex3f( corners[2][0], corners[2][1], corners[2][2] );
		glVertex3f( corners[7][0], corners[7][1], corners[7][2] );
        glVertex3f( corners[4][0], corners[4][1], corners[4][2] );
        glVertex3f( corners[3][0], corners[3][1], corners[3][2] );

		// Back
        glNormal3f( -orthogonalAxes[0][0], -orthogonalAxes[0][1], -orthogonalAxes[0][2] );
		glVertex3f( corners[3][0], corners[3][1], corners[3][2] );
		glVertex3f( corners[4][0], corners[4][1], corners[4][2] );
        glVertex3f( corners[5][0], corners[5][1], corners[5][2] );
        glVertex3f( corners[0][0], corners[0][1], corners[0][2] );

		// Left
        glNormal3f( -orthogonalAxes[1][0], orthogonalAxes[1][1], orthogonalAxes[1][2] );
		glVertex3f( corners[0][0], corners[0][1], corners[0][2] );
		glVertex3f( corners[5][0], corners[5][1], corners[5][2] );
        glVertex3f( corners[6][0], corners[6][1], corners[6][2] );
        glVertex3f( corners[1][0], corners[1][1], corners[1][2] );

		// Top
        glNormal3f( orthogonalAxes[2][0], orthogonalAxes[2][1], orthogonalAxes[2][2] );
		glVertex3f( corners[6][0], corners[6][1], corners[6][2] );
		glVertex3f( corners[5][0], corners[5][1], corners[5][2] );
        glVertex3f( corners[4][0], corners[4][1], corners[4][2] );
        glVertex3f( corners[7][0], corners[7][1], corners[7][2] );

		// Bottom
        glNormal3f( -orthogonalAxes[2][0], -orthogonalAxes[2][1], -orthogonalAxes[2][2] );
		glVertex3f( corners[1][0], corners[1][1], corners[1][2] );
		glVertex3f( corners[2][0], corners[2][1], corners[2][2] );
        glVertex3f( corners[3][0], corners[3][1], corners[3][2] );
        glVertex3f( corners[0][0], corners[0][1], corners[0][2] );
	glEnd();
}

// Calculate the corner points of the box; used for collision detection and drawing,
// return by reference in corners array
void OrientedBoundingBox::calculateCornerPoints( Vector3f corners[], const Vector3f & center, const Vector3f & edgeHalfLengths, const Quaternion & orientation )
{
	Vector3f orthogonalAxes[3];
	OrientedBoundingBox::calculateOrthogonalAxes( orthogonalAxes, orientation );

    corners[0] = center - orthogonalAxes[0] * edgeHalfLengths[0]
                        - orthogonalAxes[1] * edgeHalfLengths[1]
                        - orthogonalAxes[2] * edgeHalfLengths[2];
    corners[1] = center + orthogonalAxes[0] * edgeHalfLengths[0]
                        - orthogonalAxes[1] * edgeHalfLengths[1]
                        - orthogonalAxes[2] * edgeHalfLengths[2];
    corners[2] = center + orthogonalAxes[0] * edgeHalfLengths[0]
                        + orthogonalAxes[1] * edgeHalfLengths[1]
                        - orthogonalAxes[2] * edgeHalfLengths[2];
    corners[3] = center - orthogonalAxes[0] * edgeHalfLengths[0]
                        + orthogonalAxes[1] * edgeHalfLengths[1]
                        - orthogonalAxes[2] * edgeHalfLengths[2];
    corners[4] = center - orthogonalAxes[0] * edgeHalfLengths[0]
                        + orthogonalAxes[1] * edgeHalfLengths[1]
                        + orthogonalAxes[2] * edgeHalfLengths[2];
    corners[5] = center - orthogonalAxes[0] * edgeHalfLengths[0]
                        - orthogonalAxes[1] * edgeHalfLengths[1]
                        + orthogonalAxes[2] * edgeHalfLengths[2];
    corners[6] = center + orthogonalAxes[0] * edgeHalfLengths[0]
                        - orthogonalAxes[1] * edgeHalfLengths[1]
                        + orthogonalAxes[2] * edgeHalfLengths[2];
    corners[7] = center + orthogonalAxes[0] * edgeHalfLengths[0]
                        + orthogonalAxes[1] * edgeHalfLengths[1]
                        + orthogonalAxes[2] * edgeHalfLengths[2];
}

void OrientedBoundingBox::calculateOrthogonalAxes( Vector3f axes[], const Quaternion & orientation )
{
	axes[0] = orientation * Vector3f( 1.0f, 0.0f, 0.0f );
	axes[1] = orientation * Vector3f( 0.0f, 1.0f, 0.0f );
	axes[2] = orientation * Vector3f( 0.0f, 0.0f, 1.0f );
}

