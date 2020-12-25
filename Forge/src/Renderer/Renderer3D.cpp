#include "ForgePch.h"
#include "Renderer3D.h"
#include "RenderCommand.h"

namespace Forge
{

	void Renderer3D::BeginScene(const CameraData& camera, const std::vector<LightSource>& lightSources)
	{
		m_Context.Reset();
		m_Context.SetCamera(camera);
		m_Context.SetLightSources(lightSources);
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

			RenderCommand::DrawIndexed(mesh->GetVertices());
		}
	}

}
