#ifndef EAE6320_COLLISION_AABBCOLLIDER_H
#define EAE6320_COLLISION_AABBCOLLIDER_H
#include "ColliderBase.h"

namespace eae6320 {
	namespace Collision {
		class AABBCollider: public ColliderBase
		{
		public:
			float size[3];

			AABBCollider(const Physics::sRigidBodyState* rigidBodyState, char* i_tag, float hX = 0.5f, float hY = 0.5f, float hZ = 0.5f);
			void SetSize(float hX, float hY, float hZ);
			Math::sVector GetSize();

		};
	}
}

#endif
