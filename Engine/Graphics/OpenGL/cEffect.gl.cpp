#include "../cEffect.h"

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

	auto programId_copy = programId;
	// Create a program
	eae6320::cScopeGuard scopeGuard_program([&result, &programId_copy]
		{
			if (!result)
			{
				if (programId_copy != 0)
				{
					glDeleteProgram(programId_copy);
					const auto errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
						eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
							reinterpret_cast<const char*>(gluErrorString(errorCode)));
					}
					programId_copy = 0;
				}
			}
		});
	programId = programId_copy;
	{
		EAE6320_ASSERT(programId == 0);
		programId = glCreateProgram();
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to create a program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
		else if (programId == 0)
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("OpenGL failed to create a program");
			return result;
		}
	}
	// Attach the shaders to the program
	{
		// Vertex
		{
			EAE6320_ASSERT((vertexShader != nullptr) && (vertexShader->m_shaderId != 0));
			glAttachShader(programId, vertexShader->m_shaderId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to attach the vertex shader to the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
		// Fragment
		{
			EAE6320_ASSERT((fragmentShader != nullptr) && (fragmentShader->m_shaderId != 0));
			glAttachShader(programId, fragmentShader->m_shaderId);
			const auto errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				result = eae6320::Results::Failure;
				EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
				eae6320::Logging::OutputError("OpenGL failed to attach the fragment shader to the program: %s",
					reinterpret_cast<const char*>(gluErrorString(errorCode)));
				return result;
			}
		}
	}
	// Link the program
	{
		glLinkProgram(programId);
		const auto errorCode = glGetError();
		if (errorCode == GL_NO_ERROR)
		{
			// Get link info
			// (this won't be used unless linking fails
			// but it can be useful to look at when debugging)
			std::string linkInfo;
			{
				GLint infoSize;
				glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoSize);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (infoSize > 0)
					{
						auto* const info = new (std::nothrow) GLchar[infoSize];
						if (info)
						{
							eae6320::cScopeGuard scopeGuard_info([info]
								{
									delete[] info;
								});
							constexpr GLsizei* const dontReturnLength = nullptr;
							glGetProgramInfoLog(programId, static_cast<GLsizei>(infoSize), dontReturnLength, info);
							const auto errorCode = glGetError();
							if (errorCode == GL_NO_ERROR)
							{
								linkInfo = info;
							}
							else
							{
								result = eae6320::Results::Failure;
								EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
								eae6320::Logging::OutputError("OpenGL failed to get link info of the program: %s",
									reinterpret_cast<const char*>(gluErrorString(errorCode)));
								return result;
							}
						}
						else
						{
							result = eae6320::Results::OutOfMemory;
							EAE6320_ASSERTF(false, "Couldn't allocate memory for the program link info");
							eae6320::Logging::OutputError("Failed to allocate memory for the program link info");
							return result;
						}
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to get the length of the program link info: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
			// Check to see if there were link errors
			GLint didLinkingSucceed;
			{
				glGetProgramiv(programId, GL_LINK_STATUS, &didLinkingSucceed);
				const auto errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					if (didLinkingSucceed == GL_FALSE)
					{
						result = eae6320::Results::Failure;
						EAE6320_ASSERTF(false, linkInfo.c_str());
						eae6320::Logging::OutputError("The program failed to link: %s",
							linkInfo.c_str());
						return result;
					}
				}
				else
				{
					result = eae6320::Results::Failure;
					EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
					eae6320::Logging::OutputError("OpenGL failed to find out if linking of the program succeeded: %s",
						reinterpret_cast<const char*>(gluErrorString(errorCode)));
					return result;
				}
			}
		}
		else
		{
			result = eae6320::Results::Failure;
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to link the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
			return result;
		}
	}
	return result;
}


void eae6320::Graphics::MyEffect::Bind()
{
	{
		EAE6320_ASSERT(programId != 0);
		glUseProgram(programId);
		EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
	}
	// Render state
	{
		renderState.Bind();
	}
}


eae6320::cResult eae6320::Graphics::MyEffect::CleanUp()
{
	auto result = Results::Success;
	if (programId != 0)
	{
		glDeleteProgram(programId);
		const auto errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			if (result)
			{
				result = eae6320::Results::Failure;
			}
			EAE6320_ASSERTF(false, reinterpret_cast<const char*>(gluErrorString(errorCode)));
			eae6320::Logging::OutputError("OpenGL failed to delete the program: %s",
				reinterpret_cast<const char*>(gluErrorString(errorCode)));
		}
		programId = 0;
	}
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