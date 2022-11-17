/*
	A vertex shader is responsible for two things:
		* Telling the GPU where the vertex (one of the three in a triangle) should be drawn on screen in a given window
			* The GPU will use this to decide which fragments (i.e. pixels) need to be shaded for a given triangle
		* Providing any data that a corresponding fragment shader will need
			* This data will be interpolated across the triangle and thus vary for each fragment of a triangle that gets shaded
*/

#include <Shaders/shaders.inc>

// Constant Buffers(Frame has been declared in .inc file, DrawCall is declared below)
//=================

DeclareConstantBuffer( g_constantBuffer_drawCall, 2 )
{
  mat4 g_transform_localToWorld;
};

// Input
//======

DECLARE_VERTEX_INPUT_VARIABLES

// Entry Point
//============

// Using D3D Syntax by default
VertexShader_MainFunctionHead
{
	// Transform the local vertex into world space
	float4 vertexPosition_world;
	{
		// Transform the vertex from local space into world space
		float4 vertexPosition_local = float4( i_vertexPosition_local, 1.0 );
		vertexPosition_world = mul( g_transform_localToWorld, vertexPosition_local );
	}
	// Calculate the position of this vertex projected onto the display
	{
		// Transform the vertex from world space into camera space
		float4 vertexPosition_camera = mul( g_transform_worldToCamera, vertexPosition_world );
		// Project the vertex from camera space into projected space
		OutputVertexPosition = mul( g_transform_cameraToProjected, vertexPosition_camera );
	}
}