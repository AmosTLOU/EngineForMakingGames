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

#include "cMyGame.h"

// Inherited Implementation
//=========================

// Run
//----

void eae6320::MyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

// Initialize / Clean Up
//----------------------
void eae6320::MyGame::InitGraphicsContent()
{
	auto result = Results::Success;

	/**
		Object No.1
	*/
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
		// Initialize the geometry
		// Mesh No.1
		{
			eae6320::Graphics::MyMesh* pMesh = nullptr;
			if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/obj_1_mesh_1.mesh_bin")))
			{
				EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
				return;
			}
			meshes.push_back(pMesh);
		}
		// Mesh No.2
		{
			eae6320::Graphics::MyMesh* pMesh = nullptr;
			if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/obj_1_mesh_2.mesh_bin")))
			{
				EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
				return;
			}
			meshes.push_back(pMesh);
		}
		objects.push_back(new RenderableObject(meshes, effects));
	}
	/**
		Object No.2
	*/
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
		objects.push_back(new RenderableObject(meshes, effects));
	}
	/**
		Object No.3
	*/
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
			if (!(result = eae6320::Graphics::MyMesh::FactoryMethod(pMesh, "data/meshes/MayaExportObject.mesh_bin")))
			{
				EAE6320_ASSERTF(false, "Can't create a new mesh using the FactoryMethod");
				return;
			}
			meshes.push_back(pMesh);
		}
		objects.push_back(new RenderableObject(meshes, effects));
	}
}

eae6320::cResult eae6320::MyGame::Initialize()
{
	Networking::MyNetworkClient::StartClient();
	InitGraphicsContent();	
	return Results::Success;
}


void eae6320::MyGame::CleanUpGraphicsContent()
{
	for (auto& pObj : objects)
	{
		for (auto& pM : pObj->meshes)
			eae6320::Graphics::TryDecrementReferenceCount(pM);
		for (auto& pE : pObj->effects)
			eae6320::Graphics::TryDecrementReferenceCount(pE);
	}	
}


eae6320::cResult eae6320::MyGame::CleanUp()
{
	Networking::MyNetworkClient::EndClient();
	CleanUpGraphicsContent();
	return Results::Success;
}

void eae6320::MyGame::UpdateSimulationBasedOnInput()
{
	bool hasSynced = false;

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space) || Networking::MyNetworkClient::RecvMsg == "Space")
	{
		myGameState = MyGameState::removeOneObject;
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space) && !hasSynced)
		{
			Networking::MyNetworkClient::Sync("Space");
			hasSynced = true;
		}	
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::F1))
	{		
		myGameState = MyGameState::changeObjectsMesh;
	}
	else if (UserInput::IsKeyPressed(UserInput::KeyCodes::F2))
	{
		myGameState = MyGameState::changeOneEffect;
	}
	else
	{
		myGameState = MyGameState::default;
	}

	// Change chracter's velocity based on input
	{
		auto character = GetMainObject();
		//if (!character)
		//	return;
		EAE6320_ASSERT(character);
		float acc = 0.2f;
		float maxMagnitude = 2.0f;
		auto& v_x = character->rbState.velocity.x;
		auto& v_y = character->rbState.velocity.y;
		auto& v_z = character->rbState.velocity.z;
		
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left) || Networking::MyNetworkClient::RecvMsg == "Left")
		{
			v_x += -acc;
			v_x = v_x < -maxMagnitude ? -maxMagnitude : v_x;
			if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left) && !hasSynced)
			{
				Networking::MyNetworkClient::Sync("Left");
				hasSynced = true;
			}
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right) || Networking::MyNetworkClient::RecvMsg == "Right")
		{
			v_x += acc;
			v_x = maxMagnitude < v_x ? maxMagnitude : v_x;
			if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right) && !hasSynced)
			{
				Networking::MyNetworkClient::Sync("Right");
				hasSynced = true;
			}
		}
		else
		{
			v_x = 0;
		}
		if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down) || Networking::MyNetworkClient::RecvMsg == "Down")
		{
			v_y += -acc;
			v_y = v_y < -maxMagnitude ? -maxMagnitude : v_y;
			if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down) && !hasSynced)
			{
				Networking::MyNetworkClient::Sync("Down");
				hasSynced = true;
			}
		}
		else if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up) || Networking::MyNetworkClient::RecvMsg == "Up")
		{
			v_y += acc;
			v_y = v_y < -maxMagnitude ? -maxMagnitude : v_y;
			if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up) && !hasSynced)
			{
				Networking::MyNetworkClient::Sync("Up");
				hasSynced = true;
			}
		}
		else
		{
			v_y = 0;
		}
	}
	
	// Change camera's velocity based on input
	{
		auto& vel_z = camera.rbState.velocity.z;
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
		}
	}

	// 注意不能一帧sync两次或者sync太频繁，会出问题！
	counter = (counter + 1) % 999999;
	unsigned int interval = 2;
	if(!hasSynced && counter % interval == 0)
		Networking::MyNetworkClient::Sync("1");
}

void eae6320::MyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate)
{
	// Update chracter's position based on velocity
	{
		auto character = GetMainObject();
		//if (!character)
		//	return;
		EAE6320_ASSERT(character);
		character->rbState.position = character->rbState.PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate);
		for (auto& pMesh : character->meshes)
		{
			pMesh->constantData_drawCall.g_transform_localToWorld = eae6320::Math::cMatrix_transformation(
				eae6320::Math::cQuaternion(),
				eae6320::Math::sVector(character->rbState.position.x, character->rbState.position.y, character->rbState.position.z)
				);
		}		
	}
	// Update camera's position based on velocity
	{
		camera.rbState.position = camera.rbState.PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate);
	}
}

void eae6320::MyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	auto result = Results::Success;

	// Submit bg color to Graphics Project
	eae6320::Graphics::SubmitBackgroundColor(0.5f, 0.0f, 0.5f, 1.0f);

	// Submit mesh-effect pairs to Graphics Project
	std::vector<eae6320::Graphics::MyMesh*> FrameMeshes;
	std::vector<eae6320::Graphics::MyEffect*> FrameEffects;

	EAE6320_ASSERT(2 < objects.size());

	// static object. Index = 1, 2
	for (auto& pM : objects[1]->meshes)
		FrameMeshes.push_back(pM);
	for (auto& pE : objects[1]->effects)
		FrameEffects.push_back(pE);
	for (auto& pM : objects[2]->meshes)
		FrameMeshes.push_back(pM);
	for (auto& pE : objects[2]->effects)
		FrameEffects.push_back(pE);

	// deal with dynamic object(main character)
	if (myGameState == MyGameState::default)
	{
		auto pObj = GetMainObject();
		FrameMeshes.push_back(pObj->meshes[0]);
		FrameEffects.push_back(pObj->effects[0]);
	}
	else if (myGameState == MyGameState::changeObjectsMesh)
	{
		auto pObj = GetMainObject();
		FrameMeshes.push_back(pObj->meshes[1]);
		FrameEffects.push_back(pObj->effects[0]);
	}
	else if (myGameState == MyGameState::removeOneObject)
	{
	}
	else if (myGameState == MyGameState::changeOneEffect)
	{
		auto pObj = GetMainObject();
		FrameMeshes.push_back(pObj->meshes[0]);
		FrameEffects.push_back(pObj->effects[1]);
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