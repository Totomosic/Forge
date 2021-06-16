#include "ForgePch.h"
#include "Renderer3D.h"
#include "RenderCommand.h"

#include "Assets/GraphicsCache.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>

namespace Forge
{

	Renderer3D::Renderer3D()
		: m_CurrentScene(), m_CurrentRenderPass(), m_RenderImGui(false), m_Context(), m_ClearedFramebuffers(), m_ShadowRenderTarget(nullptr)
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
		m_RenderImGui = false;
	}

	void Renderer3D::EndScene()
	{
		if (m_ShadowRenderTarget && m_CurrentScene.LightSources.size() > 0)
		{
			CameraData camera = m_CurrentScene.Camera;
			camera.Viewport = { 0, 0, m_ShadowRenderTarget->GetWidth(), m_ShadowRenderTarget->GetHeight() };
			SceneData shadowScene = {
				m_ShadowRenderTarget,
				camera,
				std::vector<LightSource>{}
			};

			m_CurrentRenderPass = RenderPass::ShadowFormation;
			glm::mat4 projections[6];
			GetCameraTransformsFromLightSource(m_CurrentScene.LightSources[0], m_ShadowRenderTarget->GetAspect(), camera.Frustum, projections);
			SetupScene(shadowScene);
			m_Context.SetPointShadowMatrices(projections);
			m_Context.SetShadowLightPosition(m_CurrentScene.LightSources[0].Position);
			RenderAll();

			m_CurrentRenderPass = RenderPass::WithShadow;
			Ref<Texture> shadowMap = m_ShadowRenderTarget->GetDepthAttachment();
			m_ShadowRenderTarget = nullptr;

			SetupScene(m_CurrentScene);
			m_Context.SetShadowMap(shadowMap);
			m_Context.SetShadowLightPosition(m_CurrentScene.LightSources[0].Position);
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
		m_Context.SetCameraPosition(glm::inverse(data.Camera.ViewMatrix)[3]);

		RenderCommand::EnableClippingPlanes(data.Camera.ClippingPlanes.size());
		RenderCommand::SetViewport(data.Camera.Viewport);
	}

	void Renderer3D::RenderAll()
	{
		for (const RenderData& data : m_Renderables)
			RenderModelInternal(data);
		if (m_RenderImGui)
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
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

			if (m_ShadowRenderTarget)
			{
				RenderSettings settings = submodel.Material->GetSettings();
				settings.Culling = CullFace::Front;
				m_Context.ApplyRenderSettings(settings);
			}
			else
				m_Context.ApplyRenderSettings(submodel.Material->GetSettings());

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
		camera.Frustum = Frustum::Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 50.0f);
		return camera;
	}

	void Renderer3D::GetCameraTransformsFromLightSource(const LightSource& light, float aspect, const Frustum& frustum, glm::mat4 transforms[6])
	{
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, frustum.NearPlane, frustum.FarPlane);
		transforms[0] = projection * glm::lookAt(light.Position, light.Position + glm::vec3{ 1.0, 0.0, 0.0 }, glm::vec3{ 0, -1,  0 });
		transforms[1] = projection * glm::lookAt(light.Position, light.Position + glm::vec3{-1.0, 0.0, 0.0 }, glm::vec3{ 0, -1,  0 });
		transforms[2] = projection * glm::lookAt(light.Position, light.Position + glm::vec3{ 0.0, 1.0, 0.0 }, glm::vec3{ 0,  0,  1 });
		transforms[3] = projection * glm::lookAt(light.Position, light.Position + glm::vec3{ 0.0,-1.0, 0.0 }, glm::vec3{ 0,  0, -1 });
		transforms[4] = projection * glm::lookAt(light.Position, light.Position + glm::vec3{ 0.0, 0.0, 1.0 }, glm::vec3{ 0, -1,  0 });
		transforms[5] = projection * glm::lookAt(light.Position, light.Position + glm::vec3{ 0.0, 0.0,-1.0 }, glm::vec3{ 0, -1,  0 });
	}

}
