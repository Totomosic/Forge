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

	FORGE_API enum class ProjectionType
	{
		Perspective,
		Orthographic,
	};

	struct FORGE_API Frustum
	{
	public:
		glm::mat4 ProjectionMatrix;
		ProjectionType Type;
		float NearPlane;
		float FarPlane;
		float Left;
		float Right;
		float Bottom;
		float Top;

	public:
		inline static Frustum Perspective(float fovy, float aspect, float nearPlane, float farPlane)
		{
			float height = tan(fovy) * nearPlane * 2.0f;
			float width = aspect * height;

			Frustum frustum;
			frustum.ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
			frustum.Type = ProjectionType::Perspective;
			frustum.NearPlane = nearPlane;
			frustum.FarPlane = farPlane;
			frustum.Left = -width / 2.0f;
			frustum.Right = width / 2.0f;
			frustum.Bottom = -width / 2.0f;
			frustum.Top = width / 2.0f;
			return frustum;
		}

		inline static Frustum Perspective(float left, float right, float bottom, float top, float nearPlane, float farPlane)
		{
			float fovy = atan2f((top - bottom) / 2.0f, nearPlane) * 2.0f;
			float aspect = (right - left) / (top - bottom);

			Frustum frustum;
			frustum.ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
			frustum.Type = ProjectionType::Perspective;
			frustum.NearPlane = nearPlane;
			frustum.FarPlane = farPlane;
			frustum.Left = left;
			frustum.Right = right;
			frustum.Bottom = bottom;
			frustum.Top = top;
			return frustum;
		}

		inline static Frustum Orthographic(float left, float right, float bottom, float top, float nearPlane = -1.0f, float farPlane = 1.0f)
		{
			Frustum frustum;
			frustum.ProjectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
			frustum.Type = ProjectionType::Orthographic;
			frustum.NearPlane = nearPlane;
			frustum.FarPlane = farPlane;
			frustum.Left = left;
			frustum.Right = right;
			frustum.Bottom = bottom;
			frustum.Top = top;
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
