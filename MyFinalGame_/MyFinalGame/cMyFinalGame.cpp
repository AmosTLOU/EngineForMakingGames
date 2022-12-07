// Includes
//=========
#include <Engine/Graphics/Graphics.h>
#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Math/cMatrix_transformation.h>
#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/sVector.h>
#include <Engine/NetworkingClient/cMyNetworkClient.h>

#include <thread>

#include "cMyFinalGame.h"
#include "Collision.h"

#define NetClientID "0"

// Run
//----

void eae6320::MyFinalGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::MyFinalGame::UpdateBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	Collision::Update(i_elapsedSecondCount_sinceLastUpdate);
}

// Initialize / Clean Up
//----------------------
void eae6320::MyFinalGame::InitGraphicsContent()
{
	auto result = Results::Success;
	/**
		Planes
	*/
	{
		for (size_t i = 0; i < 2; i++)
		{
			std::vector<eae6320::Graphics::MyMesh*> meshes;
			std::vector<eae6320::Graphics::MyEffect*> effects;
			// Initialize the shading data
			// Effect No.1 
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/standard.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}
				effects.push_back(pEffect);
			}
			// Effect No.2
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/damaged.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}

				effects.push_back(pEffect);
			}
			// Initialize the geometry
			std::string mesh_path = "";
			if (i == 0)
			{
				mesh_path = "data/meshes/plane_1.mesh_bin";
			}
			else
			{
				mesh_path = "data/meshes/plane_2.mesh_bin";
			}
			// Mesh No.1
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, mesh_path)))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}
			// Mesh No.2
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, mesh_path)))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}
			auto object = new RenderableObject(meshes, effects);
			Collision::AABBCollider* collider;
			float ColliderWidth = 0.15f;
			if (i == 0)
			{
				object->rbState.position = eae6320::Math::sVector(0.0f, -1.5f, 0.0f);
				collider = new Collision::AABBCollider(&(object->rbState), "plane1", ColliderWidth, ColliderWidth, ColliderWidth);
				collider->OnColliderEnter = [this, i](const Collision::ColliderBase* collider)
				{
					if (collider->tag == "bullet2" || collider->tag == "enemy")
					{
						plane[i]->health -= 5;
						if (plane[i]->health <= 0)
							plane[i]->health = 0;
						plane[i]->hurtingRemainTime += 1.5f;
					}
				};
			}				
			else
			{
				object->rbState.position = eae6320::Math::sVector(0.0f, 1.5f, 0.0f);
				collider = new Collision::AABBCollider(&(object->rbState), "plane2", ColliderWidth, ColliderWidth, ColliderWidth);
				collider->OnColliderEnter = [this, i](const Collision::ColliderBase* collider)
				{
					if (collider->tag == "bullet1" || collider->tag == "enemy")
					{
						plane[i]->health -= 5;
						if (plane[i]->health <= 0)
							plane[i]->health = 0;
						if(plane[i]->hurtingRemainTime <= 1.5f)
							plane[i]->hurtingRemainTime = 1.5f;
					}
				};
			}	
			plane[i] = object;			
			Collision::AddCollider(collider);				
		}
	}
	/**
		Enemies
	*/
	{
		size_t cnt_enemies = 40;
		for (size_t i = 0; i < cnt_enemies; i++)
		{
			std::vector<eae6320::Graphics::MyMesh*> meshes;
			std::vector<eae6320::Graphics::MyEffect*> effects;
			// Initialize the shading data
			// Effect No.1
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/animatedcolor.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}
				effects.push_back(pEffect);
			}
			// Initialize the geometry
			// Mesh No.1
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/obj_2_mesh_1.mesh_bin")))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}
			auto object = new RenderableObject(meshes, effects);
			object->alive = false;
			object->rbState.position = eae6320::Math::sVector(99.0f, 99.0f, 0.0f);
			float ColliderWidth = 0.1f;
			Collision::AABBCollider* collider = new Collision::AABBCollider(&(object->rbState), "enemy", ColliderWidth, ColliderWidth, ColliderWidth);
			collider->OnColliderEnter = [this, object](const Collision::ColliderBase* collider)
			{
				if (collider->tag != "enemy")
				{
					object->alive = false;
					object->rbState.position = eae6320::Math::sVector(99.0f, 99.0f, 0.0f);
					object->rbState.velocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
				}		
				if (collider->tag == "bullet1")
					planePower[0] += 0.2f;
				else if (collider->tag == "bullet2")
					planePower[1] += 0.2f;
				for (size_t k = 0; k < 2; k++)
				{
					if (5.0f <= planePower[k])
					{
						planeFiringType[k] = 2;
					}
					else if(1.0f <= planePower[k])
					{
						planeFiringType[k] = 1;
					}
					else
					{
						planeFiringType[k] = 0;
					}
				}
			};
			Collision::AddCollider(collider);
			enemies.push_back(object);
		}	
	}
	/**
		Bullets
	*/
	{
		size_t group_bullets = 2;
		for (size_t i = 0; i < group_bullets; i++)
		{
			size_t cnt_bullets = 100;
			for (size_t j = 0; j < cnt_bullets; j++)
			{
				std::vector<eae6320::Graphics::MyMesh*> meshes;
				std::vector<eae6320::Graphics::MyEffect*> effects;
				// Initialize the shading data
				// Effect No.1
				{
					eae6320::Graphics::MyEffect* pEffect = nullptr;
					if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
						"data/Shaders/Fragment/standard.shader")))
					{
						EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
						return;
					}
					effects.push_back(pEffect);
				}
				// Initialize the geometry
				// Mesh No.1
				{
					eae6320::Graphics::MyMesh* pMesh = nullptr;
					if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/obj_1_mesh_2.mesh_bin")))
					{
						EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
						return;
					}
					meshes.push_back(pMesh);
				}
				auto object = new RenderableObject(meshes, effects);
				object->alive = false;
				Collision::AABBCollider* collider;
				float ColliderWidth = 0.06f;
				if (i == 0)
				{	
					object->rbState.position = eae6320::Math::sVector(-99.0f, -99.0f, 0.0f);					
					collider = new Collision::AABBCollider(&(object->rbState), "bullet1", ColliderWidth, ColliderWidth, ColliderWidth);
					collider->OnColliderEnter = [this, object](const Collision::ColliderBase* collider)
					{
						if (collider->tag != "plane1" && collider->tag != "bullet1")
						{
							object->alive = false;
							object->rbState.position = eae6320::Math::sVector(-99.0f, -99.0f, 0.0f);
							object->rbState.velocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
						}
					};
				}				
				else
				{
					object->rbState.position = eae6320::Math::sVector(-99.0f, 99.0f, 0.0f);
					collider = new Collision::AABBCollider(&(object->rbState), "bullet2", ColliderWidth, ColliderWidth, ColliderWidth);
					collider->OnColliderEnter = [this, object](const Collision::ColliderBase* collider)
					{
						if (collider->tag != "plane2" && collider->tag != "bullet2")
						{
							object->alive = false;
							object->rbState.position = eae6320::Math::sVector(-99.0f, 99.0f, 0.0f);
							object->rbState.velocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
						}
					};
				}
				plane_bullets[i].push_back(object);
				Collision::AddCollider(collider);
			}
		}
	}
	/**
		Plane Power Bar
	*/
	{
		for (size_t i = 0; i < 2; i++)
		{
			std::vector<eae6320::Graphics::MyMesh*> meshes;
			std::vector<eae6320::Graphics::MyEffect*> effects;
			// Initialize the shading data
			// Effect No.1 
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/yellowcolor.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}
				effects.push_back(pEffect);
			}
			// Effect No.2 
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/yellowcolor.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}
				effects.push_back(pEffect);
			}
			// Effect No.3 
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/yellowcolor.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}
				effects.push_back(pEffect);
			}
			// Initialize the geometry
			// Mesh No.1
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/planePowerBar_1.mesh_bin")))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}
			// Mesh No.2
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/planePowerBar_2.mesh_bin")))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}
			// Mesh No.3
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/planePowerBar_3.mesh_bin")))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}
			auto object = new RenderableObject(meshes, effects);
			planePowerBar[i] = object;
		}
	}
	/**
	Plane Health bar
	*/
	{
		for (size_t i = 0; i < 2; i++)
		{
			std::vector<eae6320::Graphics::MyMesh*> meshes;
			std::vector<eae6320::Graphics::MyEffect*> effects;
			// Initialize the shading data
			// Effect No.1 
			{
				eae6320::Graphics::MyEffect* pEffect = nullptr;
				if (!(result = eae6320::Graphics::MyEffect::FactoryMethod(pEffect, "data/Shaders/Vertex/standard.shader",
					"data/Shaders/Fragment/green.shader")))
				{
					EAE6320_ASSERTF(false, "Can't create a new effect using the FactoryMethod");
					return;
				}
				effects.push_back(pEffect);
			}
			// Initialize the geometry
			// Mesh No.1 - No.100
			for (size_t i = 0; i < 100; i++)
			{
				eae6320::Graphics::MyMesh* pMesh = nullptr;
				if (!(result = eae6320::Graphics::MyMesh::CreateStaticMesh(pMesh, (i+1)/100.0f)))
				{
					EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
					return;
				}
				meshes.push_back(pMesh);
			}		
			planeHealthBar[i] = new RenderableObject(meshes, effects);
		}
	}
	
}

