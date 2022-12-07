/*
	A fragment shader is responsible for telling the GPU what color a specific fragment should be
*/

#include <Shaders/shaders.inc>

DECLARE_FRAGMENT_INPUT_VARIABLES

// Entry Point
//============

// Using GL Syntax by default
FragmentShader_MainFunctionHead
{
	float r = abs(sin( g_elapsedSecondCount_simulationTime ));
	float g = abs(cos( g_elapsedSecondCount_simulationTime ));
	float b = abs(1.0-sin( g_elapsedSecondCount_simulationTime ));
	o_color = vec4(
		// RGB (color)
		r, g, b,
		// Alpha (opacity)
		1.0 );
}