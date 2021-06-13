#include "ForgePch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

namespace Forge
{
    namespace Math
    {

        bool Forge::Math::DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
        {
			glm::mat4 localMatrix(transform);

			// Normalize the matrix.
			if (glm::epsilonEqual(localMatrix[3][3], static_cast<float>(0), glm::epsilon<float>()))
				return false;

			// First, isolate perspective.  This is the messiest.
			if (
				glm::epsilonNotEqual(localMatrix[0][3], static_cast<float>(0), glm::epsilon<float>()) ||
				glm::epsilonNotEqual(localMatrix[1][3], static_cast<float>(0), glm::epsilon<float>()) ||
				glm::epsilonNotEqual(localMatrix[2][3], static_cast<float>(0), glm::epsilon<float>()))
			{
				// Clear the perspective partition
				localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = static_cast<float>(0);
				localMatrix[3][3] = static_cast<float>(1);
			}

			// Next take care of translation (easy).
			translation = glm::vec3(localMatrix[3]);
			localMatrix[3] = glm::vec4(0, 0, 0, localMatrix[3].w);

			glm::vec3 row[3];
			glm::vec3 Pdum3;

			// Now get scale and shear.
			for (glm::length_t i = 0; i < 3; ++i)
				for (glm::length_t j = 0; j < 3; ++j)
					row[i][j] = localMatrix[i][j];

			// Compute X scale factor and normalize first row.
			scale.x = glm::length(row[0]);
			if (glm::epsilonNotEqual(scale.x, 0.0f, glm::epsilon<float>()))
				row[0] = row[0] / scale.x;
			scale.y = glm::length(row[1]);
			if (glm::epsilonNotEqual(scale.y, 0.0f, glm::epsilon<float>()))
				row[1] = row[1] / scale.y;
			scale.z = glm::length(row[2]);
			if (glm::epsilonNotEqual(scale.z, 0.0f, glm::epsilon<float>()))
				row[2] = row[2] / scale.z;

			rotation = glm::quat(glm::mat3(row[0], row[1], row[2]));
			return true;
        }

    }
}