eae6320::cResult eae6320::MyFinalGame::Initialize()
{
	Networking::MyNetworkClient::StartClient();
	Collision::Initialize();
	InitGraphicsContent();	
	return Results::Success;
}


void eae6320::MyFinalGame::CleanUpGraphicsContent()
{
	// static objects
	for (auto& pObj : objects)
	{
		for (auto& pM : pObj->meshes)
			eae6320::Graphics::TryDecrementReferenceCount(pM);
		for (auto& pE : pObj->effects)
			eae6320::Graphics::TryDecrementReferenceCount(pE);
	}	
	// planes
	for (size_t i = 0; i < 2; i++)
	{
		for (auto& pM : plane[i]->meshes)
			eae6320::Graphics::TryDecrementReferenceCount(pM);
		for (auto& pE : plane[i]->effects)
			eae6320::Graphics::TryDecrementReferenceCount(pE);
	}
	// plane_bullets
	for (size_t i = 0; i < 2; i++)
	{
		for (auto& bullet : plane_bullets[i])
		{
			for (auto& pM : bullet->meshes)
				eae6320::Graphics::TryDecrementReferenceCount(pM);
			for (auto& pE : bullet->effects)
				eae6320::Graphics::TryDecrementReferenceCount(pE);
		}
	}
	// enemies
	for (auto& enemy : enemies)
	{
		for (auto& pM : enemy->meshes)
			eae6320::Graphics::TryDecrementReferenceCount(pM);
		for (auto& pE : enemy->effects)
			eae6320::Graphics::TryDecrementReferenceCount(pE);
	}
	// plane power bar
	for (size_t i = 0; i < 2; i++)
	{
		for (auto& pM : planePowerBar[i]->meshes)
			eae6320::Graphics::TryDecrementReferenceCount(pM);
		for (auto& pE : planePowerBar[i]->effects)
			eae6320::Graphics::TryDecrementReferenceCount(pE);
	}
	// plane power bar
	for (size_t i = 0; i < 2; i++)
	{
		for (auto& pM : planeHealthBar[i]->meshes)
			eae6320::Graphics::TryDecrementReferenceCount(pM);
		for (auto& pE : planeHealthBar[i]->effects)
			eae6320::Graphics::TryDecrementReferenceCount(pE);
	}
}


