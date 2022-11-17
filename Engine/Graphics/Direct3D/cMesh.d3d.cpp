#include "../cMesh.h"
#include "../sContext.h"
#include "../cRenderContext.h"
#include "Includes.h"

#include<algorithm>

eae6320::cResult eae6320::Graphics::MyMesh::InitializeGeometry(std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> vertexData,
	std::vector<uint16_t> indexData)
{
	auto result = eae6320::Results::Success;
	if (vertexData.size() == 0 || indexData.size() == 0)
	{
		EAE6320_ASSERTF(false, "In MyMesh::InitializeGeometry(Direct3D) function, vertextData's size is 0! or indexData's size is 0!");
	}

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Vertex Format
	{
		if (!(result = eae6320::Graphics::cVertexFormat::Load(eae6320::Graphics::eVertexType::Mesh, vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
		{
			EAE6320_ASSERTF(false, "Can't initialize geometry without vertex format");
			return result;
		}
	}
	// Vertex Buffer
	{
		const auto vertexCount = vertexData.size();
		eae6320::Graphics::VertexFormats::sVertex_mesh* p_vertexData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vertexCount];
		for (size_t i = 0; i < vertexCount; i++)
		{
			p_vertexData[i] = vertexData[i];
		}
		const auto bufferSize = sizeof(p_vertexData[0]) * vertexCount;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		const auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [p_vertexData]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = p_vertexData;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &vertexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	// Index Buffer
	{
		indexCountToRender = static_cast<unsigned int>(indexData.size());
		uint16_t* p_indexData = new uint16_t[indexCountToRender];
		for (size_t i = 0; i < indexCountToRender; i++)
		{
			p_indexData[i] = indexData[i];
		}
		for (size_t i = 2; i < indexCountToRender; i += 3)
			std::swap(p_indexData[i], p_indexData[i - 1]);
		
		const auto bufferSize = sizeof(p_indexData[0]) * indexCountToRender;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		const auto bufferDescription = [bufferSize]
		{
			D3D11_BUFFER_DESC bufferDescription{};

			bufferDescription.ByteWidth = static_cast<unsigned int>(bufferSize);
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;	// In our class the buffer will never change after it's been created
			bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDescription.CPUAccessFlags = 0;	// No CPU access is necessary
			bufferDescription.MiscFlags = 0;
			bufferDescription.StructureByteStride = 0;	// Not used

			return bufferDescription;
		}();

		const auto initialData = [p_indexData]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = p_indexData;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &indexBuffer);
		if (FAILED(result_create))
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, "3D object vertex buffer creation failed (HRESULT %#010x)", result_create);
			eae6320::Logging::OutputError("Direct3D failed to create a 3D object vertex buffer (HRESULT %#010x)", result_create);
			return result;
		}
	}

	return result;
}


eae6320::cResult eae6320::Graphics::MyMesh::InitializeGeometry(const VertexFormats::sVertex_mesh* const vertexArray, const uint16_t vertexCount,
	const uint16_t* const indexArray, const uint16_t indexCount)
{
	std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> vertexData;
	std::vector<uint16_t> indexData;
	for (size_t i = 0; i < vertexCount; i++)
	{
		vertexData.push_back(vertexArray[i]);
	}
	for (size_t i = 0; i < indexCount; i++)
	{
		indexData.push_back(indexArray[i]);
	}
	return InitializeGeometry(vertexData, indexData);
}


void eae6320::Graphics::MyMesh::Draw()
{
	// Draw the geometry
	{
		ID3D11DeviceContext* direct3dImmediateContext = eae6320::Graphics::RenderContext::direct3dImmediateContext;
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(vertexBuffer != nullptr);
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			constexpr unsigned int bufferStride = sizeof(VertexFormats::sVertex_mesh);
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &vertexBuffer, &bufferStride, &bufferOffset);
		}
		// Bind a specific index buffer to the device as a data source
		{
			EAE6320_ASSERT(indexBuffer);
			constexpr DXGI_FORMAT indexFormat = DXGI_FORMAT_R16_UINT;
			// The indices start at the beginning of the buffer
			constexpr unsigned int offset = 0;
			direct3dImmediateContext->IASetIndexBuffer(indexBuffer, indexFormat, offset);
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Bind the vertex format (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT(vertexFormat != nullptr);
				vertexFormat->Bind();
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			//// As of this comment only a single triangle is drawn
			//// (you will have to update this code in future assignments!)
			//constexpr unsigned int triangleCount = 1;
			//constexpr unsigned int vertexCountPerTriangle = 3;
			//constexpr auto vertexCountToRender = triangleCount * vertexCountPerTriangle;
			//// It's possible to start rendering primitives in the middle of the stream
			//constexpr unsigned int indexCountToRender = 3;
			constexpr unsigned int indexOfFirstIndexToUse = 0;
			constexpr unsigned int offsetToAddToEachIndex = 0;
			direct3dImmediateContext->DrawIndexed(static_cast<unsigned int>(indexCountToRender), indexOfFirstIndexToUse, offsetToAddToEachIndex);
		}
	}
}

eae6320::cResult eae6320::Graphics::MyMesh::CleanUp()
{
	auto result = Results::Success;
	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = nullptr;
	}
	if (vertexFormat)
	{
		vertexFormat->DecrementReferenceCount();
		vertexFormat = nullptr;
	}
	return result;
}