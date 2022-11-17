// Includes
//=========

#include "cMeshBuilder.h"
#include "ReadNestedTableValues.h"
#include <Engine/Platform/Platform.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Time/Time.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <string>
#include <iostream>
#include <fstream>

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cMeshBuilder::Build( const std::vector<std::string>& i_arguments )
{
	/*
		ReadNestedTableValues will deal with .mesh files(text file) and save the data to vertexData and indexData
		Then we write them(vertexData's size, each vertex data, indexData's size, each index data) to the .mesh_bin files(binary file) 
			in the same diretory
		Then we use eae6320::Platform::CopyFile to copy the .mesh_bin files from SourceDirectory(MyGame_\Content\Meshes) 
			to BuildDirectory(temp\x86\Debug\MyGame_\data\meshes(if the build config is x86 debug))
	*/

	auto result = Results::Success;
	
	// start timing
	eae6320::Time::Initialize();
	auto startTime = eae6320::Time::GetCurrentSystemTimeTickCount();

	// Read data by lua
	// the m_path_source will be "what we designated in AssetsToBuild.lua" with "certain path attached to the front of it".
	std::string path_meshDataFile = m_path_source;
	std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> vertexData;
	std::vector<uint16_t> indexData;
	ReadNestedTableValues(path_meshDataFile, vertexData, indexData);

	// end timing
	auto endTime = eae6320::Time::GetCurrentSystemTimeTickCount();
	std::cout << "Load the humam-readable file:\n";
	std::cout << eae6320::Time::ConvertTicksToSeconds(endTime - startTime) << "\n";

	std::string BinaryFilePath = m_path_source;
	std::string BinaryFilePostfix = "_bin";
	BinaryFilePath += BinaryFilePostfix;
	// Write binary data
	{		
		std::ofstream outfile(BinaryFilePath, std::ofstream::binary);

		// The number of vertices
		{
			long size = 1;
			uint16_t* buffer = new uint16_t[size];
			buffer[0] = static_cast<uint16_t>(vertexData.size());

			outfile.write(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(size) * sizeof(uint16_t));
			delete[] buffer;
		}
		// An array of vertex data
		{
			long size = static_cast<long>(3 * vertexData.size());
			float_t* buffer = new float_t[size];
			for (long i = 0; i < size; i++)
			{
				int j = i / 3;
				int md = i % 3;
				if (md == 0)
					buffer[i] = vertexData[j].x;
				else if(md == 1)
					buffer[i] = vertexData[j].y;
				else if (md == 2)
					buffer[i] = vertexData[j].z;
				else
					OutputErrorMessageWithFileInfo(BinaryFilePath.c_str(), "Writing binary data faild during An Array Of Vertex Data");
			}
			outfile.write(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(size) * sizeof(float_t));
			delete[] buffer;
		}
		// The number of indices
		{
			long size = 1;
			uint16_t* buffer = new uint16_t[size];
			buffer[0] = static_cast<uint16_t>(indexData.size());

			outfile.write(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(size) * sizeof(uint16_t));
			delete[] buffer;
		}
		// An array of index data
		{
			long size = static_cast<long>(indexData.size());
			uint16_t* buffer = new uint16_t[size];
			for (long i = 0; i < size; i++)
			{
				buffer[i] = indexData[i];
			}
			outfile.write(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(size) * sizeof(uint16_t));
			delete[] buffer;
		}
		// release dynamically-allocated memory
		outfile.close();
	}	

	std::string errorMessage = "";
	std::string PathBinaryTarget = m_path_target;
	PathBinaryTarget += BinaryFilePostfix;
	/* 
		what the following function does is to copy the exact content of the sourcefile(all of the bits) to the targetfile,
		it doesn't matter if the sourcefile and the targetfile share the same file extension name. For instance, 
		if the sourcefile is a png, the targetfile is a word, this function will still work. It just moves the data!
		It doesn't care about anything else like who is gonna parse it. 
	*/
	result = eae6320::Platform::CopyFile(BinaryFilePath.c_str(), PathBinaryTarget.c_str(), false, true, &errorMessage);
	if (result != Results::Success)
	{
		OutputErrorMessageWithFileInfo(BinaryFilePath.c_str(), errorMessage.c_str());
		result = Results::Failure;
		return result;
	}

	return result;
}
