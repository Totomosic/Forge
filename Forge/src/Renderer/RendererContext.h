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
	constexpr const char ShadowMapUniformName[] = "frg_ShadowMap";
	constexpr const char ShadowLightPositionUniformName[] = "frg_LightPosition";

	constexpr int RENDER_PASS_COUNT = 4;

	FORGE_API enum class RenderPass
	{
		ShadowFormation,
		WithShadow,
		WithoutShadow,
		Pick,
	};

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

		bool PointShadowMatrices;
		bool ShadowMap;
		bool ShadowLightPosition;
	};

	class FORGE_API RendererContext
	{
	private:
		static constexpr int SHADOW_MAP_TEXTURE_SLOT = 0;

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;
		float m_CameraFarPlane;
		float m_CameraNearPlane;
		glm::vec3 m_CameraPosition;

		std::vector<LightSource> m_LightSources;
		std::vector<glm::vec4> m_ClippingPlanes;
		int m_NextTextureSlot;

		// Shadows
		glm::mat4 m_PointShadowMatrices[6];
		glm::vec3 m_ShadowLightSourcePosition;
		Ref<Texture> m_ShadowMap;

		bool m_CullingEnabled;
		RenderSettings m_RenderSettings;

		std::unordered_map<const Shader*, ShaderRequirements> m_RequirementsMap;

	public:
		RendererContext();

		inline void SetCameraPosition(const glm::vec3& position) { m_CameraPosition = position; }
		void ApplyRenderSettings(const RenderSettings& settings);
		void SetCamera(const CameraData& camera);
		void SetLightSources(const std::vector<LightSource>& lights);
		void AddLightSource(const LightSource& light);
		void SetClippingPlanes(const std::vector<glm::vec4>& planes);
		void SetPointShadowMatrices(const glm::mat4* matrices);
		void SetShadowLightPosition(const glm::vec3& position);
		void SetShadowMap(const Ref<Texture>& shadowMap);
		void Reset();

		ShaderRequirements GetShaderRequirements(const Ref<Shader>& shader);
		void BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements);
		int BindTexture(const Ref<Texture>& texture);
	};

}
