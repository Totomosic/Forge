#pragma once
#include "ForgePch.h"
#include "Renderer/Framebuffer.h"

#include <glm/glm.hpp>

namespace Forge
{

	struct FORGE_API CameraComponent
	{
	public:
		glm::mat4 ProjectionMatrix;
		Ref<Framebuffer> RenderTarget = nullptr;
		std::vector<glm::vec4> ClippingPlanes = {};

	public:
		CameraComponent() = default;
		inline CameraComponent(const glm::mat4& projection)
			: ProjectionMatrix(projection)
		{}

	};

}
