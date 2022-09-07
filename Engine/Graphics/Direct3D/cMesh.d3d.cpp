#include "../cMesh.h"
#include "Includes.h"

eae6320::Graphics::cVertexFormat* eae6320::Graphics::MyMesh::s_vertexFormat = nullptr;
ID3D11Buffer* eae6320::Graphics::MyMesh::s_vertexBuffer = nullptr;

eae6320::cResult eae6320::Graphics::MyMesh::InitializeGeometry()
{
	auto result = eae6320::Results::Success;

	auto* const direct3dDevice = eae6320::Graphics::sContext::g_context.direct3dDevice;
	EAE6320_ASSERT(direct3dDevice);

	// Vertex Format
	{
		if (!(result = eae6320::Graphics::cVertexFormat::Load(eae6320::Graphics::eVertexType::Mesh, s_vertexFormat,
			"data/Shaders/Vertex/vertexInputLayout_mesh.shader")))
		{
			EAE6320_ASSERTF(false, "Can't initialize geometry without vertex format");
			return result;
		}
	}
	// Vertex Buffer
	{
		constexpr unsigned int triangleCount = 2;
		constexpr unsigned int vertexCountPerTriangle = 3;
		constexpr auto vertexCount = triangleCount * vertexCountPerTriangle;
		eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[vertexCount];
		{
			// Direct3D is left-handed

			vertexData[0].x = 0.0f;
			vertexData[0].y = 0.0f;
			vertexData[0].z = 0.0f;

			vertexData[1].x = 1.0f;
			vertexData[1].y = 1.0f;
			vertexData[1].z = 0.0f;

			vertexData[2].x = 1.0f;
			vertexData[2].y = 0.0f;
			vertexData[2].z = 0.0f;

			vertexData[3].x = 0.0f;
			vertexData[3].y = 0.0f;
			vertexData[3].z = 0.0f;

			vertexData[4].x = 0.0f;
			vertexData[4].y = 1.0f;
			vertexData[4].z = 0.0f;

			vertexData[5].x = 1.0f;
			vertexData[5].y = 1.0f;
			vertexData[5].z = 0.0f;
		}
		constexpr auto bufferSize = sizeof(vertexData[0]) * vertexCount;
		EAE6320_ASSERT(bufferSize <= std::numeric_limits<decltype(D3D11_BUFFER_DESC::ByteWidth)>::max());
		constexpr auto bufferDescription = [bufferSize]
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

		const auto initialData = [vertexData]
		{
			D3D11_SUBRESOURCE_DATA initialData{};

			initialData.pSysMem = vertexData;
			// (The other data members are ignored for non-texture buffers)

			return initialData;
		}();

		const auto result_create = direct3dDevice->CreateBuffer(&bufferDescription, &initialData, &s_vertexBuffer);
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

void eae6320::Graphics::MyMesh::Draw(ID3D11DeviceContext* direct3dImmediateContext)
{
	// Draw the geometry
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			EAE6320_ASSERT(s_vertexBuffer != nullptr);
			constexpr unsigned int startingSlot = 0;
			constexpr unsigned int vertexBufferCount = 1;
			// The "stride" defines how large a single vertex is in the stream of data
			constexpr unsigned int bufferStride = sizeof(VertexFormats::sVertex_mesh);
			// It's possible to start streaming data in the middle of a vertex buffer
			constexpr unsigned int bufferOffset = 0;
			direct3dImmediateContext->IASetVertexBuffers(startingSlot, vertexBufferCount, &s_vertexBuffer, &bufferStride, &bufferOffset);
		}
		// Specify what kind of data the vertex buffer holds
		{
			// Bind the vertex format (which defines how to interpret a single vertex)
			{
				EAE6320_ASSERT(s_vertexFormat != nullptr);
				s_vertexFormat->Bind();
			}
			// Set the topology (which defines how to interpret multiple vertices as a single "primitive";
			// the vertex buffer was defined as a triangle list
			// (meaning that every primitive is a triangle and will be defined by three vertices)
			direct3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		// Render triangles from the currently-bound vertex buffer
		{
			// As of this comment only a single triangle is drawn
			// (you will have to update this code in future assignments!)
			constexpr unsigned int triangleCount = 2;
			constexpr unsigned int vertexCountPerTriangle = 3;
			constexpr auto vertexCountToRender = triangleCount * vertexCountPerTriangle;
			// It's possible to start rendering primitives in the middle of the stream
			constexpr unsigned int indexOfFirstVertexToRender = 0;
			direct3dImmediateContext->Draw(vertexCountToRender, indexOfFirstVertexToRender);
		}
	}
}

void eae6320::Graphics::MyMesh::CleanUp(eae6320::cResult& result)
{
	if (s_vertexBuffer)
	{
		s_vertexBuffer->Release();
		s_vertexBuffer = nullptr;
	}
	if (s_vertexFormat)
	{
		s_vertexFormat->DecrementReferenceCount();
		s_vertexFormat = nullptr;
	}
}