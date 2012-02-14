#include "Octree.hpp"
#include "Plane.hpp"
#include "GL/glut.h"

Octree::Octree( const Vector3f & minCorner, const Vector3f & maxCorner )
{
    mRoot = new OctreeNode;

    // Root has depth of 1
    initializeNode( mRoot, minCorner, maxCorner, 1 );
}

void Octree::initializeNode( OctreeNode * const node, const Vector3f & minCorner, const Vector3f & maxCorner, int depth )
{
    node->minCorner = minCorner;
    node->maxCorner = maxCorner;
    node->center = ( minCorner + maxCorner ) / 2;
    node->hasChildren = false;
    node->numBoxes = 0;
    node->depth = depth;
}

void Octree::createChildren( OctreeNode * const node )
{
	node->children[0][0][0] = new OctreeNode;
	node->children[0][0][1] = new OctreeNode;
	node->children[0][1][0] = new OctreeNode;
	node->children[0][1][1] = new OctreeNode;
	node->children[1][0][0] = new OctreeNode;
	node->children[1][0][1] = new OctreeNode;
	node->children[1][1][0] = new OctreeNode;
	node->children[1][1][1] = new OctreeNode;

	int newDepth = node->depth + 1;
	initializeNode( node->children[0][0][0], Vector3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] ), Vector3f( node->center[0],    node->center[1],    node->center[2] ),    newDepth );
	initializeNode( node->children[0][0][1], Vector3f( node->minCorner[0], node->minCorner[1], node->center[2] ),    Vector3f( node->center[0],    node->center[1],    node->maxCorner[2] ), newDepth );
	initializeNode( node->children[0][1][0], Vector3f( node->minCorner[0], node->center[1],    node->minCorner[2] ), Vector3f( node->center[0],    node->maxCorner[1], node->center[2] ),    newDepth );
	initializeNode( node->children[0][1][1], Vector3f( node->minCorner[0], node->center[1],    node->center[2] ),    Vector3f( node->center[0],    node->maxCorner[1], node->maxCorner[2] ), newDepth );
	initializeNode( node->children[1][0][0], Vector3f( node->center[0],    node->minCorner[1], node->minCorner[2] ), Vector3f( node->maxCorner[0], node->center[1],    node->center[2] ), newDepth );
	initializeNode( node->children[1][0][1], Vector3f( node->center[0],    node->minCorner[1], node->center[2] ),    Vector3f( node->maxCorner[0], node->center[1],    node->maxCorner[2] ),    newDepth );
	initializeNode( node->children[1][1][0], Vector3f( node->center[0],    node->center[1],    node->minCorner[2] ), Vector3f( node->maxCorner[0], node->maxCorner[1], node->center[2] ), newDepth );
	initializeNode( node->children[1][1][1], Vector3f( node->center[0],    node->center[1],    node->center[2] ),    Vector3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] ),    newDepth );

    // Now, node has children
    node->hasChildren = true;

    // Set this node's number of boxes to 0, and add all of its boxes into its children
    // It's number of boxes is incremented each call to insertBox(), so it will be basically
    // no change to the numBoxes field
    node->numBoxes = 0;
    for( std::set<OrientedBoundingBox *>::iterator it = node->boxes.begin();
         it != node->boxes.end();
         ++it )
    {
        insertBox( *it, node );
    }
    node->boxes.clear();
}

