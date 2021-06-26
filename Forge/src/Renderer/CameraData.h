#pragma once
#include "Logging.h"
#include "Core/Color.h"
#include "Core/Viewport.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Forge
{

	FORGE_API enum class CameraMode
	{
		Normal,
		Overlay,
	};

	struct FORGE_API Frustum
	{
	public:
		glm::mat4 ProjectionMatrix;
		float NearPlane;
		float FarPlane;

	public:
		inline static Frustum Perspective(float fovy, float aspect, float nearPlane, float farPlane)
		{
			Frustum frustum;
			frustum.ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
			frustum.NearPlane = nearPlane;
			frustum.FarPlane = farPlane;
			return frustum;
		}

		inline static Frustum Orthographic(float left, float right, float bottom, float top, float nearPlane = -1.0f, float farPlane = 1.0f)
		{
			Frustum frustum;
			frustum.ProjectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
			frustum.NearPlane = nearPlane;
			frustum.FarPlane = farPlane;
			return frustum;
		}
	};

	struct FORGE_API CameraData
	{
	public:
		Forge::Frustum Frustum;
		glm::mat4 ViewMatrix;
		Forge::Viewport Viewport;
		std::vector<glm::vec4> ClippingPlanes;
		Color ClearColor;
		CameraMode Mode = CameraMode::Normal;
		bool UsePostProcessing = true;
	};

}
