/*
	This class builds Meshs
*/

#ifndef EAE6320_CMeshBUILDER_H
#define EAE6320_CMeshBUILDER_H

// Includes
//=========

#include <Tools/AssetBuildLibrary/iBuilder.h>

//#include <Engine/Graphics/Configuration.h>
//#include <Engine/Graphics/cMesh.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cMeshBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build( const std::vector<std::string>& i_arguments ) final;

			// Implementation
			//===============

		private:
		};
	}
}

#endif	// EAE6320_CMeshBUILDER_H
