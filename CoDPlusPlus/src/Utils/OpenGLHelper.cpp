#ifdef CLIENT

#include <Utils/OpenGLHelper.h>
#include <Hook/Hook.h>
#include <Hook/Detours.h>
#include <Structs/vec3_t.h>

#include <Engine/CoDUO.h>
#include <glm/gtc/matrix_transform.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

using namespace CoDUO;
using namespace Hook::Detour;

namespace OpenGLHelper
{
	void InjectDetours()
	{
        GLimp_EndFrameHook.Inject(0x004f66c0, hkGLimp_EndFrame);
	}

    float DotProduct(vec3_t v1, vec3_t v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    bool IsOnScreen(vec3_t vec, int width, int height)
    {
        return vec.x > 0 && vec.y > 0 && vec.x <= width && vec.y <= height;
    }

    bool WorldToScreen(vec3_t src, vec3_t dst, vec3_t& screen, float fovx, float fovy, float windowWidth, float windowHeight, vec3_t left, vec3_t up, vec3_t forward)
    {
        vec3_t transform;
        float xc, yc;
        float px, py;
        float z;

        px = tan(fovx * M_PI / 360.0f);
        py = tan(fovy * M_PI / 360.0f);

        transform = dst - src;

        xc = windowWidth / 2.0f;
        yc = windowHeight / 2.0f;

        z = DotProduct(transform, left);

        if (z <= 0.1)
        {
            return false;
        }

        screen.x = xc - DotProduct(transform, up) * xc / (z * px);
        screen.y = yc - DotProduct(transform, forward) * yc / (z * py);

        return true;
    }

    bool WorldToScreen(vec3_t dst, vec3_t& screen, refdef_t* refdef)
    {
        return WorldToScreen(refdef->vieworg, dst, screen, refdef->fov_x, refdef->fov_y, refdef->width, refdef->height, refdef->viewaxis[0], refdef->viewaxis[1], refdef->viewaxis[2]);
    }

    CameraMatrices GetMatrices()
    {
        glm::mat4 projection = glm::perspective(glm::radians(refdef->fov_y), (float)refdef->width / (float)refdef->height, 0.1f, 10000.0f);

        glm::vec3 forward(refdef->viewaxis[0]);
        glm::vec3 right(refdef->viewaxis[1]);
        glm::vec3 up(refdef->viewaxis[2]);

        glm::vec3 cameraPos(refdef->vieworg);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + forward, up);

        return CameraMatrices{ .Model = glm::mat4(1.0f), .View = view, .Projection = projection };
    }
}

#endif