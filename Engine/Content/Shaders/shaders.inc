/*
	This file should be #included by all shaders
*/

// Version Information
//====================

#if defined( EAE6320_PLATFORM_GL )

	// GLSL shaders require the version to be #defined before anything else in the shader
	#version 420

#endif

// Constant Buffers
//=================

#if defined( EAE6320_PLATFORM_D3D )

	/*
		general
	*/
	#define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
	#define mat4 float4x4
	#define vec2 float2
	#define vec4 float4
	
	/*
		Vertex Shader
	*/
	#define DECLARE_VERTEX_INPUT_VARIABLES /* nothing */
	// Input:
	//		The "semantics" (the keywords in all caps after the colon) are arbitrary, but must match the C call to CreateInputLayout()
	// 		These values come from one of the VertexFormats::sVertex_mesh that the vertex buffer was filled with in C code
	// Output:
	// 		An SV_POSITION value must always be output from every vertex shader
	// 		so that the GPU can figure out which fragments need to be shaded
	#define VertexShader_MainFunctionHead void main( in const float3 i_vertexPosition_local : POSITION, out float4 o_vertexPosition_projected : SV_POSITION )
	#define OutputVertexPosition o_vertexPosition_projected
	
	/*
		Fragment Shader
	*/
	#define DECLARE_FRAGMENT_INPUT_VARIABLES /* nothing */
	// Whatever color value is output from the fragment shader will determine the color of the corresponding pixel on the screen
	#define FragmentShader_MainFunctionHead void main( in const float4 i_fragmentPosition : SV_POSITION, out float4 o_color : SV_TARGET )

#elif defined( EAE6320_PLATFORM_GL )
	
	/*
		general
	*/
	#define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
	#define float4x4 mat4
	#define float2 vec2
	#define float4 vec4
	
	/*
		Vertex Shader
	*/
	// The locations assigned are arbitrary, but must match the C calls to glVertexAttribPointer()
	// These values come from one of the VertexFormats::sVertex_mesh that the vertex buffer was filled with in C code
	#define DECLARE_VERTEX_INPUT_VARIABLES layout( location = 0 ) in vec3 i_vertexPosition_local;
	#define VertexShader_MainFunctionHead void main()
	#define mul(a, b) a * b
	// The vertex shader must always output a position value,
	// but unlike HLSL where the value is explicit
	// GLSL has an automatically-required variable named "gl_Position"
	#define OutputVertexPosition gl_Position
	
	/*
		Fragment Shader
	*/
	// Whatever color value is output from the fragment shader will determine the color of the corresponding pixel on the screen
	#define DECLARE_FRAGMENT_INPUT_VARIABLES out vec4 o_color;
	#define FragmentShader_MainFunctionHead void main()
	
#endif


// Using GL Syntax by default
DeclareConstantBuffer( g_constantBuffer_frame, 0 )
{
	mat4 g_transform_worldToCamera;
	mat4 g_transform_cameraToProjected;

	float g_elapsedSecondCount_systemTime;
	float g_elapsedSecondCount_simulationTime;
	// For vec4 alignment
	vec2 g_padding;
};