// Insert the box into the appropriate node in the octree
// Will create children if necessary, and insert the box in the
// appropriate child(ren)
void Octree::insertBox( OrientedBoundingBox * const box, OctreeNode * const node )
{
    // If this node has no children, but can, create its children
    if( !node->hasChildren &&
        node->depth < MAX_OCTREE_DEPTH &&
        node->numBoxes + 1 > MAX_ELEMENTS_PER_OCTREE )
    {
        createChildren( node );
    }

    // If it does have children, try to insert to insert the box into one
    // or more of them
    if( node->hasChildren )
    {
        Vector3f boxCenter = box->getCenter();
        float boxRadius = box->getRadius();

        // Figure out in which child(ren) the box belongs
        // It is ok for a box to end up in two or more different branches
        // of a tree, since that would mean that it's overlapping all those regions.
        // Note that each continue statement below basically means, "this is
        // NOT the child (region) that box should go in
        for( int x = 0; x < 2; x++ )
        {
            if( x == 0 )
            {
                if( boxCenter[0] - boxRadius > node->center[0] )
                {
                    continue;
                }
            }
            else if( boxCenter[0] + boxRadius < node->center[0] )
            {
                continue;
            }

            for( int y = 0; y < 2; y++ )
            {
                if( y == 0 )
                {
                    if( boxCenter[1] - boxRadius > node->center[1] )
                    {
                        continue;
                    }
                }
                else if( boxCenter[1] + boxRadius < node->center[1] )
                {
                    continue;
                }

                for( int z = 0; z < 2; z++ )
                {
                    if( z == 0 )
                    {
                        if( boxCenter[2] - boxRadius > node->center[2] )
                        {
                            continue;
                        }
                    }
                    else if( boxCenter[2] - boxRadius < node->center[2] )
                    {
                        continue;
                    }

                    // Add the box to the appropriate child
                    insertBox( box, node->children[x][y][z] );
                }
            }
        }
    }
    // If it has no children, just add to the node's set of boxes
    else
    {
        node->boxes.insert( box );
    }

    node->numBoxes++;
}

// Removes the box from this node, helper function for public version
void Octree::removeBox( OrientedBoundingBox * const box, OctreeNode * const node )
{
    // Removing a box, keep track
    node->numBoxes--;

    if( node->hasChildren && node->numBoxes < MIN_ELEMENTS_PER_OCTREE )
    {
        collapseChildren( node );
    }

    // If node has children, try to delete box from child
    if( node->hasChildren )
    {
        Vector3f boxCenter = box->getCenter();
        float boxRadius = box->getRadius();

        // Figure out in which child(ren) the box belongs, then remove it
        // It is ok for a box to be removed from two or more different branches
        // of a tree, since that would mean that it's overlapping all those regions.
        // Note that each continue statement below basically means, "this is
        // NOT the child (region) that box should be removed from
        for( int x = 0; x < 2; x++ )
        {
            if( x == 0 )
            {
                if( boxCenter[0] - boxRadius > node->center[0] )
                {
                    continue;
                }
            }
            else if( boxCenter[0] + boxRadius < node->center[0] )
            {
                continue;
            }

            for( int y = 0; y < 2; y++ )
            {
                if( y == 0 )
                {
                    if( boxCenter[1] - boxRadius > node->center[1] )
                    {
                        continue;
                    }
                }
                else if( boxCenter[1] + boxRadius < node->center[1] )
                {
                    continue;
                }

                for( int z = 0; z < 2; z++ )
                {
                    if( z == 0 )
                    {
                        if( boxCenter[2] - boxRadius > node->center[2] )
                        {
                            continue;
                        }
                    }
                    else if( boxCenter[2] - boxRadius < node->center[2] )
                    {
                        continue;
                    }

                    // Remove box from the appropriate child
                    removeBox( box, node->children[x][y][z] );
                }
            }
        }
    }
    // If this is a leaf node, just remove from this node's set of boxes
    else
    {
        node->boxes.erase( box );
    }
}

void Octree::collectBoxesFromChildren( const OctreeNode * const node, std::set<OrientedBoundingBox *> & collectedBoxes )
{
    // Recurse on children
    if( node->hasChildren )
    {
        collectBoxesFromChildren( node->children[0][0][0], collectedBoxes );
        collectBoxesFromChildren( node->children[0][0][1], collectedBoxes );
        collectBoxesFromChildren( node->children[0][1][0], collectedBoxes );
        collectBoxesFromChildren( node->children[0][1][1], collectedBoxes );
        collectBoxesFromChildren( node->children[1][0][0], collectedBoxes );
        collectBoxesFromChildren( node->children[1][0][1], collectedBoxes );
        collectBoxesFromChildren( node->children[1][1][0], collectedBoxes );
        collectBoxesFromChildren( node->children[1][1][1], collectedBoxes );
    }
    // For leaf nodes
    else
    {
        for( std::set<OrientedBoundingBox *>::iterator it = node->boxes.begin();
             it != node->boxes.end();
             ++it )
        {
            collectedBoxes.insert( *it );
        }
    }
}

