#ifndef EAE6320_COLLISION_OBBCOLLIDER_H
#define EAE6320_COLLISION_OBBCOLLIDER_H

#include "ColliderBase.h"

namespace eae6320 {
	namespace Collision {
		class OBBCollider : public ColliderBase
		{
		public:
			float size[3];

			OBBCollider(const Physics::sRigidBodyState* rigidBodyState, char* tag, float hX = 0.5f, float hY = 0.5f, float hZ = 0.5f);

			void SetSize(float hX, float hY, float hZ);
			Math::sVector GetSize();

		};
	}
}

#endif
