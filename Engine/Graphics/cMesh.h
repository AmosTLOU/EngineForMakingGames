/*
	A Mesh is a representation of geometry task
	In charge of initializing & draw & cleaning up
*/

#ifndef EAE6320_GRAPHICS_MESH_H
#define EAE6320_GRAPHICS_MESH_H

// Includes
//=========
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "VertexFormats.h"
#include "cVertexFormat.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/Time/Time.h>

#include <new>
#include <utility>
#include <vector>
#include <string>
#include <iostream>

namespace eae6320
{
	namespace Graphics
	{
		class MyMesh
		{
		public:
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(MyMesh)

				static eae6320::cResult FactoryMethod(MyMesh*& pMesh, std::string path_meshDataFile)
			{

				auto result = Results::Success;
				// Allocate
				{
					pMesh = new MyMesh();
					if (!pMesh)
					{
						result = Results::OutOfMemory;
						EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
						return result;
					}
				}

				/*
				// Test Code
				 std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> vertexData;
				 std::vector<uint16_t> indexData;
				{
					vertexData.resize(3);
					vertexData[0].x = 0.0f;
					vertexData[0].y = 0.0f;
					vertexData[0].z = 0.0f;
					vertexData[1].x = 0.5f;
					vertexData[1].y = 0.0f;
					vertexData[1].z = 0.0f;
					vertexData[2].x = 0.0f;
					vertexData[2].y = 0.5f;
					vertexData[2].z = 0.0f;
					indexData.resize(3);
					indexData[0] = 0;
					indexData[1] = 1;
					indexData[2] = 2;
				}
				*/

				// start timing
				eae6320::Time::Initialize();
				auto startTime = eae6320::Time::GetCurrentSystemTimeTickCount();

				// Read mesh data from binary data file
				eae6320::Platform::sDataFromFile dataFromFile;
				std::string errorMessage = "";
				result = eae6320::Platform::LoadBinaryFile(path_meshDataFile.c_str(), dataFromFile, &errorMessage);
				if (!result)
				{
					EAE6320_ASSERTF(false, "MyMesh FactoryMethod: LoadBinaryFile failed!");
					return result;
				}
				auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
				const auto finalOffset = currentOffset + dataFromFile.size;
				// The number of vertices	
				/*uint16_t vertexCount;
				memcpy(&vertexCount, reinterpret_cast<void*>(currentOffset), sizeof(vertexCount));*/
				const auto vertexCount = *reinterpret_cast<uint16_t*>(currentOffset);
				// An array of vertex data
				currentOffset += sizeof(vertexCount);
				const auto* const vertexArray = reinterpret_cast<VertexFormats::sVertex_mesh*>(currentOffset);
				// The number of indices
				currentOffset += static_cast<uintptr_t>(vertexCount) * 3 * sizeof(float_t);
				const auto indexCount = *reinterpret_cast<uint16_t*>(currentOffset);
				// An array of index data
				currentOffset += sizeof(indexCount);
				const auto* const indexArray = reinterpret_cast<uint16_t*>(currentOffset);

				// end timing
				auto endTime = eae6320::Time::GetCurrentSystemTimeTickCount();
				/*std::cout << "Load the binary data file:\n";
				std::cout << eae6320::Time::ConvertTicksToSeconds(endTime - startTime) << "\n";*/
				Logging::OutputMessage( "How many seconds does \"Load the binary data file\" takes: %lf", eae6320::Time::ConvertTicksToSeconds(endTime - startTime) );

				// Initialize
				if (!(result = pMesh->InitializeGeometry(vertexArray, vertexCount, indexArray, indexCount)))
				{
					EAE6320_ASSERTF(false, "Initialization of new mesh failed");
					return result;
				}
				return result;
			}
			void Draw();

		private:
			MyMesh() { m_referenceCount = 1; }
			~MyMesh() { CleanUp(); }
			eae6320::cResult InitializeGeometry(std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> vertexData, 
				std::vector<uint16_t> indexData);
			eae6320::cResult InitializeGeometry(const VertexFormats::sVertex_mesh* const vertexArray, const uint16_t vertexCount,
				const uint16_t* const indexArray, const uint16_t indexCount);
			eae6320::cResult CleanUp();

		public:
			eae6320::Graphics::ConstantBufferFormats::sDrawCall constantData_drawCall;

		private:
			// Geometry Data
			//--------------
#ifdef EAE6320_PLATFORM_GL
			// A vertex buffer holds the data for each vertex
			GLuint vertexBufferId = 0;
			// A index buffer holds the data for each index
			GLuint indexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint vertexArrayId = 0;
			GLsizei indexCountToRender = 0;
#endif
#ifdef EAE6320_PLATFORM_D3D
			eae6320::Graphics::cVertexFormat* vertexFormat = nullptr;
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* vertexBuffer = nullptr;
			ID3D11Buffer* indexBuffer = nullptr;
			unsigned int indexCountToRender = 0;
#endif
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		};
	}
}


#endif	// EAE6320_GRAPHICS_MESH_H
