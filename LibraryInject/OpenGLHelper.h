#pragma once
#include "coduo_structs.h"

namespace OpenGLHelper
{
	void InjectDetours();

	float DotProduct(Vector3 v1, Vector3 v2);
	
	bool IsOnScreen(Vector3 vec, int width, int height);
	bool WorldToScreen(Vector3 src, Vector3 dst, Vector3& screen, float fovx, float fovy, float windowWidth, float windowHeight, Vector3 left, Vector3 up, Vector3 forward);
	bool WorldToScreen(Vector3 dst, Vector3& screen, refdef_t* refdef);
};