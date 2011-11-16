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
        Octree( Vector3f minCorner, Vector3f maxCorner );
        ~Octree();

        void addBox( OrientedBoundingBox * box );
        void removeBox( OrientedBoundingBox * box );
        void getPotentialCollisionPairs( std::vector<BoxPair> & pairs ) const;
        void drawOctreeFrame( Vector3f color ) const;

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
        void initializeNode( OctreeNode * node, Vector3f minCorner, Vector3f maxCorner, int depth );
        // Allocate and initialize children, put boxes from parent into children
        // based on their position
        void createChildren( OctreeNode * node );
        // Insert the box into the appropriate node in the octree. Helper for addBox()
        void insertBox( OrientedBoundingBox * box, OctreeNode * node );
        // Remove the box from this node in the octree. Helper for public removeBox()
        void removeBox( OrientedBoundingBox * box, OctreeNode * node );
        // Collect the boxes of the children of this node into the set
        void collectBoxesFromChildren( OctreeNode * node, std::set<OrientedBoundingBox *> & collectedBoxes );
        // Destroy the children of this node, and collect all their boxes into this node
        void collapseChildren( OctreeNode * node );
        // Deallocate all of the nodes in the tree below & including this one.
        // Kill all the children, too :(
        void destroyOctreeNode( OctreeNode * node );
        // Populates the vector with potential collision pairs
        void getPotentialCollisionPairs( OctreeNode * node, std::vector<BoxPair> & pairs ) const;

        // Draw the outlines of the space divvied up by the octree
        void drawNodeAndChildren( OctreeNode * node, Vector3f color ) const;

        OctreeNode * mRoot;
};

#endif
