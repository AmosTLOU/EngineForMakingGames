/*
	A Mesh is a representation of geometry task
	In charge of initializing & draw & cleaning up
*/

#ifndef EAE6320_GRAPHICS_MESH_H
#define EAE6320_GRAPHICS_MESH_H

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
		class MyMesh
		{
		public:
			static eae6320::cResult InitializeGeometry();
#ifdef EAE6320_PLATFORM_GL
			static void Draw();
#endif
#ifdef EAE6320_PLATFORM_D3D
			static void Draw(ID3D11DeviceContext* direct3dImmediateContext);
#endif
			static void CleanUp(eae6320::cResult& result);
			
		private:
			// Geometry Data
			//--------------
#ifdef EAE6320_PLATFORM_GL
			// A vertex buffer holds the data for each vertex
			static GLuint s_vertexBufferId;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			static GLuint s_vertexArrayId;
#endif
#ifdef EAE6320_PLATFORM_D3D
			static eae6320::Graphics::cVertexFormat* s_vertexFormat;
			// A vertex buffer holds the data for each vertex
			static ID3D11Buffer* s_vertexBuffer;
#endif
		};
	}
}


#endif	// EAE6320_GRAPHICS_MESH_H