void Octree::collectBoxesFromChildren( const OctreeNode * const node, std::vector<OrientedBoundingBox *> & collectedBoxes )
{
    // Recurse on children
    if( node->hasChildren )
    {
        collectBoxesFromChildren( node->children[0][0][0], collectedBoxes );
        collectBoxesFromChildren( node->children[0][0][1], collectedBoxes );
        collectBoxesFromChildren( node->children[0][1][0], collectedBoxes );
        collectBoxesFromChildren( node->children[0][1][1], collectedBoxes );
        collectBoxesFromChildren( node->children[1][0][0], collectedBoxes );
        collectBoxesFromChildren( node->children[1][0][1], collectedBoxes );
        collectBoxesFromChildren( node->children[1][1][0], collectedBoxes );
        collectBoxesFromChildren( node->children[1][1][1], collectedBoxes );
    }
    // For leaf nodes
    else
    {
        for( std::set<OrientedBoundingBox *>::iterator it = node->boxes.begin();
             it != node->boxes.end();
             ++it )
        {
            collectedBoxes.push_back( *it );
        }
    }
}

void Octree::collapseChildren( OctreeNode * const node )
{
    collectBoxesFromChildren( node, node->boxes );

    if( node->hasChildren )
    {
        destroyOctreeNode( node->children[0][0][0] );
        destroyOctreeNode( node->children[0][0][1] );
        destroyOctreeNode( node->children[0][1][0] );
        destroyOctreeNode( node->children[0][1][1] );
        destroyOctreeNode( node->children[1][0][0] );
        destroyOctreeNode( node->children[1][0][1] );
        destroyOctreeNode( node->children[1][1][0] );
        destroyOctreeNode( node->children[1][1][1] );
    }

    node->hasChildren = false;
}

void Octree::destroyOctreeNode( OctreeNode *& node )
{
    // Recurse on children first
    if( node->hasChildren )
    {
        destroyOctreeNode( node->children[0][0][0] );
        destroyOctreeNode( node->children[0][0][1] );
        destroyOctreeNode( node->children[0][1][0] );
        destroyOctreeNode( node->children[0][1][1] );
        destroyOctreeNode( node->children[1][0][0] );
        destroyOctreeNode( node->children[1][0][1] );
        destroyOctreeNode( node->children[1][1][0] );
        destroyOctreeNode( node->children[1][1][1] );
    }

    delete node;
    node = NULL;
}

void Octree::getPotentialCollisionPairs( const OctreeNode * const node, std::vector<BoxPair> & pairs )
{
    if( node->hasChildren )
    {
        getPotentialCollisionPairs( node->children[0][0][0], pairs );
        getPotentialCollisionPairs( node->children[0][0][1], pairs );
        getPotentialCollisionPairs( node->children[0][1][0], pairs );
        getPotentialCollisionPairs( node->children[0][1][1], pairs );
        getPotentialCollisionPairs( node->children[1][0][0], pairs );
        getPotentialCollisionPairs( node->children[1][0][1], pairs );
        getPotentialCollisionPairs( node->children[1][1][0], pairs );
        getPotentialCollisionPairs( node->children[1][1][1], pairs );
    }
    else
    {
		BoxPair pair;
		int i = 0;    // Loop control to make sure we don't check too many pairs
		for( std::set<OrientedBoundingBox *>::const_iterator it = node->boxes.begin();
			 i < node->numBoxes - 1;
			 i++ )
		{
			std::set<OrientedBoundingBox *>::const_iterator temp = it;
			for( std::set<OrientedBoundingBox *>::const_iterator it2 = ++it;
				 it2 != node->boxes.end();
				 ++it2 )
			{
				pair.box1 = *temp;
				pair.box2 = *it2;
				pairs.push_back( pair );
			}
		}
    }
}

