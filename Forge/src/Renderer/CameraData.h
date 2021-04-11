#pragma once
#include "Logging.h"
#include "Core/Color.h"
#include "Core/Viewport.h"

#include <glm/glm.hpp>

namespace Forge
{

	enum class CameraMode
	{
		Normal,
		Overlay,
	};

	struct FORGE_API CameraData
	{
	public:
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewMatrix;
		Forge::Viewport Viewport;
		std::vector<glm::vec4> ClippingPlanes;
		Color ClearColor;
		CameraMode Mode = CameraMode::Normal;
	};

}
