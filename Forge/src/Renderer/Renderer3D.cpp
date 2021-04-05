#include "ForgePch.h"
#include "Renderer3D.h"
#include "RenderCommand.h"

namespace Forge
{

	void Renderer3D::BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources)
	{
		FORGE_ASSERT(framebuffer != nullptr, "Invalid framebuffer");
		if (framebuffer != m_CurrentFramebuffer || framebuffer->RequiresRebind())
		{
			framebuffer->Bind();
			RenderCommand::SetViewport(framebuffer->GetViewport());
			m_CurrentFramebuffer = framebuffer;
		}
		RenderCommand::Clear();
		m_Context.Reset();
		m_Context.SetCamera(camera);
		m_Context.SetLightSources(lightSources);
		m_Context.SetClippingPlanes(camera.ClippingPlanes);

		RenderCommand::EnableClippingPlanes(camera.ClippingPlanes.size());
	}

	void Renderer3D::EndScene()
	{
	}

	void Renderer3D::Flush()
	{
	}

	void Renderer3D::RenderModel(const Ref<Model>& model, const glm::mat4& transform)
	{
		for (const Model::SubModel& submodel : model->GetSubModels())
		{
			const Ref<Mesh>& mesh = submodel.Mesh;
			const Ref<Material>& material = submodel.Material;
			glm::mat4 overallTransform = transform * submodel.Transform;

			ShaderRequirements requirements = m_Context.GetShaderRequirements(material->GetShader());
			m_Context.BindShader(material->GetShader(), requirements);
			if (requirements.ModelMatrix)
				material->GetShader()->SetUniform(ModelMatrixUniformName, overallTransform);
			material->Apply(m_Context);
			mesh->Apply(material->GetShader(), requirements);

			RenderCommand::DrawIndexed(mesh->GetDrawMode(), mesh->GetVertices());
		}
	}

}
