#pragma once
#include "RendererContext.h"
#include "Model.h"
#include "PostProcessor.h"

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
			bool UsePostProcessing = false;
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
			const LightSource* Light;
			Ref<Framebuffer> RenderTarget;
			int LightIndex;
		};

	private:
		RendererStats m_Stats;
		SceneData m_CurrentScene;
		std::vector<ShadowPass> m_ShadowPasses;
		std::vector<RenderData> m_Renderables;
		bool m_RenderImGui;
		RenderPass m_CurrentRenderPass;
		int m_CurrentShadowLightIndex;

		RendererContext m_Context;
		Ref<Framebuffer> m_CurrentFramebuffer = nullptr;
		std::unordered_set<const Framebuffer*> m_ClearedFramebuffers;

		PostProcessor m_PostProcessor;

	public:
		Renderer3D();

		inline const PostProcessor& GetPostProcessor() const { return m_PostProcessor; }
		inline PostProcessor& GetPostProcessor() { return m_PostProcessor; }
		inline const RendererStats& GetStats() const { return m_Stats; }
		void SetTime(float time);

		void BeginPickScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera);
		void BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources = {});
		void EndScene();
		void Flush();

		void RenderModel(const Ref<Model>& model, const glm::mat4& transform, const RenderOptions& options = {});
		inline void RenderImGui() { m_RenderImGui = true; }

	private:
		void AddShadowPass(const Ref<Framebuffer>& framebuffer, const LightSource& light, int index);
		void RenderShadowScene(const ShadowPass& pass);
		void SetupScene(const SceneData& data);
		void RenderAll();
		void RenderImGuiInternal();
		void RenderModelInternal(const RenderData& data);

		CameraData CreateCameraFromLightSource(const glm::vec3& lightPosition, const glm::vec3& lightDirection, const Ref<Framebuffer>& renderTarget, const Frustum& frustum) const;
		void GetCameraTransformsFromLightSource(const glm::vec3& lightPosition, float aspect, const Frustum& frustum, glm::mat4 transforms[6]);

	};

}
