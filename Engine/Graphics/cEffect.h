/*
	An Effect is a representation of shading task
	In charge of initializing & binding & cleaning up
*/

#ifndef EAE6320_GRAPHICS_EFFECT_H
#define EAE6320_GRAPHICS_EFFECT_H

// Includes
//=========
#include "cRenderState.h"
#include "cShader.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Results/Results.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <new>
#include <string>

namespace eae6320
{
	namespace Graphics
	{
		class MyEffect
		{
		public:
			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS()
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(MyEffect)

				static eae6320::cResult FactoryMethod(MyEffect*& pEffect, const std::string vertexShaderPath, const std::string fragmentShaderPath)
			{
				auto result = Results::Success;
				// Allocate
				{
					pEffect = new MyEffect();
					if (!pEffect)
					{
						result = Results::OutOfMemory;
						EAE6320_ASSERTF(false, "Couldn't allocate memory for the effect");
						return result;
					}
				}
				// Initialize
				if (!(result = pEffect->InitializeShadingData(vertexShaderPath, fragmentShaderPath)))
				{
					EAE6320_ASSERTF(false, "Initialization of new effect failed");
					return result;
				}
				return result;
			}
			void Bind();

		private:
			MyEffect() { m_referenceCount = 1; }
			~MyEffect() { CleanUp(); }
			eae6320::cResult InitializeShadingData(const std::string vertexShaderPath, const std::string fragmentShaderPath);
			eae6320::cResult CleanUp();

		private:
			// Shading Data
			//-------------
			eae6320::Graphics::cRenderState renderState;
			eae6320::Graphics::cShader* vertexShader = nullptr;
			eae6320::Graphics::cShader* fragmentShader = nullptr;
#ifdef EAE6320_PLATFORM_GL		
			GLuint programId = 0;
#endif
			EAE6320_ASSETS_DECLAREREFERENCECOUNT()
		};
	}
}


#endif	// EAE6320_GRAPHICS_EFFECT_H
