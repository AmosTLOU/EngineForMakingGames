#include "../cEffect.h"
#include "../cRenderContext.h"
#include"Includes.h"


eae6320::cResult eae6320::Graphics::MyEffect::InitializeShadingData(const std::string vertexShaderPath, const std::string fragmentShaderPath)
{
	auto result = eae6320::Results::Success;

	if (!(result = eae6320::Graphics::cShader::Load(vertexShaderPath,
		vertexShader, eae6320::Graphics::eShaderType::Vertex)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without vertex shader");
		return result;
	}
	if (!(result = eae6320::Graphics::cShader::Load(fragmentShaderPath,
		fragmentShader, eae6320::Graphics::eShaderType::Fragment)))
	{
		EAE6320_ASSERTF(false, "Can't initialize shading data without fragment shader");
		return result;
	}
	{
		constexpr auto renderStateBits = []
		{
			uint8_t renderStateBits = 0;

			/*eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);*/
			eae6320::Graphics::RenderStates::EnableAlphaTransparency(renderStateBits);
			eae6320::Graphics::RenderStates::EnableDepthTesting(renderStateBits);
			eae6320::Graphics::RenderStates::EnableDepthWriting(renderStateBits);
			eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(renderStateBits);

			return renderStateBits;
		}();
		if (!(result = renderState.Initialize(renderStateBits)))
		{
			EAE6320_ASSERTF(false, "Can't initialize shading data without render state");
			return result;
		}
	}

	return result;
}


void eae6320::Graphics::MyEffect::Bind()
{
	{
		ID3D11DeviceContext* direct3dImmediateContext = eae6320::Graphics::RenderContext::direct3dImmediateContext;
		constexpr ID3D11ClassInstance* const* noInterfaces = nullptr;
		constexpr unsigned int interfaceCount = 0;
		// Vertex shader
		{
			EAE6320_ASSERT((vertexShader != nullptr) && (vertexShader->m_shaderObject.vertex != nullptr));
			direct3dImmediateContext->VSSetShader(vertexShader->m_shaderObject.vertex, noInterfaces, interfaceCount);
		}
		// Fragment shader
		{
			EAE6320_ASSERT((fragmentShader != nullptr) && (fragmentShader->m_shaderObject.vertex != nullptr));
			direct3dImmediateContext->PSSetShader(fragmentShader->m_shaderObject.fragment, noInterfaces, interfaceCount);
		}
	}
	// Render state
	{
		renderState.Bind();
	}
}


eae6320::cResult eae6320::Graphics::MyEffect::CleanUp()
{
	auto result = Results::Success;
	if (vertexShader)
	{
		vertexShader->DecrementReferenceCount();
		vertexShader = nullptr;
	}
	if (fragmentShader)
	{
		fragmentShader->DecrementReferenceCount();
		fragmentShader = nullptr; 
	}
	return result;
}
