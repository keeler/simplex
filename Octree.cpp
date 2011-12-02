#include "Octree.hpp"
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
    node->center = Vector3f( ( minCorner[0] + maxCorner[0] ) / 2,
                             ( minCorner[1] + maxCorner[1] ) / 2,
                             ( minCorner[2] + maxCorner[2] ) / 2 );

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
	initializeNode( node->children[1][0][0], Vector3f( node->center[0],    node->minCorner[1], node->minCorner[2] ), Vector3f( node->maxCorner[0], node->center[1],    node->maxCorner[2] ), newDepth );
	initializeNode( node->children[1][0][1], Vector3f( node->center[0],    node->minCorner[1], node->center[2] ),    Vector3f( node->maxCorner[0], node->center[1],    node->center[2] ),    newDepth );
	initializeNode( node->children[1][1][0], Vector3f( node->center[0],    node->center[1],    node->minCorner[2] ), Vector3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] ), newDepth );
	initializeNode( node->children[1][1][1], Vector3f( node->center[0],    node->center[1],    node->center[2] ),    Vector3f( node->maxCorner[0], node->maxCorner[1], node->center[2] ),    newDepth );

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

void Octree::collectBoxesFromChildren( OctreeNode * const node, std::set<OrientedBoundingBox *> & collectedBoxes )
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
    // Must destroy my children before destroying myself.
    // Feel like psycho typing such things.
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

void Octree::getPotentialCollisionPairs( OctreeNode * const node, std::vector<BoxPair> & pairs )
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
        for( std::set<OrientedBoundingBox *>::iterator it = node->boxes.begin();
             it != node->boxes.end();
             ++it )
        {
            for( std::set<OrientedBoundingBox *>::iterator it2 = node->boxes.begin();
                 it2 != node->boxes.end();
                 ++it2 )
            {
                // This test makes sure that we only add each pair once
                if( *it < *it2 )
                {
                    BoxPair pair;
                    pair.box1 = *it;
                    pair.box2 = *it2;
                    pairs.push_back( pair );
                }
            }
        }
    }
}

void Octree::drawNodeAndChildren( OctreeNode * const node, const Vector3f & color )
{
    // Draw octree partitions as wireframe cubes
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if( node->hasChildren )
    {
        glColor3f( color[0], color[1], color[2] );
        glBegin( GL_QUADS );
            // Back
            glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );

            // Right
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );

            // Front
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );

            // Left
            glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );

            // Top
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );

            // Bottom
            glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );
        glEnd();

        for( int x = 0; x < 2; x++ )
        {
            for( int y = 0; y < 2; y++ )
            {
                for( int z = 0; z < 2; z++ )
                {
                    drawNodeAndChildren( node->children[x][y][z], color );
                }
            }
        }
    }
    else
    {
        glColor3f( color[0], color[1], color[2] );
        glBegin( GL_QUADS );
            // Back
            glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );

            // Right
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );

            // Front
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );

            // Left
            glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );

            // Top
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->maxCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->maxCorner[0], node->minCorner[1], node->maxCorner[2] );

            // Bottom
            glVertex3f( node->minCorner[0], node->minCorner[1], node->minCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->minCorner[2] );
            glVertex3f( node->minCorner[0], node->maxCorner[1], node->maxCorner[2] );
            glVertex3f( node->minCorner[0], node->minCorner[1], node->maxCorner[2] );
        glEnd();
    }

    // Make it so we draw with filled polygons again
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

