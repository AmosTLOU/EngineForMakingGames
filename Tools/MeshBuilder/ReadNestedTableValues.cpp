// Includes
//=========

#include "ReadNestedTableValues.h"

#include <Engine/Graphics/VertexFormats.h>
//#include <Engine/Asserts/Asserts.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Results/Results.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <iostream>
#include <vector>
#include <string>

// Helper Function Declarations
//=============================

namespace
{
	eae6320::cResult LoadTableValues( lua_State& io_luaState );

	eae6320::cResult LoadTableValues_vertexData( lua_State& io_luaState );
	eae6320::cResult LoadTableValues_vertexData_iterate(lua_State& io_luaState);
	eae6320::cResult LoadTableValues_vertexData_specificData(lua_State& io_luaState);
	eae6320::cResult LoadTableValues_vertexData_specificData_position(lua_State& io_luaState);
	eae6320::cResult LoadTableValues_float3( lua_State& io_luaState, int valueType);

	eae6320::cResult LoadTableValues_indexData( lua_State& io_luaState );
	eae6320::cResult LoadTableValues_indexData_values( lua_State& io_luaState );

	eae6320::cResult LoadAsset( const char* const i_path );

	std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh> s_vertexData;
	std::vector<uint16_t> s_indexData;
	const char* path_file;
}

// Interface
//==========

eae6320::cResult ReadNestedTableValues(std::string path_str, std::vector<eae6320::Graphics::VertexFormats::sVertex_mesh>& vertexData, std::vector<uint16_t>& indexData)
{
	auto result = eae6320::Results::Success;

	path_file = path_str.c_str();

	s_vertexData.clear();
	s_indexData.clear();
	//EAE6320_ASSERT(vertexData.size() == 0);
	if(vertexData.size() != 0)
		eae6320::Assets::OutputErrorMessageWithFileInfo(path_file, "vertexData.size() is not 0!");
	//EAE6320_ASSERT(indexData.size() == 0);
	if (indexData.size() != 0)
		eae6320::Assets::OutputErrorMessageWithFileInfo(path_file, "!");

	auto* const path = path_str.c_str();
	if ( !( result = LoadAsset( path ) ) )
	{
		eae6320::Assets::OutputErrorMessageWithFileInfo(path_file, "ReadNestedTableValues() can't load the assest!");
		return result;
	}

	for (const auto& item : s_vertexData)
	{
		vertexData.push_back(item);
	}
	for (const auto& item : s_indexData)
	{
		indexData.push_back(item);
	}

	return result;
}

// Helper Function Definitions
//============================

namespace
{
	eae6320::cResult LoadTableValues( lua_State& io_luaState )
	{
		auto result = eae6320::Results::Success;
		// vertex data
		if ( !( result = LoadTableValues_vertexData( io_luaState ) ) )
		{
			return result;
		}
		// index data
		if (!(result = LoadTableValues_indexData(io_luaState)))
		{
			return result;
		}
		return result;
	}

