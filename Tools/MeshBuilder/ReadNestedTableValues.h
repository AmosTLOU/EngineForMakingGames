/*
	This example shows how to read nested table values
	(i.e. tables within)
*/
#include <Engine/Graphics/VertexFormats.h>
#include <vector>
#include <string>

// Forward Declarations
//=====================

namespace eae6320
{
	class cResult;
}

// Interface
//==========

eae6320::cResult ReadNestedTableValues(std::string path_str, std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh>& vertexData, std::vector<uint16_t>& indexData);
