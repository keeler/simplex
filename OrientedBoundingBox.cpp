#include "OrientedBoundingBox.hpp"
#include <cmath>
#include <cstdlib>
#include <GL/glut.h>

#define PI_OVER_180 0.0174532925f

OrientedBoundingBox::OrientedBoundingBox() :
    mCenter( Vector3f( 0.0f, 0.0f, 0.0f ) ),
    mEdgeHalfLengths( Vector3f( 1.0f, 1.0f, 1.0f ) )
{
    mOrthogonalAxes[0] = Vector3f( 1.0f, 0.0f, 0.0f );
    mOrthogonalAxes[1] = Vector3f( 0.0f, 1.0f, 0.0f );
    mOrthogonalAxes[2] = Vector3f( 0.0f, 0.0f, 1.0f );

    this->calculateRadius();
}

OrientedBoundingBox::OrientedBoundingBox( const Vector3f & center, const Vector3f & edgeHalfLengths, Vector3f orthogonalAxes[] ) :
    mCenter( center ),
    mEdgeHalfLengths( edgeHalfLengths )
{
    mOrthogonalAxes[0] = orthogonalAxes[0].normalize();
    mOrthogonalAxes[1] = orthogonalAxes[1].normalize();
    mOrthogonalAxes[2] = orthogonalAxes[2].normalize();

    this->calculateRadius();
}

void OrientedBoundingBox::rotate( Vector3f axis, float degrees )
{
    axis = axis.normalize();
    float radians = degrees * PI_OVER_180;
    float s = sin( radians );
    float c = cos( radians );

    mOrthogonalAxes[0] = mOrthogonalAxes[0] * c + axis * axis.dot( mOrthogonalAxes[0] ) * ( 1 - c ) + mOrthogonalAxes[0].cross( axis ) * s;
    mOrthogonalAxes[1] = mOrthogonalAxes[1] * c + axis * axis.dot( mOrthogonalAxes[1] ) * ( 1 - c ) + mOrthogonalAxes[1].cross( axis ) * s;
    mOrthogonalAxes[2] = mOrthogonalAxes[2] * c + axis * axis.dot( mOrthogonalAxes[2] ) * ( 1 - c ) + mOrthogonalAxes[2].cross( axis ) * s;
}

