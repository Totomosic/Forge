#pragma once
#include "CameraData.h"
#include "Shader.h"
#include "Lighting.h"
#include "Texture.h"
#include "MaterialUniforms.h"

namespace Forge
{

	constexpr const char ProjectionMatrixUniformName[] = "frg_ProjectionMatrix";
	constexpr const char ViewMatrixUniformName[] = "frg_ViewMatrix";
	constexpr const char ProjViewMatrixUniformName[] = "frg_ProjViewMatrix";
	constexpr const char ModelMatrixUniformName[] = "frg_ModelMatrix";
	constexpr const char CameraFarPlaneUniformName[] = "frg_FarPlane";
	constexpr const char CameraNearPlaneUniformName[] = "frg_NearPlane";
	constexpr const char CameraPositionUniformName[] = "frg_CameraPosition";

	constexpr const char LightSourceArrayBase[] = "frg_LightSources";
	constexpr const char LightSourceArrayUniformName[] = "frg_LightSources[0].Position";
	constexpr const char UsedLightSourcesUniformName[] = "frg_UsedLightSources";

	constexpr const char JointTransformsBase[] = "frg_JointTransforms";
	constexpr const char JointTransformsUniformName[] = "frg_JointTransforms[0]";

	constexpr const char ClippingPlanesArrayBase[] = "frg_ClippingPlanes";
	constexpr const char ClippingPlanesArrayUniformName[] = "frg_ClippingPlanes[0]";
	constexpr const char UsedClippingPlanesUniformName[] = "frg_UsedClippingPlanes";

	constexpr const char PointShadowMatricesArrayBase[] = "frg_PointShadowMatrices";
	constexpr const char PointShadowMatricesArrayUniformName0[] = "frg_PointShadowMatrices[0]";
	constexpr const char PointShadowMatricesArrayUniformName1[] = "frg_PointShadowMatrices[1]";
	constexpr const char PointShadowMatricesArrayUniformName2[] = "frg_PointShadowMatrices[2]";
	constexpr const char PointShadowMatricesArrayUniformName3[] = "frg_PointShadowMatrices[3]";
	constexpr const char PointShadowMatricesArrayUniformName4[] = "frg_PointShadowMatrices[4]";
	constexpr const char PointShadowMatricesArrayUniformName5[] = "frg_PointShadowMatrices[5]";
	constexpr const char ShadowFormationLightPositionUniformName[] = "frg_ShadowLightPosition";

	constexpr const char TimeUniformName[] = "frg_Time";

	constexpr const char EntityIdUniformName[] = "frg_EntityID";

	struct FORGE_API ShaderRequirements
	{
	public:
		bool ProjectionMatrix;
		bool ViewMatrix;
		bool ProjViewMatrix;
		bool ModelMatrix;
		bool CameraFarPlane;
		bool CameraNearPlane;
		bool CameraPosition;

		bool LightSources;
		bool Animation;
		bool ClippingPlanes;

		bool ShadowFormationLightPosition;
		bool PointShadowMatrices;

		bool Time;
	};

	class FORGE_API RendererContext
	{
	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;
		float m_CameraFarPlane;
		float m_CameraNearPlane;
		glm::vec3 m_CameraPosition;
		float m_Time;
		glm::mat4 m_ShadowPointMatrices[6];
		glm::vec3 m_CurrentShadowLightPosition;
		glm::mat4 m_ShadowLightSpaceTransform;

		std::vector<LightSource> m_LightSources;
		std::vector<glm::vec4> m_ClippingPlanes;
		int m_NextTextureSlot;

		bool m_CullingEnabled;
		RenderSettings m_RenderSettings;

		std::unordered_map<const Shader*, ShaderRequirements> m_RequirementsMap;
		Ref<Shader> m_CurrentShader;

	public:
		RendererContext();

		inline void SetCameraPosition(const glm::vec3& position) { m_CameraPosition = position; }
		void ApplyRenderSettings(const RenderSettings& settings);
		void SetCamera(const CameraData& camera);
		void SetLightSources(const std::vector<LightSource>& lights);
		void AddLightSource(const LightSource& light);
		void SetClippingPlanes(const std::vector<glm::vec4>& planes);
		void SetTime(float time);
		void SetShadowPointMatrices(const glm::vec3& lightPosition, const glm::mat4 matrices[6]);
		void NewScene();
		void Reset();

		ShaderRequirements GetShaderRequirements(const Ref<Shader>& shader);
		void BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements);
		int BindTexture(const Ref<Texture>& texture);
	};

}
