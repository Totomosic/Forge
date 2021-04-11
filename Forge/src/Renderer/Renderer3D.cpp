#include "ForgePch.h"
#include "Renderer3D.h"
#include "RenderCommand.h"

#include "GraphicsCache.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Forge
{
	Renderer3D::Renderer3D()
		: m_CurrentScene(), m_CurrentRenderPass(), m_Context(), m_ClearedFramebuffers(), m_ShadowRenderTarget(nullptr)
	{
	}

	void Renderer3D::BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources, const Ref<Framebuffer>& shadowRenderTarget)
	{
		m_CurrentScene = {
			framebuffer,
			camera,
			lightSources
		};
		m_Renderables.clear();
		m_ShadowRenderTarget = shadowRenderTarget;
	}

	void Renderer3D::EndScene()
	{
		if (m_ShadowRenderTarget)
		{
			FORGE_ASSERT(m_CurrentScene.LightSources.size() > 0, "Cannot run shadow pass without light sources");
			SceneData shadowScene = {
				m_ShadowRenderTarget,
				CreateCameraFromLightSource(m_CurrentScene.LightSources[0]),
				std::vector<LightSource>{}
			};

			m_CurrentRenderPass = RenderPass::ShadowFormation;
			SetupScene(shadowScene);
			RenderAll();

			m_CurrentRenderPass = RenderPass::WithShadow;
			Ref<Texture> shadowMap = m_ShadowRenderTarget->GetTexture(ColorBuffer::Depth);
			m_ShadowRenderTarget = nullptr;

			SetupScene(m_CurrentScene);
			m_Context.GetUniforms().AddUniform("u_ShadowMap", shadowMap);
			m_Context.GetUniforms().AddUniform("u_LightSpaceTransform", shadowScene.Camera.Frustum.ProjectionMatrix * shadowScene.Camera.ViewMatrix);
			RenderAll();
		}
		else
		{
			m_CurrentRenderPass = RenderPass::WithoutShadow;
			SetupScene(m_CurrentScene);
			RenderAll();
		}
	}

	void Renderer3D::Flush()
	{
		m_ClearedFramebuffers.clear();
	}

	void Renderer3D::RenderModel(const Ref<Model>& model, const glm::mat4& transform, bool createsShadow)
	{
		m_Renderables.push_back({ model, transform, createsShadow });
	}

	void Renderer3D::SetupScene(const SceneData& data)
	{
		FORGE_ASSERT(data.RenderTarget != nullptr, "Invalid framebuffer");
		if (data.RenderTarget != m_CurrentFramebuffer)
		{
			data.RenderTarget->Bind();
			m_CurrentFramebuffer = data.RenderTarget;
		}
		if (data.RenderTarget == m_ShadowRenderTarget)
		{
			RenderCommand::ClearDepth();
		}
		else if (m_ClearedFramebuffers.find(data.RenderTarget.get()) == m_ClearedFramebuffers.end())
		{
			RenderCommand::SetClearColor(data.Camera.ClearColor);
			RenderCommand::Clear();
			m_ClearedFramebuffers.insert(data.RenderTarget.get());
		}
		else if (data.Camera.Mode == CameraMode::Overlay)
		{
			RenderCommand::ClearDepth();
		}
		m_Context.Reset();
		m_Context.SetCamera(data.Camera);
		m_Context.SetLightSources(data.LightSources);
		m_Context.SetClippingPlanes(data.Camera.ClippingPlanes);

		RenderCommand::EnableClippingPlanes(data.Camera.ClippingPlanes.size());
		RenderCommand::SetViewport(data.Camera.Viewport);
	}

	void Renderer3D::RenderAll()
	{
		for (const RenderData& data : m_Renderables)
			RenderModelInternal(data);
	}

	void Renderer3D::RenderModelInternal(const RenderData& data)
	{
		if (m_ShadowRenderTarget && !data.CreatesShadow)
			return;
		const Ref<Model>& model = data.Model;
		const glm::mat4& transform = data.Transform;
		for (const Model::SubModel& submodel : model->GetSubModels())
		{
			const Ref<Mesh>& mesh = submodel.Mesh;
			Ref<Material> material = submodel.Material;
			glm::mat4 overallTransform = transform * submodel.Transform;

			ShaderRequirements requirements = m_Context.GetShaderRequirements(material->GetShader(m_CurrentRenderPass));
			m_Context.BindShader(material->GetShader(m_CurrentRenderPass), requirements);
			if (requirements.ModelMatrix)
				material->GetShader(m_CurrentRenderPass)->SetUniform(ModelMatrixUniformName, overallTransform);
			material->Apply(m_CurrentRenderPass, m_Context);
			mesh->Apply(material->GetShader(m_CurrentRenderPass), requirements);

			RenderCommand::DrawIndexed(mesh->GetDrawMode(), mesh->GetVertices());
		}
	}

	CameraData Renderer3D::CreateCameraFromLightSource(const LightSource& light) const
	{
		CameraData camera;
		camera.Viewport = { 0, 0, m_ShadowRenderTarget->GetWidth(), m_ShadowRenderTarget->GetHeight() };
		camera.ViewMatrix = glm::lookAt(light.Position, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		camera.Frustum = Frustum::Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 50.0f);
		return camera;
	}

	void Renderer3D::GetCameraTransformsFromLightSource(const LightSource& light, glm::mat4 transforms[6])
	{
	}

}
