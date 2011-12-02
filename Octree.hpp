#ifndef OCTREE_HPP
#define OCTREE_HPP

#include "Vector3f.hpp"
#include "OrientedBoundingBox.hpp"
#include <iostream>
#include <set>    // Akin to STL vector class
#include <vector>

#define MAX_OCTREE_DEPTH 6
#define MIN_ELEMENTS_PER_OCTREE 3
#define MAX_ELEMENTS_PER_OCTREE 6

// Used for grouping possible collision pairs
struct BoxPair
{
    OrientedBoundingBox * box1;
    OrientedBoundingBox * box2;
};

class Octree
{
    public:
        Octree( const Vector3f & minCorner, const Vector3f & maxCorner );
        ~Octree() { destroyOctreeNode( mRoot ); };

        void addBox( OrientedBoundingBox * box ) { insertBox( box, mRoot ); };
        void removeBox( OrientedBoundingBox * box ) { removeBox( box, mRoot ); };
        void getPotentialCollisionPairs( std::vector<BoxPair> & pairs ) const { getPotentialCollisionPairs( mRoot, pairs ); };
        void draw( Vector3f color ) const { drawNodeAndChildren( mRoot, color ); };

    private:
        // One eighth of the space. Each level has 8 of these, hence octree.
        struct OctreeNode
        {
            Vector3f minCorner;
            Vector3f maxCorner;
            Vector3f center;

            bool hasChildren;
            OctreeNode * children[2][2][2];

            int depth;
            int numBoxes;    // Sum of boxes in this node and all below it
            std::set<OrientedBoundingBox *> boxes;
        };

        // Initializes an allocated node to have no boxes, no children, etc.
        static void initializeNode( OctreeNode * const node, const Vector3f & minCorner, const Vector3f & maxCorner, int depth );
        // Allocate and initialize children, put boxes from parent into children
        // based on their position
        static void createChildren( OctreeNode * const node );
        // Insert the box into the appropriate node in the octree. Helper for addBox()
        static void insertBox( OrientedBoundingBox * const box, OctreeNode * const node );
        // Remove the box from this node in the octree. Helper for public removeBox()
        static void removeBox( OrientedBoundingBox * const box, OctreeNode * const node );
        // Collect the boxes of the children of this node into the set
        static void collectBoxesFromChildren( OctreeNode * const node, std::set<OrientedBoundingBox *> & collectedBoxes );
        // Destroy the children of this node, and collect all their boxes into this node
        static void collapseChildren( OctreeNode * const node );
        // Deallocate all of the nodes in the tree below & including this one.
        // Kill all the children, too :(
        static void destroyOctreeNode( OctreeNode *& node );
        // Populates the vector with potential collision pairs
        static void getPotentialCollisionPairs( OctreeNode *const node, std::vector<BoxPair> & pairs );

        // Draw the outlines of the space divvied up by the octree
        static void drawNodeAndChildren( OctreeNode * const node, const Vector3f & color );

        OctreeNode * mRoot;
};

#endif
