/*
	An Effect is a representation of shading task
	In charge of initializing & binding & cleaning up
*/

#ifndef EAE6320_GRAPHICS_EFFECT_H
#define EAE6320_GRAPHICS_EFFECT_H

// Includes
//=========

#include "Configuration.h"
#include "Graphics.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "sContext.h"
#include "VertexFormats.h"
#include "cVertexFormat.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Results/Results.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <new>
#include <utility>

namespace eae6320
{
	namespace Graphics
	{
		class MyEffect
		{
		public:
			static eae6320::cResult InitializeShadingData();
#ifdef EAE6320_PLATFORM_GL
			static void Bind();
#endif
#ifdef EAE6320_PLATFORM_D3D
			static void Bind(ID3D11DeviceContext* direct3dImmediateContext);
#endif			
			static void CleanUp(eae6320::cResult& result);
			
		private:
			// Shading Data
			//-------------
#ifdef EAE6320_PLATFORM_GL
			static eae6320::Graphics::cShader* s_vertexShader;
			static eae6320::Graphics::cShader* s_fragmentShader;
			static GLuint s_programId;
			static eae6320::Graphics::cRenderState s_renderState;
#endif
#ifdef EAE6320_PLATFORM_D3D
			static eae6320::Graphics::cShader* s_vertexShader;
			static eae6320::Graphics::cShader* s_fragmentShader;
			static eae6320::Graphics::cRenderState s_renderState;
#endif
		};
	}
}


#endif	// EAE6320_GRAPHICS_EFFECT_H