eae6320::cResult eae6320::MyFinalGame::CleanUp()
{
	Networking::MyNetworkClient::EndClient();
	Collision::CleanUp();
	CleanUpGraphicsContent();	
	return Results::Success;
}

void eae6320::MyFinalGame::UpdateSimulationBasedOnInput()
{
	bool hasSynced = false;

	//if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space))
	//{		
	//	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space) && !hasSynced)
	//	{
	//		Networking::MyNetworkClient::Sync("Space");
	//		hasSynced = true;
	//	}	
	//}

	// Change plane1's velocity based on input
	{
		auto character = plane[0];
		//if (!character)
		//	return;
		EAE6320_ASSERT(character);
		float acc = 0.2f;
		float maxMagnitude = 2.0f;
		auto& v_x = character->rbState.velocity.x;
		auto& v_y = character->rbState.velocity.y;
		auto& v_z = character->rbState.velocity.z;

		if (UserInput::IsKeyPressed(UserInput::KeyCodes::A) || Networking::MyNetworkClient::RecvMsg[1] == 'A')
		{
			if (!(Networking::MyNetworkClient::RecvMsg[1] == 'A'))
			{
				Networking::MyNetworkClient::Sync(NetClientID + std::string("A1"));
				hasSynced = true;
			}
			v_x += -acc;
			v_x = v_x < -maxMagnitude ? -maxMagnitude : v_x;
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::D) || Networking::MyNetworkClient::RecvMsg[1] == 'D')
		{
			if (!(Networking::MyNetworkClient::RecvMsg[1] == 'D'))
			{
				Networking::MyNetworkClient::Sync(NetClientID + std::string("D1"));
				hasSynced = true;
			}
			v_x += acc;
			v_x = maxMagnitude < v_x ? maxMagnitude : v_x;
		}
		else
		{
			v_x = 0;
		}
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::S) || Networking::MyNetworkClient::RecvMsg[1] == 'S')
		{
			if (!(Networking::MyNetworkClient::RecvMsg[1] == 'S'))
			{
				Networking::MyNetworkClient::Sync(NetClientID + std::string("S1"));
				hasSynced = true;
			}
			v_y += -acc;
			v_y = v_y < -maxMagnitude ? -maxMagnitude : v_y;
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::W) || Networking::MyNetworkClient::RecvMsg[1] == 'W')
		{
			if (!(Networking::MyNetworkClient::RecvMsg[1] == 'W'))
			{
				Networking::MyNetworkClient::Sync(NetClientID + std::string("W1"));
				hasSynced = true;
			}
			v_y += acc;
			v_y = v_y < -maxMagnitude ? -maxMagnitude : v_y;
		}
		else
		{
			v_y = 0;
		}
	}
	// Change plane2's velocity based on input
	{
		auto character = plane[1];
		//if (!character)
		//	return;
		EAE6320_ASSERT(character);
		float acc = 0.2f;
		float maxMagnitude = 2.0f;
		auto& v_x = character->rbState.velocity.x;
		auto& v_y = character->rbState.velocity.y;
		auto& v_z = character->rbState.velocity.z;
		
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left))
		{
			v_x += -acc;
			v_x = v_x < -maxMagnitude ? -maxMagnitude : v_x;

		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right))
		{
			v_x += acc;
			v_x = maxMagnitude < v_x ? maxMagnitude : v_x;
		}
		else
		{
			v_x = 0;
		}
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down))
		{
			v_y += -acc;
			v_y = v_y < -maxMagnitude ? -maxMagnitude : v_y;
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up))
		{
			v_y += acc;
			v_y = v_y < -maxMagnitude ? -maxMagnitude : v_y;
		}
		else
		{
			v_y = 0;
		}
	}	
	// Change camera's velocity based on input
	{
		/*auto& vel_z = camera.rbState.velocity.z;
		const float constantZoomAcc = 0.4f;
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::LeftMouse))
		{
			vel_z -= constantZoomAcc;
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::RightMouse))
		{
			vel_z += constantZoomAcc;
		}
		else
		{
			vel_z = 0;
		}*/
	}

	//// Sync too frequently might cause a crash
	//unsigned int interval = 2;
	//if(!hasSynced && counter % interval == 0)
	//	Networking::MyNetworkClient::Sync(NetClientID + std::string("M0"));
}


