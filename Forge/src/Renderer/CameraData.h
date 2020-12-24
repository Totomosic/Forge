#pragma once
#include "Logging.h"

#include <glm/glm.hpp>

namespace Forge
{

	struct FORGE_API CameraData
	{
	public:
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
	};

}