void Octree::getBoxesWithinFrustum( const OctreeNode * const node, const Frustum & frustum, std::vector<OrientedBoundingBox *> & visibleBoxes )
{
	int status = -1;                // Assume inside; -1 = inside, 0 = outside; 1 = intersect

	const Vector3f * frustumCorners = frustum.getCorners();
	const Vector3f * frustumNormals = frustum.getNormals();

	Plane frustumPlanes[6] = { Plane( frustumNormals[Frustum::NEAR],   frustumCorners[Frustum::NTR] ),		// Near
							   Plane( frustumNormals[Frustum::TOP],    frustumCorners[Frustum::NTR] ),		// Top
							   Plane( frustumNormals[Frustum::RIGHT],  frustumCorners[Frustum::NTR] ),		// Right
							   Plane( frustumNormals[Frustum::FAR],    frustumCorners[Frustum::FBL] ),		// Far
							   Plane( frustumNormals[Frustum::BOTTOM], frustumCorners[Frustum::FBL] ),		// Bottom
							   Plane( frustumNormals[Frustum::LEFT],   frustumCorners[Frustum::FBL] ) };	// Left

	bool intersectFlag = false;
	// Do a sphere cull first, if the box is outside the frustum this will probably be much faster
	if( !frustum.isSphereInFrustum( node->center, ( node->maxCorner - node->center ).magnitude() ) )
	{
		status = 0;
	}
	// The box potentially intersects or is inside the frustum. Could still be outside, though.
	else
	{
		float nodeRadius = node->maxCorner[0] - node->center[0];
		for( int i = 0; i < 6; i++ )
		{
			Vector3f currentNormal = frustumPlanes[i].getNormal();
			Vector3f direction = Vector3f( 1.0, 1.0, 1.0 );    // Default to pointing towards maxCorner

			if( currentNormal[0] >= 0 )
			{
				direction[0] = -1.0;
			}
			if( currentNormal[1] >= 0 )
			{
				direction[1] = -1.0;
			}
			if( currentNormal[2] >= 0 )
			{
				direction[2] = -1.0;
			}

			// Node corner closest to plane
			Vector3f closePoint = node->center + direction * nodeRadius;
			
			// If the closest point isn't inside the frustum, then the node must be outside the frustum
			if( frustumPlanes[i].isInPositiveHalfSpace( closePoint ) )
			{
				status = 0;
				break;
			}

			// Node corner farthest from plane, simply opposite direction from closePoint
			Vector3f farPoint = node->center - direction * nodeRadius;

			if( frustumPlanes[i].isInPositiveHalfSpace( farPoint ) )
			{
				intersectFlag = true;
			}
		}
	}

	// If status hasn't been changed to 'outside', and the intersect flag is
	// set, then the node is only partially within the frustum
	if( status != 0 && intersectFlag )
	{
		status = 1;
	}

	// If the node is completely outside, no boxes within it or any of its
	// children could be inside
	if( status == 0 )
	{
		return;
	}
	// If the node is completely inside, then all of the boxes within it and
	// its children are inside
	else if( status == -1 )
	{
		collectBoxesFromChildren( node, visibleBoxes );
	}
	else
	{
		if( node->hasChildren )
		{
			getBoxesWithinFrustum( node->children[0][0][0], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[0][0][1], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[0][1][0], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[0][1][1], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[1][0][0], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[1][0][1], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[1][1][0], frustum, visibleBoxes );
			getBoxesWithinFrustum( node->children[1][1][1], frustum, visibleBoxes );
		}
		else
		{
			for( std::set<OrientedBoundingBox *>::iterator it = node->boxes.begin();
				 it != node->boxes.end();
				 ++it )
			{
				visibleBoxes.push_back( *it );
			}
		}
	}
}

void Octree::drawNode( const OctreeNode * const node, const Vector3f & color )
{
	glColor3f( color[0], color[1], color[2] );
    glBegin( GL_QUADS );
        // Top
        glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );
        glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );

		// Bottom
        glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
        glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
        
        // Left
        glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
        glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );
        glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
        glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );

		// Right
        glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
        glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );

		// Far
        glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );
        glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
        glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );

		// Near
        glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
        glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );
        glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
        glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
    glEnd();
}

void Octree::drawNodeAndChildren( const OctreeNode * const node, const Vector3f & color )
{
	drawNode( node, color );

    if( node->hasChildren )
    {
		drawNode( node->children[0][0][0], color );
		drawNode( node->children[0][0][1], color );
		drawNode( node->children[0][1][0], color );
		drawNode( node->children[0][1][1], color );
		drawNode( node->children[1][0][0], color );
		drawNode( node->children[1][0][1], color );
		drawNode( node->children[1][1][0], color );
		drawNode( node->children[1][1][1], color );
    }

}
