#ifndef EAE6320_COLLISION_OCTREE_H
#define EAE6320_COLLISION_OCTREE_H

#include "OctreeNode.h"
#include "ColliderBase.h"

namespace eae6320 {
	namespace Collision {
		class Octree
		{
		public:
			OctreeNode* root;
			Octree();
			void Check();
			void Insert(const ColliderBase* colliderBase);

		};
	}
}


#endif
