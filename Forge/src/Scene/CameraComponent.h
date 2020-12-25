#pragma once
#include "ForgePch.h"

#include <glm/glm.hpp>

namespace Forge
{

	struct FORGE_API CameraComponent
	{
	public:
		glm::mat4 ProjectionMatrix;

	public:
		CameraComponent() = default;
		inline CameraComponent(const glm::mat4& projection)
			: ProjectionMatrix(projection)
		{}

	};

}
