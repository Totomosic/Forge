#pragma once
#include "CameraData.h"
#include "Shader.h"
#include "Lighting.h"
#include "Texture.h"

namespace Forge
{

	constexpr const char ProjectionMatrixUniformName[] = "u_ProjectionMatrix";
	constexpr const char ViewMatrixUniformName[] = "u_ViewMatrix";
	constexpr const char ProjViewMatrixUniformName[] = "u_ProjViewMatrix";
	constexpr const char ModelMatrixUniformName[] = "u_ModelMatrix";

	constexpr const char LightSourceArrayBase[] = "u_LightSources";
	constexpr const char LightSourceArrayUniformName[] = "u_LightSources[0].Position";
	constexpr const char UsedLightSourcesUniformName[] = "u_UsedLightSources";

	constexpr const char JointTransformsBase[] = "u_JointTransforms";
	constexpr const char JointTransformsUniformName[] = "u_JointTransforms[0]";

	constexpr const char ClippingPlanesArrayBase[] = "u_ClippingPlanes";
	constexpr const char ClippingPlanesArrayUniformName[] = "u_ClippingPlanes[0]";
	constexpr const char UsedClippingPlanesUniformName[] = "u_UsedClippingPlanes";

	constexpr const char ShadowMapUniformName[] = "u_ShadowMap";
	constexpr const char LightSpaceTransformUniformName[] = "u_LightSpaceTransform";

	enum class RenderPass
	{
		ShadowFormation,
		WithShadow,
		WithoutShadow,
	};
	constexpr int RENDER_PASS_COUNT = 3;

	struct FORGE_API ShaderRequirements
	{
	public:
		bool ProjectionMatrix;
		bool ViewMatrix;
		bool ProjViewMatrix;
		bool ModelMatrix;

		bool LightSources;
		bool Animation;
		bool ClippingPlanes;

		bool ShadowMap;
	};

	class FORGE_API RendererContext
	{
	private:
		static constexpr int SHADOW_MAP_TEXTURE_SLOT = 0;

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ProjViewMatrix;
		
		std::vector<LightSource> m_LightSources;
		std::vector<glm::vec4> m_ClippingPlanes;
		int m_NextTextureSlot;
		Ref<Texture2D> m_ShadowMap;
		glm::mat4 m_LightSpaceTransform;

		std::unordered_map<const Shader*, ShaderRequirements> m_RequirementsMap;

	public:
		void SetShadowMap(const Ref<Texture2D>& shadowMap);
		inline void SetLightSpaceTransform(const glm::mat4& transform) { m_LightSpaceTransform = transform; }
		void SetCamera(const CameraData& camera);
		void SetLightSources(const std::vector<LightSource>& lights);
		void AddLightSource(const LightSource& light);
		void SetClippingPlanes(const std::vector<glm::vec4>& planes);
		void Reset();

		ShaderRequirements GetShaderRequirements(const Ref<Shader>& shader);
		void BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements);
		int BindTexture(const Ref<Texture>& texture);
	};

}