void eae6320::MyFinalGame::UpdateSimulationBasedOnTimePerCharacter(const float i_elapsedSecondCount_sinceLastUpdate, 
	RenderableObject* character, const char* characterTag="")
{
	EAE6320_ASSERT(character);
	//auto oldPosition = character->rbState.position;
	character->rbState.position = character->rbState.PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate);
	const float boundaryCenter = 0.3f;
	const float boundarySide = 1.9f;
	const float boundaryTop = 1.9f;
	const float boundaryWide = 3.0f;
	if (characterTag == "plane1")
	{		
		if (-boundaryCenter <= character->rbState.position.y)
			character->rbState.position.y = -boundaryCenter;
		if (character->rbState.position.y <= -boundaryTop)
			character->rbState.position.y = -boundaryTop;
		if (character->rbState.position.x <= -boundarySide)
			character->rbState.position.x = -boundarySide;
		if (boundarySide <= character->rbState.position.x)
			character->rbState.position.x = boundarySide;
		if (0.0f < character->hurtingRemainTime)
			character->hurtingRemainTime -= i_elapsedSecondCount_sinceLastUpdate;
	}
	else if (characterTag == "plane2")
	{
		if (character->rbState.position.y <= boundaryCenter)
			character->rbState.position.y = boundaryCenter;
		if (boundaryTop <= character->rbState.position.y)
			character->rbState.position.y = boundaryTop;
		if (character->rbState.position.x <= -boundarySide)
			character->rbState.position.x = -boundarySide;
		if (boundarySide <= character->rbState.position.x)
			character->rbState.position.x = boundarySide;
		if (0.0f < character->hurtingRemainTime)
			character->hurtingRemainTime -= i_elapsedSecondCount_sinceLastUpdate;
	}
	else if (characterTag == "bullet1" || characterTag == "bullet2" || characterTag == "enemy")
	{
		if (character->rbState.position.y < -boundaryWide || boundaryWide < character->rbState.position.y ||
			character->rbState.position.x < -boundaryWide || boundaryWide < character->rbState.position.x)
		{
			character->alive = false;			
			character->rbState.velocity = eae6320::Math::sVector(0.0f, 0.0f, 0.0f);
			if(characterTag == "bullet1")
				character->rbState.position = eae6320::Math::sVector(-99.0f, -99.0f, 0.0f);
			else if(characterTag == "bullet2")
				character->rbState.position = eae6320::Math::sVector(-99.0f, 99.0f, 0.0f);
			else if (characterTag == "enemy")
				character->rbState.position = eae6320::Math::sVector(99.0f, 99.0f, 0.0f);
		}			
	}
	for (auto& pMesh : character->meshes)
	{
		pMesh->constantData_drawCall.g_transform_localToWorld = eae6320::Math::cMatrix_transformation(
			eae6320::Math::cQuaternion(),
			eae6320::Math::sVector(character->rbState.position.x, character->rbState.position.y, character->rbState.position.z)
		);
	}
}


