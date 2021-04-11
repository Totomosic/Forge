#pragma once
#include "RendererContext.h"
#include "Framebuffer.h"
#include "Model.h"

#include <unordered_set>

namespace Forge
{

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
			bool CreatesShadow;
		};

	private:
		SceneData m_CurrentScene;
		std::vector<RenderData> m_Renderables;
		RenderPass m_CurrentRenderPass;

		RendererContext m_Context;
		Ref<Framebuffer> m_CurrentFramebuffer = nullptr;
		std::unordered_set<const Framebuffer*> m_ClearedFramebuffers;

		Ref<Framebuffer> m_ShadowRenderTarget;

	public:
		Renderer3D();

		void BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources = {}, const Ref<Framebuffer>& shadowRenderTarget = nullptr);
		void EndScene();
		void Flush();

		void RenderModel(const Ref<Model>& model, const glm::mat4& transform, bool createsShadow = true);

	private:
		void SetupScene(const SceneData& data);
		void RenderAll();
		void RenderModelInternal(const RenderData& data);

		CameraData CreateCameraFromLightSource(const LightSource& light) const;
		void GetCameraTransformsFromLightSource(const LightSource& light, float aspect, const Frustum& frustum, glm::mat4 transforms[6]);

	};

}
