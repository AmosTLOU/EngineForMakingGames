#ifndef EAE6320_GRAPHICS_EXTRA_H
#define EAE6320_GRAPHICS_EXTRA_H

// Includes
//=========
#include "sContext.h"

#include <d3d11.h>

namespace eae6320
{
	namespace Graphics
	{
		class RenderContext
		{
		public:
			static ID3D11DeviceContext* direct3dImmediateContext;

			static eae6320::cResult InitializeViews(const sInitializationParameters& i_initializationParameters);
			static void ClearBackgroundColor(std::vector<float> bgClr);
			static void SwapBuffer();
			static void CleanUpViews();

		private:
#ifdef EAE6320_PLATFORM_D3D
			// In Direct3D "views" are objects that allow a texture to be used a particular way:
			// A render target view allows a texture to have color rendered to it
			static ID3D11RenderTargetView* s_renderTargetView;
			// A depth/stencil view allows a texture to have depth rendered to it
			static ID3D11DepthStencilView* s_depthStencilView;
#endif
		};
	}
}

#endif	// EAE6320_GRAPHICS_EXTRA_H
