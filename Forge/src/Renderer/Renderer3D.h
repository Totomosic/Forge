#pragma once
#include "RendererContext.h"
#include "Framebuffer.h"
#include "Model.h"

#include <unordered_set>

namespace Forge
{

	struct FORGE_API RendererStats
	{
	public:
		int DrawCount = 0;
	};

	struct FORGE_API RenderOptions
	{
	public:
		std::bitset<MAX_LIGHT_COUNT> ShadowMask;
		int EntityId;
	};

	class FORGE_API Renderer3D
	{
	private:
		struct SceneData
		{
			Ref<Framebuffer> RenderTarget;
			CameraData Camera;
			std::vector<LightSource> LightSources;
		};

		struct RenderData
		{
		public:
			Ref<Forge::Model> Model;
			glm::mat4 Transform;
			RenderOptions Options;
		};

		struct ShadowPass
		{
		public:
			Ref<Framebuffer> RenderTarget;
			glm::vec3 Position;
		};

	private:
		RendererStats m_Stats;
		SceneData m_CurrentScene;
		std::vector<ShadowPass> m_ShadowPasses;
		std::vector<RenderData> m_Renderables;
		bool m_RenderImGui;
		int m_CurrentShadowIndex;
		RenderPass m_CurrentRenderPass;

		RendererContext m_Context;
		Ref<Framebuffer> m_CurrentFramebuffer = nullptr;
		std::unordered_set<const Framebuffer*> m_ClearedFramebuffers;

	public:
		Renderer3D();

		inline const RendererStats& GetStats() const { return m_Stats; }
		void SetTime(float time);

		void AddShadowPass(const Ref<Framebuffer>& framebuffer, const glm::vec3& lightPosition);
		void BeginPickScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera);
		void BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources = {});
		void EndScene();
		void Flush();

		void RenderModel(const Ref<Model>& model, const glm::mat4& transform, const RenderOptions& options = {});
		inline void RenderImGui() { m_RenderImGui = true; }

	private:
		ShadowRenderData RenderShadowScene(const ShadowPass& pass);
		void SetupScene(const SceneData& data);
		void RenderAll();
		void RenderModelInternal(const RenderData& data);

		CameraData CreateCameraFromLightSource(const LightSource& light, const Ref<Framebuffer>& renderTarget, float range) const;
		void GetCameraTransformsFromLightSource(const glm::vec3& lightPosition, float aspect, const Frustum& frustum, glm::mat4 transforms[6]);

	};

}
