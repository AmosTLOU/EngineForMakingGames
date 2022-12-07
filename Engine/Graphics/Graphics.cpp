// Includes
//=========

#include "Graphics.h"

#include "OpenGL/Includes.h"
#include "Direct3D/Includes.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "sContext.h"
#include "VertexFormats.h"
#include "cMesh.h"
#include "cEffect.h"
#include "cRenderContext.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <Engine/Math/cQuaternion.h>
#include <Engine/Math/Functions.h>
#include <new>
#include <utility>
#include <vector>

#define MESHEFFECTPAIR_LIMIT 1024

// Static Data
//============

namespace
{
	// Constant buffer object(s)
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_drawCall(eae6320::Graphics::ConstantBufferTypes::DrawCall);

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
		std::vector<float> bg_color;
		// Mesh and Effect
		eae6320::Graphics::MyMesh* meshes[MESHEFFECTPAIR_LIMIT];
		eae6320::Graphics::MyEffect* effects[MESHEFFECTPAIR_LIMIT];
		size_t sz_mesheffectPair;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;
}



// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SubmitBackgroundColor(float r, float g, float b, float a)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& bg_color = s_dataBeingSubmittedByApplicationThread->bg_color;
	if (bg_color.size() != 4)
		bg_color.resize(4);
	bg_color[0] = r;
	bg_color[1] = g;
	bg_color[2] = b;
	bg_color[3] = a;
}

void eae6320::Graphics::SubmitMeshEffectPair(std::vector<MyMesh*> i_meshes, std::vector<MyEffect*> i_effects)
{	
	size_t sz1 = i_meshes.size();
	size_t sz2 = i_effects.size();
	if (sz1 != sz2)
	{
		EAE6320_ASSERTF(false, "The mesh array and effect array submitted by Game Project is not the same size!");
		Logging::OutputError("Failed to submit mesh-effect pairs from Game to Graphics.");
	}
	if (MESHEFFECTPAIR_LIMIT < sz1)
	{
#ifdef _DEBUG
		EAE6320_ASSERTF(false, "The Game Project tries to submit more data than is budgeted!");
		Logging::OutputError("Failed to submit all mesh-effect pairs from Game project because it is more than the budget amount.");
		return;
#else
		sz1 = MESHEFFECTPAIR_LIMIT;
#endif
	}

	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& sz_mesheffectPair = s_dataBeingSubmittedByApplicationThread->sz_mesheffectPair;
	auto& meshes = s_dataBeingSubmittedByApplicationThread->meshes;
	auto& effects = s_dataBeingSubmittedByApplicationThread->effects;

	sz_mesheffectPair = sz1;
	for (size_t i = 0; i < sz_mesheffectPair; i++)
	{
		meshes[i] = i_meshes[i];
		TryIncrementReferenceCount(meshes[i]);
		effects[i] = i_effects[i];
		TryIncrementReferenceCount(effects[i]);
	}
}

void eae6320::Graphics::SubmitCameraData(float pos_x)
{
	auto& g_transform_worldToCamera = s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_worldToCamera;
	g_transform_worldToCamera = Math::cMatrix_transformation::CreateWorldToCameraTransform(
		Math::cQuaternion(0.0f, eae6320::Math::sVector(0.0f, 0.0f, 1.0f)),
		Math::sVector(0, 0, pos_x));

	auto& g_transform_cameraToProjected = s_dataBeingSubmittedByApplicationThread->constantData_frame.g_transform_cameraToProjected;
	g_transform_cameraToProjected = Math::cMatrix_transformation::CreateCameraToProjectedTransform_perspective(
		Math::ConvertDegreesToRadians(45.0f), 1.0f, 0.1f, 100.0f
	);
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);

	RenderContext::ClearBackgroundColor(s_dataBeingRenderedByRenderThread->bg_color);

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = s_dataBeingRenderedByRenderThread->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}
	
	// Bind the shading data & Draw the geometry
	auto& sz_mesheffectPair = s_dataBeingSubmittedByApplicationThread->sz_mesheffectPair;
	auto& effects = s_dataBeingRenderedByRenderThread->effects;
	auto& meshes = s_dataBeingRenderedByRenderThread->meshes;

	for (size_t i = 0; i < sz_mesheffectPair; i++)
	{		
		effects[i]->Bind();

		// Update the draw call constant buffer
		{
			// Copy the data from the system memory that the application owns to GPU memory
			auto& constantData_frame = s_dataBeingRenderedByRenderThread->constantData_frame;
			s_constantBuffer_drawCall.Update(&(meshes[i]->constantData_drawCall));
		}

		meshes[i]->Draw();
	}
	RenderContext::SwapBuffer();

	// After all of the data that was submitted for this frame has been used
	// you must make sure that it is all cleaned up and cleared out
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		for (size_t i = 0; i < sz_mesheffectPair; i++)
		{
			TryDecrementReferenceCount(meshes[i]);
			TryDecrementReferenceCount(effects[i]);
		}		
	}
}


