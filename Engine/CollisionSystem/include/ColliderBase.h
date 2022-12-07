#ifndef EAE6320_COLLISION_COLLIDERBASE_H
#define EAE6320_COLLISION_COLLIDERBASE_H

#include <functional>
#include "Engine/Math/sVector.h"
#include "Engine/Math/cQuaternion.h"
//#include "Engine/Graphics/GameObject.h"
#include "Windows/ExternalLibraries.win.h"
#ifdef EAE6320_PLATFORM_D3D
struct D3D11_MAPPED_SUBRESOURCE;
struct D3D11_BUFFER_DESC;
#endif
namespace eae6320 {
	namespace Collision {
		
		enum class ColliderType
		{
			AABB,
			OBB,
			Sphere,
			None
		};
		class ColliderBase
		{
		public:
			bool isActive;
			char* tag;
			ColliderType m_type;
			const Physics::sRigidBodyState* rigidBodyState;
			
			ColliderBase();
			void OverLap(const ColliderBase* i_collider) const;
			void SetActive(bool active);
			void SetTransform(Math::sVector pos, Math::sVector rotate);
			std::function<void(const ColliderBase*)> OnColliderEnter;
			std::function<void(const ColliderBase*)> OnColliderStay;
			std::function<void(const ColliderBase*)> OnColliderExit;

			void CheckColliders(const float i_secondCountToIntegrate);
			

		protected:	
			Math::sVector m_positionOffset;
			Math::sVector m_rotationOffset;
		private:
			const ColliderBase** colliders;

			
		};
	}
}

#endif