	eae6320::cResult LoadTableValues_vertexData( lua_State& io_luaState )
	{
		auto result = eae6320::Results::Success;

		constexpr auto* const key = "vertexData";
		lua_pushstring( &io_luaState, key );
		lua_gettable( &io_luaState, -2 );
		// now the vertex data table is on the top
		eae6320::cScopeGuard scopeGuard_popTextures( [&io_luaState]
			{
				lua_pop( &io_luaState, 1 );
			} );
		if ( lua_istable( &io_luaState, -1 ) )
		{
			if ( !( result = LoadTableValues_vertexData_iterate( io_luaState ) ) )
			{
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename( &io_luaState, -1 ) << ")" << std::endl;
			return result;
		}

		return result;
	}

	eae6320::cResult LoadTableValues_vertexData_iterate( lua_State& io_luaState )
	{
		auto result = eae6320::Results::Success;

		std::cout << "Iterating through every element in vertexData" << std::endl;
		const auto sz = luaL_len( &io_luaState, -1 );
		for ( int i = 1; i <= sz; ++i )
		{
			lua_pushinteger( &io_luaState, i );
			lua_gettable( &io_luaState, -2 );
			// now the specific data table is on the top
			eae6320::cScopeGuard scopeGuard_popTexturePath( [&io_luaState]
				{
					lua_pop( &io_luaState, 1 );
				} );
			LoadTableValues_vertexData_specificData(io_luaState);
		}

		return result;
	}

	eae6320::cResult LoadTableValues_vertexData_specificData(lua_State& io_luaState)
	{
		auto result = eae6320::Results::Success;
		// position
		if (!(result = LoadTableValues_vertexData_specificData_position(io_luaState)))
		{
			return result;
		}
		// color
		
		// normal

		return result;
	}

	eae6320::cResult LoadTableValues_vertexData_specificData_position(lua_State& io_luaState)
	{
		auto result = eae6320::Results::Success;

		constexpr auto* const key = "position";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		// now the position table(the one has 3 float values) is on the top
		eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
			{
				lua_pop(&io_luaState, 1);
			});
		if (lua_istable(&io_luaState, -1))
		{
			if (!(result = LoadTableValues_float3(io_luaState, 0)))
			{
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")" << std::endl;
			return result;
		}

		return result;
	}

	eae6320::cResult LoadTableValues_float3(lua_State& io_luaState, int valueType)
	{
		auto result = eae6320::Results::Success;

		std::cout << "Iterating through every element(float) in the current table" << std::endl;
		std::vector<float> v_f3;
		const auto sz = luaL_len(&io_luaState, -1);
		for (int i = 1; i <= sz; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
			float floatVal = (float)lua_tonumber(&io_luaState, -1);
			// position
			if (valueType == 0)
			{
				v_f3.push_back(floatVal);
			}
			// color
			// normal
		}
		eae6320::Graphics::VertexFormats::sVertex_mesh vertexMesh;
		//EAE6320_ASSERT(v_f3.size() == 3);
		if (v_f3.size() != 3)
			eae6320::Assets::OutputErrorMessageWithFileInfo(path_file, "v_f3.size() is not 3!");
		vertexMesh.x = v_f3[0];
		vertexMesh.y = v_f3[1];
		vertexMesh.z = v_f3[2];
		s_vertexData.push_back(vertexMesh);
		return result;
	}

	eae6320::cResult LoadTableValues_indexData( lua_State& io_luaState )
	{
		auto result = eae6320::Results::Success;

		constexpr auto* const key = "indexData";
		lua_pushstring( &io_luaState, key );
		lua_gettable( &io_luaState, -2 );
		eae6320::cScopeGuard scopeGuard_popParameters( [&io_luaState]
			{
				lua_pop( &io_luaState, 1 );
			} );
		if ( lua_istable( &io_luaState, -1 ) )
		{
			if ( !( result = LoadTableValues_indexData_values( io_luaState ) ) )
			{
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			std::cerr << "The value at \"" << key << "\" must be a table "
				"(instead of a " << luaL_typename( &io_luaState, -1 ) << ")" << std::endl;
			return result;
		}

		return result;
	}

	eae6320::cResult LoadTableValues_indexData_values( lua_State& io_luaState )
	{
		auto result = eae6320::Results::Success;

		std::cout << "Iterating through the index:" << std::endl;
		const auto sz = luaL_len(&io_luaState, -1);
		for (int i = 1; i <= sz; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);
			eae6320::cScopeGuard scopeGuard_popTexturePath([&io_luaState]
				{
					lua_pop(&io_luaState, 1);
				});
			uint16_t indexOfVertex = (uint16_t)lua_tonumber(&io_luaState, -1);
			s_indexData.push_back(indexOfVertex);
		}

		return result;
	}

	eae6320::cResult LoadAsset( const char* const i_path )
	{
		auto result = eae6320::Results::Success;

		// Create a new Lua state
		lua_State* luaState = nullptr;
		eae6320::cScopeGuard scopeGuard_onExit( [&luaState]
			{
				if ( luaState )
				{
					// If I haven't made any mistakes
					// there shouldn't be anything on the stack
					// regardless of any errors
					//EAE6320_ASSERT( lua_gettop( luaState ) == 0 );
					if (lua_gettop(luaState) != 0)
						eae6320::Assets::OutputErrorMessageWithFileInfo(path_file, "There shouldn't be anything on the stack!");

					lua_close( luaState );
					luaState = nullptr;
				}
			} );
		{
			luaState = luaL_newstate();
			if ( !luaState )
			{
				result = eae6320::Results::OutOfMemory;
				std::cerr << "Failed to create a new Lua state" << std::endl;
				return result;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		const auto stackTopBeforeLoad = lua_gettop( luaState );
		{
			const auto luaResult = luaL_loadfile( luaState, i_path );
			if ( luaResult != LUA_OK )
			{
				result = eae6320::Results::Failure;
				std::cerr << lua_tostring( luaState, -1 ) << std::endl;
				// Pop the error message
				lua_pop( luaState, 1 );
				return result;
			}
		}
		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			constexpr int argumentCount = 0;
			constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			constexpr int noMessageHandler = 0;
			const auto luaResult = lua_pcall( luaState, argumentCount, returnValueCount, noMessageHandler );
			if ( luaResult == LUA_OK )
			{
				// A well-behaved asset file will only return a single value
				const auto returnedValueCount = lua_gettop( luaState ) - stackTopBeforeLoad;
				if ( returnedValueCount == 1 )
				{
					// A correct asset file _must_ return a table
					if ( !lua_istable( luaState, -1 ) )
					{
						result = eae6320::Results::InvalidFile;
						std::cerr << "Asset files must return a table (instead of a " <<
							luaL_typename( luaState, -1 ) << ")" << std::endl;
						// Pop the returned non-table value
						lua_pop( luaState, 1 );
						return result;
					}
				}
				else
				{
					result = eae6320::Results::InvalidFile;
					std::cerr << "Asset files must return a single table (instead of " <<
						returnedValueCount << " values)" << std::endl;
					// Pop every value that was returned
					lua_pop( luaState, returnedValueCount );
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				std::cerr << lua_tostring( luaState, -1 ) << std::endl;
				// Pop the error message
				lua_pop( luaState, 1 );
				return result;
			}
		}

		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1
		eae6320::cScopeGuard scopeGuard_popAssetTable( [luaState]
			{
				lua_pop( luaState, 1 );
			} );
		result = LoadTableValues( *luaState );

		return result;
	}
}