// Initialization / Clean Up
//--------------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
		if (result = s_constantBuffer_drawCall.Initialize())
		{
			s_constantBuffer_drawCall.Bind( static_cast<uint_fast8_t>(eShaderType::Vertex) );
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without draw call constant buffer");
			return result;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}
	// Initialize the views
	{
		if (!(result = RenderContext::InitializeViews(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the views");
			return result;
		}
	}
	Logging::OutputMessage("Graphics Project finished Initialization");
	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	Logging::OutputMessage("CCCCCC");
	auto result = Results::Success;

	RenderContext::CleanUpViews();

	// Application Thread
	{
		auto& sz_mesheffectPair = s_dataBeingSubmittedByApplicationThread->sz_mesheffectPair;
		auto& effects = s_dataBeingSubmittedByApplicationThread->effects;
		auto& meshes = s_dataBeingSubmittedByApplicationThread->meshes;
		for (size_t i = 0; i < sz_mesheffectPair; i++)
		{
			TryDecrementReferenceCount(meshes[i]);
			TryDecrementReferenceCount(effects[i]);
		}
	}
	// Render Thread
	{
		auto& sz_mesheffectPair = s_dataBeingRenderedByRenderThread->sz_mesheffectPair;
		auto& effects = s_dataBeingRenderedByRenderThread->effects;
		auto& meshes = s_dataBeingRenderedByRenderThread->meshes;
		for (size_t i = 0; i < sz_mesheffectPair; i++)
		{
			TryDecrementReferenceCount(meshes[i]);
			TryDecrementReferenceCount(effects[i]);
		}
	}

	// Clean up the platform-independent graphics objects
	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
		const auto result_constantBuffer_drawCall = s_constantBuffer_drawCall.CleanUp();
		if (!result_constantBuffer_drawCall)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_drawCall;
			}
		}
	}

	// Clean up the platform-specific context
	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}
	Logging::OutputMessage("Graphics Project finished CleanUp");
	return result;
}

eae6320::cResult eae6320::Graphics::TryDecrementReferenceCount(eae6320::Graphics::MyMesh*& p)
{
	auto result = Results::Success;
	if (!p)
	{
		result = Results::Failure;
		return result;
	}		
	p->DecrementReferenceCount();
	return result;
}


eae6320::cResult eae6320::Graphics::TryDecrementReferenceCount(eae6320::Graphics::MyEffect*& p)
{
	auto result = Results::Success;
	if (!p)
	{
		result = Results::Failure;
		return result;
	}
	p->DecrementReferenceCount();
	return result;
}

eae6320::cResult eae6320::Graphics::TryIncrementReferenceCount(eae6320::Graphics::MyMesh*& p)
{
	auto result = Results::Success;
	if (!p)
	{
		result = Results::Failure;
		return result;
	}
	p->IncrementReferenceCount();
	return result;
}


eae6320::cResult eae6320::Graphics::TryIncrementReferenceCount(eae6320::Graphics::MyEffect*& p)
{
	auto result = Results::Success;
	if (!p)
	{
		result = Results::Failure;
		return result;
	}
	p->IncrementReferenceCount();
	return result;
}