void eae6320::MyFinalGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	bool hasSynced = false;

	// Update plane1's position based on velocity
	{
		auto character = plane[0];
		UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character, "plane1");
	}
	// Update plane2's position based on velocity
	{
		auto character = plane[1];
		UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character, "plane2");
	}
	// Update plane1_bullets' position based on velocity
	{
		for (auto& bullet : plane_bullets[0])
		{
			auto character = bullet;
			UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character, "bullet1");
		}
	}
	// Update plane2_bullets' position based on velocity
	{
		for (auto& bullet : plane_bullets[1])
		{
			auto character = bullet;
			UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character, "bullet2");
		}
	}
	// Update enemies' position based on velocity
	{
		for (auto& enemy : enemies)
		{
			auto character = enemy;
			UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character, "enemy");
		}
	}
	// Update plane power bar
	{
		for (size_t i = 0; i < 2; i++)
		{
			auto character = planePowerBar[i];
			if (i == 0)
				character->rbState.position = plane[i]->rbState.position + eae6320::Math::sVector(0.0f, -0.22f, 0.0f);
			else
				character->rbState.position = plane[i]->rbState.position + eae6320::Math::sVector(0.0f, 0.22f, 0.0f);
			UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character);
		}
	}
	// Update plane health bar
	{
		for (size_t i = 0; i < 2; i++)
		{
			auto character = planeHealthBar[i];
			if (i == 0)
				character->rbState.position = plane[i]->rbState.position + eae6320::Math::sVector(0.0f, 0.24f, 0.0f);
			else
				character->rbState.position = plane[i]->rbState.position + eae6320::Math::sVector(0.0f, -0.24f, 0.0f);
			UpdateSimulationBasedOnTimePerCharacter(i_elapsedSecondCount_sinceLastUpdate, character);
		}
	}
	// Update camera's position based on velocity
	{
		camera.rbState.position = camera.rbState.PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate);
	}

	// planes firing
	for (size_t i = 0; i < 2; i++)
	{
		if (planeStopFiringRemainTime[i] <= 0.0f)
		{
			auto keyCode = UserInput::KeyCodes::Space;
			float sign = 1.0f;
			if (i == 1)
			{
				keyCode = UserInput::KeyCodes::Enter;
				sign = -1.0f;
			}				
			if (UserInput::IsKeyPressed(keyCode) || Networking::MyNetworkClient::RecvMsg[0] == 'F')
			{
				if (!(Networking::MyNetworkClient::RecvMsg[0] == 'F'))
				{
					Networking::MyNetworkClient::Sync(NetClientID + std::string("F1"));
					hasSynced = true;
				}					
				if (planeFiringType[i] != 2)
				{
					for (auto& bullet : plane_bullets[i])
					{
						if (!bullet->alive)
						{
							bullet->rbState.position = plane[i]->rbState.position;
							bullet->rbState.velocity = eae6320::Math::sVector(0.0f, sign*2.0f, 0.0f);
							bullet->alive = true;
							break;
						}
					}
				}
				else
				{
					size_t cntBulletsReleased = 0;
					float speedMagnitude = 2.0f;
					for (auto& bullet : plane_bullets[i])
					{
						if (!bullet->alive)
						{
							bullet->rbState.position = plane[i]->rbState.position;
							bullet->alive = true;
							++cntBulletsReleased;
							if (cntBulletsReleased == 1)
							{
								bullet->rbState.velocity = eae6320::Math::sVector(0.174f, sign * 0.985f, 0.0f) * speedMagnitude;
							}
							else if (cntBulletsReleased == 2)
							{
								bullet->rbState.velocity = eae6320::Math::sVector(-0.174f, sign * 0.985f, 0.0f) * speedMagnitude;
							}
							else if (cntBulletsReleased == 3)
							{
								bullet->rbState.velocity = eae6320::Math::sVector(0.0f, sign * 1.0f, 0.0f) * speedMagnitude;
								break;
							}
						}
					}
				}
				planeStopFiringRemainTime[i] = firingTypeCD[planeFiringType[i]];
			}
		}
		else
		{
			planeStopFiringRemainTime[i] -= i_elapsedSecondCount_sinceLastUpdate;
		}
	}
	
	// generate enemies
	if (StopGeneratingEnemyRemainTime <= 0.0f)
	{
		int cntEnemeyGenerated = 0;
		for (auto& enemy : enemies)
		{
			if (!enemy->alive)
			{
				enemy->alive = true;
				int cntSpawnY = 6;
				srand(static_cast<unsigned int>(time(0)));
				int randIndex = rand() % cntSpawnY;
				float boundary = 1.8f;
				float interval = 2*boundary / (float)(cntSpawnY);
				float spawnX = 2.5f;
				float speedX = 0.2f;
				if (++cntEnemeyGenerated < 2)
				{
					enemy->rbState.position = eae6320::Math::sVector(-spawnX, boundary - (cntSpawnY - randIndex) * interval, 0.0f);
					enemy->rbState.velocity = eae6320::Math::sVector(speedX, 0.0f, 0.0f);
				}
				else
				{
					enemy->rbState.position = eae6320::Math::sVector(spawnX, boundary - randIndex * interval, 0.0f);
					enemy->rbState.velocity = eae6320::Math::sVector(-speedX, 0.0f, 0.0f);
					break;
				}
			}
		}
		StopGeneratingEnemyRemainTime = generateEnemyCD;
	}
	else
	{
		StopGeneratingEnemyRemainTime -= i_elapsedSecondCount_sinceLastUpdate;
	}

	counter = (counter + 1) % 999999;
	unsigned int interval = 2;
	if (!hasSynced && counter % interval == 0)
		Networking::MyNetworkClient::Sync(NetClientID + std::string("F0"));

}

