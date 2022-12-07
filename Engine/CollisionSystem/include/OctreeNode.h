#ifndef EAE6320_COLLISION_OCTREENODE_H
#define EAE6320_COLLISION_OCTREENODE_H

#include "OctreeNode.h"
#include "ColliderBase.h"
#include "AABBCollider.h"

namespace eae6320 {
	namespace Collision {
		class OctreeNode
		{
		public:
			
			int m_index;
			OctreeNode(float i_minX, float i_maxX, float i_minY, float i_maxY, float i_minZ, float i_maxZ, int index);
			void Insert(const AABBCollider* colliderBase);
			void Check();
		private:
			OctreeNode** m_children;
			const AABBCollider** m_colliders;
			int m_capacity;
			int m_childrenAmount;
			
			void Split();
			void InsertChildren(const AABBCollider* colliderBase);
			void Enlarge();
			bool IsIntersect(const AABBCollider* colliderBase, const OctreeNode* treeNode);
			bool IsIntersect(const AABBCollider* colliderBase, const AABBCollider* treeNode);

			float minX, maxX;
			float minY, maxY;
			float minZ, maxZ;
		};
	}
}


#endif
