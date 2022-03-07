#pragma once
#include <glm/glm.hpp>

namespace Forge
{
    namespace Math
    {

        bool DecomposeTransform(
          const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale);

        template<typename T>
        inline T Sign(T value)
        {
            return value > 0 ? T(1) : value < 0 ? T(-1) : T(0);
        }

    }
}
