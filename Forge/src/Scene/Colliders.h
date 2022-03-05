#pragma once
#include "Constants.h"
#include <glm/glm.hpp>

namespace Forge
{

    struct FORGE_API AabbColliderComponent
    {
    public:
        Forge::LayerMask LayerMask = FULL_LAYER_MASK;
        glm::vec3 Dimensions;
        glm::mat4 Transform = glm::mat4(1.0f);
    };

}