bool OrientedBoundingBox::isPointInside( const Vector3f & point ) const
{
    // Transform the point so that the center of this box is the origin
    Vector3f transformedPoint = point - mCenter;

    // Check if the scalar projection of the point along each axis is
    // within the bounds of the box
    if( fabs( mOrthogonalAxes[0].dot( transformedPoint ) ) <= mEdgeHalfLengths[0] &&
        fabs( mOrthogonalAxes[1].dot( transformedPoint ) ) <= mEdgeHalfLengths[1] &&
        fabs( mOrthogonalAxes[2].dot( transformedPoint ) ) <= mEdgeHalfLengths[2] )
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

    this->calculateCornerPoints( thisCorners );
    otherBox.calculateCornerPoints( otherCorners );

    // These corner points should be precalculated already
    // Each face is determined by it's normal & a point in it, since it's basically a plane
    Vector3f thisFaces[6][2] = { { -mOrthogonalAxes[0], thisCorners[0] },
                                 { -mOrthogonalAxes[1], thisCorners[0] },
                                 { -mOrthogonalAxes[2], thisCorners[0] },
                                 {  mOrthogonalAxes[0], thisCorners[7] },
                                 {  mOrthogonalAxes[1], thisCorners[7] },
                                 {  mOrthogonalAxes[2], thisCorners[7] } };
    Vector3f otherFaces[6][2] = { { -otherBox.mOrthogonalAxes[0], otherCorners[0] },
                                  { -otherBox.mOrthogonalAxes[1], otherCorners[0] },
                                  { -otherBox.mOrthogonalAxes[2], otherCorners[0] },
                                  {  otherBox.mOrthogonalAxes[0], otherCorners[7] },
                                  {  otherBox.mOrthogonalAxes[1], otherCorners[7] },
                                  {  otherBox.mOrthogonalAxes[2], otherCorners[7] } };

    // The method of separating axes for obb collision detection works like this:
    // 1) The corners of box A are checked against each side of box B.
    // 2) The check consists on determining on which side (positive/negative) a corner
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
            // Transform to coordinates relative to the face's plane-determining point
            Vector3f transformedCorner = thisCorners[j] - otherFaces[i][1];
            float scalarProjection = otherFaces[i][0].dot( transformedCorner );

            // A negative projection has been found, no need to keep checking.
            // It does not mean that there is a collision though.
            if( scalarProjection < 0 )
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
            // Transform to coordinates relative to the face's plane-determining point
            Vector3f transformedCorner = otherCorners[j] - thisFaces[i][1];
            float scalarProjection = thisFaces[i][0].dot( transformedCorner );

            // A negative projection has been found, no need to keep checking.
            // It does not mean that there is a collision though.
            if( scalarProjection < 0 )
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
    calculateCornerPoints( corners );

    // These corner points are already in global reference, so we don't need to transform them
	glBegin( GL_QUADS );
        glColor3f( color[0], color[1], color[2] );
		// Front
        glNormal3f( mOrthogonalAxes[0][0], mOrthogonalAxes[0][1], mOrthogonalAxes[0][2] );
		glVertex3f( corners[1][0], corners[1][1], corners[1][2] );
		glVertex3f( corners[6][0], corners[6][1], corners[6][2] );
        glVertex3f( corners[7][0], corners[7][1], corners[7][2] );
        glVertex3f( corners[2][0], corners[2][1], corners[2][2] );

		// Right
        glNormal3f( mOrthogonalAxes[1][0], mOrthogonalAxes[1][1], mOrthogonalAxes[1][2] );
		glVertex3f( corners[2][0], corners[2][1], corners[2][2] );
		glVertex3f( corners[7][0], corners[7][1], corners[7][2] );
        glVertex3f( corners[4][0], corners[4][1], corners[4][2] );
        glVertex3f( corners[3][0], corners[3][1], corners[3][2] );

		// Back
        glNormal3f( -mOrthogonalAxes[0][0], -mOrthogonalAxes[0][1], -mOrthogonalAxes[0][2] );
		glVertex3f( corners[3][0], corners[3][1], corners[3][2] );
		glVertex3f( corners[4][0], corners[4][1], corners[4][2] );
        glVertex3f( corners[5][0], corners[5][1], corners[5][2] );
        glVertex3f( corners[0][0], corners[0][1], corners[0][2] );

		// Left
        glNormal3f( -mOrthogonalAxes[1][0], mOrthogonalAxes[1][1], mOrthogonalAxes[1][2] );
		glVertex3f( corners[0][0], corners[0][1], corners[0][2] );
		glVertex3f( corners[5][0], corners[5][1], corners[5][2] );
        glVertex3f( corners[6][0], corners[6][1], corners[6][2] );
        glVertex3f( corners[1][0], corners[1][1], corners[1][2] );

		// Top
        glNormal3f( mOrthogonalAxes[2][0], mOrthogonalAxes[2][1], mOrthogonalAxes[2][2] );
		glVertex3f( corners[6][0], corners[6][1], corners[6][2] );
		glVertex3f( corners[5][0], corners[5][1], corners[5][2] );
        glVertex3f( corners[4][0], corners[4][1], corners[4][2] );
        glVertex3f( corners[7][0], corners[7][1], corners[7][2] );

		// Bottom
        glNormal3f( -mOrthogonalAxes[2][0], -mOrthogonalAxes[2][1], -mOrthogonalAxes[2][2] );
		glVertex3f( corners[1][0], corners[1][1], corners[1][2] );
		glVertex3f( corners[2][0], corners[2][1], corners[2][2] );
        glVertex3f( corners[3][0], corners[3][1], corners[3][2] );
        glVertex3f( corners[0][0], corners[0][1], corners[0][2] );
	glEnd();
}

// Calculate the corner points of the box; used for collision detection and drawing
void OrientedBoundingBox::calculateCornerPoints( Vector3f corners[] ) const
{
    corners[0] = mCenter - mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         - mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         - mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[1] = mCenter + mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         - mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         - mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[2] = mCenter + mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         + mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         - mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[3] = mCenter - mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         + mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         - mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[4] = mCenter - mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         + mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         + mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[5] = mCenter - mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         - mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         + mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[6] = mCenter + mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         - mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         + mOrthogonalAxes[2] * mEdgeHalfLengths[2];
    corners[7] = mCenter + mOrthogonalAxes[0] * mEdgeHalfLengths[0]
                         + mOrthogonalAxes[1] * mEdgeHalfLengths[1]
                         + mOrthogonalAxes[2] * mEdgeHalfLengths[2];
}

void OrientedBoundingBox::printCornerPoints( std::ostream & os ) const
{
    Vector3f cornerPoints[8];
    this->calculateCornerPoints( cornerPoints );

    for( int i = 0; i < 8; i++ )
    {
        os << cornerPoints[i] << std::endl;
    }
}
