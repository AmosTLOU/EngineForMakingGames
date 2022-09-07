#include "../cEffect.h"
#include"Includes.h"

eae6320::Graphics::cShader* eae6320::Graphics::MyEffect::s_vertexShader = nullptr;
eae6320::Graphics::cShader* eae6320::Graphics::MyEffect::s_fragmentShader = nullptr;
eae6320::Graphics::cRenderState eae6320::Graphics::MyEffect::s_renderState;

eae6320::cResult eae6320::Graphics::MyEffect::InitializeShadingData()
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load("data/Shaders/Vertex/standard.shader",
		s_vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::Load("data/Shaders/Fragment/animatedcolor.shader",
		s_fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);

			return renderStateBits;
		}();
		if (!(result = s_renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}

	return result;
}


void eae6320::Graphics::MyEffect::Bind(ID3D11DeviceContext* direct3dImmediateContext)
{
	{
		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		// Vertex shader
		{
			EAE6320_ASSERT((s_vertexShader != nullptr) && (s_vertexShader->m_shaderObject.vertex != nullptr));
			direct3dImmediateContext->VSSetShader(s_vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
		}
		// Fragment shader
		{
			EAE6320_ASSERT((s_fragmentShader != nullptr) && (s_fragmentShader->m_shaderObject.vertex != nullptr));
			direct3dImmediateContext->PSSetShader(s_fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
		}
	}
	// Render state
	{
		s_renderState.Bind();
	}
}


void eae6320::Graphics::MyEffect::CleanUp(eae6320::cResult& result)
{
	if (s_vertexShader)
	{
		s_vertexShader->DecrementReferenceCount();
		s_vertexShader = nullptr;
	}
	if (s_fragmentShader)
	{
		s_fragmentShader->DecrementReferenceCount();
		s_fragmentShader = nullptr;
	}
}