void eae6320::MyFinalGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	auto result = Results::Success;

	// Submit bg color to Graphics Project
	eae6320::Graphics::SubmitBackgroundColor(0.14f, 0.04f, 0.54f, 1.0f);

	// Submit mesh-effect pairs to Graphics Project
	std::vector<eae6320::Graphics::MyMesh*> FrameMeshes;
	std::vector<eae6320::Graphics::MyEffect*> FrameEffects;

	// Planes
	for (size_t i = 0; i < 2; i++)
	{
		//for (auto& pM : plane[i]->meshes)
		//	FrameMeshes.push_back(pM);
		//for (auto& pE : plane[i]->effects)
		//	FrameEffects.push_back(pE);
		FrameMeshes.push_back(plane[i]->meshes[0]);
		int effectIndex = (0.0f < plane[i]->hurtingRemainTime) ? 1 : 0;
		if (plane[i]->health <= 0)
			effectIndex = 1;
		FrameEffects.push_back(plane[i]->effects[effectIndex]);
	}
	// Enemies
	for (const auto& enemy : enemies)
	{
		for (auto& pM : enemy->meshes)
			FrameMeshes.push_back(pM);
		for (auto& pE : enemy->effects)
			FrameEffects.push_back(pE);
	}
	// Plane_bullets
	for (size_t i = 0; i < 2; i++)
	{
		for (const auto& bullet : plane_bullets[i])
		{
			for (auto& pM : bullet->meshes)
				FrameMeshes.push_back(pM);
			for (auto& pE : bullet->effects)
				FrameEffects.push_back(pE);
		}
	}
	// Plane Power Bar
	for (int i = 0; i < 2; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			if (planeFiringType[i] < k)
				break;
			FrameMeshes.push_back(planePowerBar[i]->meshes[k]);
			FrameEffects.push_back(planePowerBar[i]->effects[k]);
		}
	}
	// Plane Health Bar
	for (int i = 0; i < 2; i++)
	{
		int indexMesh = plane[i]->health - 1;
		if (indexMesh == -1)
			continue;
		if (indexMesh < 0 || 99 < indexMesh)
			return;
		FrameMeshes.push_back(planeHealthBar[i]->meshes[indexMesh]);
		FrameEffects.push_back(planeHealthBar[i]->effects[0]);
	}

	for (auto& p : FrameMeshes)
		eae6320::Graphics::TryIncrementReferenceCount(p);
	for (auto& p : FrameEffects)
		eae6320::Graphics::TryIncrementReferenceCount(p);

	eae6320::Graphics::SubmitMeshEffectPair(FrameMeshes, FrameEffects);

	for(auto& p : FrameMeshes)
		eae6320::Graphics::TryDecrementReferenceCount(p);
	for (auto& p : FrameEffects)
		eae6320::Graphics::TryDecrementReferenceCount(p);
	
	// Submit camera transform to Graphics Project
	eae6320::Graphics::SubmitCameraData(camera.rbState.position.z);
}