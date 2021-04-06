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
		RendererContext m_Context;
		Ref<Framebuffer> m_CurrentFramebuffer = nullptr;
		std::unordered_set<const Framebuffer*> m_ClearedFramebuffers;

	public:
		void BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources = {});
		void EndScene();
		void Flush();

		void RenderModel(const Ref<Model>& model, const glm::mat4& transform);

	};

}
