#pragma once
#include "CameraData.h"
#include "Shader.h"
#include "Lighting.h"
#include "Texture.h"
#include "MaterialUniforms.h"
#include "UniformBuffer.h"
#include "ShaderLibrary.h"

namespace Forge
{

	constexpr uint32_t CameraDataBindingPoint = 0;
	constexpr uint32_t ShadowFormationDataBindingPoint = 1;
	constexpr uint32_t ClippingPlaneDataBindingPoint = 2;
	constexpr uint32_t LightingDataBindingPoint = 3;

	struct FORGE_API UniformCameraData
	{
	public:
		alignas(16) glm::mat4 ProjectionMatrix;
		alignas(16) glm::mat4 ViewMatrix;
		alignas(16) glm::mat4 ProjViewMatrix;
		alignas( 4) float CameraFarPlane;
		alignas( 4) float CameraNearPlane;
		alignas(16) glm::vec3 CameraPosition;
	};

	struct FORGE_API UniformShadowFormationData
	{
	public:
		alignas(16) glm::mat4 PointShadowMatrices[6];
		alignas(16) glm::vec3 LightPosition;
	};

	struct FORGE_API UniformClippingPlaneData
	{
	public:
		alignas(16) glm::vec4 ClippingPlanes[MAX_CLIPPING_PLANES];
		alignas( 4) int UsedClippingPlanes;
	};

	struct FORGE_API UniformLightSourceData
	{
	public:
		alignas( 4) int Type;
		alignas(16) glm::vec3 Position;
		alignas(16) glm::vec3 Direction;
		alignas( 4) float Ambient;
		alignas(16) glm::vec4 Color;
		alignas(16) glm::vec3 Attenuation;
		alignas( 4) float Intensity;
		alignas( 4) bool UseShadows;
		alignas( 4) float ShadowNear;
		alignas( 4) float ShadowFar;
		alignas(16) glm::mat4 LightSpaceTransform;
	};

	struct FORGE_API UniformLightingData
	{
	public:
		alignas(16) UniformLightSourceData LightSources[MAX_LIGHT_COUNT];
		alignas( 4) int UsedLightCount;
	};

	constexpr const char ModelMatrixUniformName[] = "frg_ModelMatrix";

	constexpr const char LightSourceShadowMapArrayBase[] = "frg_LightShadowMaps";
	constexpr const char LightSourceShadowMapArrayUniformName[] = "frg_LightShadowMaps[0].ShadowMap";

	constexpr const char JointTransformsBase[] = "frg_JointTransforms";
	constexpr const char JointTransformsUniformName[] = "frg_JointTransforms[0]";

	constexpr const char TimeUniformName[] = "frg_Time";
	constexpr const char EntityIdUniformName[] = "frg_EntityID";

	struct FORGE_API ShaderRequirements
	{
	public:
		bool ModelMatrix;

		bool LightSourceShadowMaps;
		bool Animation;

		bool Time;
	};

	class FORGE_API RendererContext
	{
	private:
		static constexpr int MaxTextureSlots = 32;
		static constexpr int FirstTextureSlot = 2;
		static constexpr int NullTexture2DSlot = 0;
		static constexpr int NullTextureCubeSlot = 1;

		struct FORGE_API LightShadowBinding
		{
		public:
			int Location;
			GLenum Type;
		};

	private:
		Ref<UniformBuffer> m_CameraUniformBuffer;
		Ref<UniformBuffer> m_ShadowFormationUniformBuffer;
		Ref<UniformBuffer> m_ClippingPlaneUniformBuffer;
		Ref<UniformBuffer> m_LightingUniformBuffer;

		bool m_BoundSlots[FirstTextureSlot];
		int m_NextTextureSlot;
		int m_NextSceneTextureSlot;

		float m_Time;
		std::vector<LightShadowBinding> m_LightSourceShadowBindings;

		bool m_CullingEnabled;
		RenderSettings m_RenderSettings;

		std::unordered_map<const Shader*, ShaderRequirements> m_RequirementsMap;
		Ref<Shader> m_CurrentShader;

	public:
		RendererContext();

		inline int GetAvailableTextureSlots() const { return MaxTextureSlots - m_NextTextureSlot; }

		void ApplyRenderSettings(const RenderSettings& settings);
		void SetCamera(const CameraData& camera);
		void SetLightSources(const std::vector<LightSource>& lights);
		void SetClippingPlanes(const std::vector<glm::vec4>& planes);
		void SetTime(float time);
		void SetShadowPointMatrices(const glm::vec3& lightPosition, const glm::mat4 matrices[6]);
		void NewScene();
		void NewDrawCall();
		void Reset();

		ShaderRequirements GetShaderRequirements(const Ref<Shader>& shader);
		void BindShader(const Ref<Shader>& shader, const ShaderRequirements& requirements);
		int BindTexture(const Ref<Texture>& texture, GLenum textureTarget, bool sceneWideTexture = false);
	};

}
