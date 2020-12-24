#pragma once
#include "RendererContext.h"
#include "Model.h"

namespace Forge
{

	class FORGE_API Renderer3D
	{
	private:
		RendererContext m_Context;

	public:
		void BeginScene(const CameraData& camera);
		void EndScene();
		void Flush();

		void RenderModel(const Ref<Model>& model, const glm::mat4& transform);

	};

}
