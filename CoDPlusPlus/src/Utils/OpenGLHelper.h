#pragma once
#include <Structs/Engine/refdef.h>
#include <Structs/vec3_t.h>

#include <glm/mat4x4.hpp>

struct CameraMatrices
{
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
};

namespace OpenGLHelper
{
	void InjectDetours();

	float DotProduct(vec3_t v1, vec3_t v2);

	bool IsOnScreen(vec3_t vec, int width, int height);
	bool WorldToScreen(vec3_t src, vec3_t dst, vec3_t& screen, float fovx, float fovy, float windowWidth, float windowHeight, vec3_t left, vec3_t up, vec3_t forward);
	bool WorldToScreen(vec3_t dst, vec3_t& screen, refdef_t* refdef);
	CameraMatrices GetMatrices();
};