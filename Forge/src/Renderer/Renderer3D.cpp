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
		: m_CurrentScene(), m_CurrentRenderPass(), m_RenderImGui(false), m_CurrentShadowLightIndex(0), m_Context(), m_ClearedFramebuffers(), m_PostProcessor()
	{
		m_PostProcessor.SetEnabled(true);
	}

	void Renderer3D::SetTime(float time)
	{
		m_Context.SetTime(time);
	}

	void Renderer3D::AddShadowPass(const Ref<Framebuffer>& framebuffer, const LightSource& light, int index)
	{
		ShadowPass pass;
		pass.Light = &light;
		pass.RenderTarget = framebuffer;
		pass.LightIndex = index;
		m_ShadowPasses.push_back(pass);
	}

	void Renderer3D::BeginPickScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera)
	{
		m_CurrentScene = {
			framebuffer,
			camera,
		};
		m_Renderables.clear();
		m_ShadowPasses.clear();
		m_RenderImGui = false;
		m_CurrentRenderPass = RenderPass::Pick;
	}

	void Renderer3D::BeginScene(const Ref<Framebuffer>& framebuffer, const CameraData& camera, const std::vector<LightSource>& lightSources)
	{
		m_CurrentScene = {
			framebuffer,
			camera,
			lightSources,
			camera.UsePostProcessing,
		};
		m_Renderables.clear();
		m_ShadowPasses.clear();
		m_RenderImGui = false;
		m_CurrentRenderPass = RenderPass::PointShadowFormation;

		if (camera.UsePostProcessing)
			m_PostProcessor.Init(framebuffer->GetWidth(), framebuffer->GetHeight());

		int index = 0;
		for (const LightSource& light : m_CurrentScene.LightSources)
		{
			if (light.ShadowFramebuffer)
				AddShadowPass(light.ShadowFramebuffer, light, index);
			index++;
		}
	}

	void Renderer3D::EndScene()
	{
		m_Context.Reset();
		if (m_CurrentRenderPass != RenderPass::Pick)
		{
			if (!m_ShadowPasses.empty())
			{
				for (const ShadowPass& pass : m_ShadowPasses)
				{
					RenderShadowScene(pass);
				}
				m_CurrentRenderPass = RenderPass::WithShadow;
				SetupScene(m_CurrentScene);
				RenderAll();
			}
			else
			{
				m_CurrentRenderPass = RenderPass::WithoutShadow;
				SetupScene(m_CurrentScene);
				RenderAll();
			}
			if (m_CurrentScene.UsePostProcessing)
				m_PostProcessor.Render(m_Context);
			RenderImGuiInternal();
		}
		else
		{
			SetupScene(m_CurrentScene);
			RenderAll();
		}
	}

	void Renderer3D::Flush()
	{
		m_ClearedFramebuffers.clear();
		m_Stats = {};
	}

	void Renderer3D::RenderModel(const Ref<Model>& model, const glm::mat4& transform, const RenderOptions& options)
	{
		m_Renderables.push_back({ model, transform, options });
	}

	void Renderer3D::RenderShadowScene(const ShadowPass& pass)
	{
		CameraData camera = m_CurrentScene.Camera;
		camera.Viewport = { 0, 0, pass.RenderTarget->GetWidth(), pass.RenderTarget->GetHeight() };
		SceneData shadowScene = {
			pass.RenderTarget,
			camera,
		};

		m_CurrentShadowLightIndex = pass.LightIndex;

		if (pass.Light->Type == LightType::Point)
		{
			m_CurrentRenderPass = RenderPass::PointShadowFormation;
			glm::mat4 matrices[6];
			GetCameraTransformsFromLightSource(pass.Light->Position, pass.RenderTarget->GetAspect(), pass.Light->ShadowFrustum, matrices);
			m_Context.SetShadowPointMatrices(pass.Light->Position, matrices);
		}
		else
		{
			m_CurrentRenderPass = RenderPass::ShadowFormation;
			shadowScene.Camera = CreateCameraFromLightSource(pass.Light->Position, pass.Light->Direction, pass.RenderTarget, pass.Light->ShadowFrustum);
			pass.Light->LightSpaceTransform = shadowScene.Camera.Frustum.ProjectionMatrix * shadowScene.Camera.ViewMatrix;
		}
		SetupScene(shadowScene);
		RenderAll();
	}

	void Renderer3D::SetupScene(const SceneData& data)
	{
		FORGE_ASSERT(data.RenderTarget != nullptr, "Invalid framebuffer");
		if (data.RenderTarget != m_CurrentFramebuffer || data.UsePostProcessing)
		{
			if (data.UsePostProcessing)
			{
				m_PostProcessor.SetDestination(data.RenderTarget);
			}
			else
			{
				data.RenderTarget->Bind();
			}
			m_CurrentFramebuffer = data.RenderTarget;
		}
		if (m_CurrentRenderPass != RenderPass::Pick)
		{
			if (m_CurrentRenderPass == RenderPass::PointShadowFormation || m_CurrentRenderPass == RenderPass::ShadowFormation)
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
		}
		m_Context.NewScene();
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

	void Renderer3D::RenderImGuiInternal()
	{
		if (m_RenderImGui && m_CurrentRenderPass != RenderPass::Pick)
		{
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}

	void Renderer3D::RenderModelInternal(const RenderData& data)
	{
		if (m_CurrentRenderPass == RenderPass::PointShadowFormation && !data.Options.ShadowMask.test(m_CurrentShadowLightIndex))
			return;
		const Ref<Model>& model = data.Model;
		const glm::mat4& transform = data.Transform;
		for (const Model::SubModel& submodel : model->GetSubModels())
		{
			const Ref<Mesh>& mesh = submodel.Mesh;
			Ref<Material> material = submodel.Material;
			glm::mat4 overallTransform = transform * submodel.Transform;

			if (m_CurrentRenderPass == RenderPass::PointShadowFormation || m_CurrentRenderPass == RenderPass::ShadowFormation)
			{
				RenderSettings settings = submodel.Material->GetSettings();
				if (settings.Culling != CullFace::None)
					settings.Culling = CullFace::Front;
				m_Context.ApplyRenderSettings(settings);
			}
			else
				m_Context.ApplyRenderSettings(submodel.Material->GetSettings());

			ShaderRequirements requirements = m_Context.GetShaderRequirements(material->GetShader(m_CurrentRenderPass));
			m_Context.BindShader(material->GetShader(m_CurrentRenderPass), requirements);
			if (requirements.ModelMatrix)
				material->GetShader(m_CurrentRenderPass)->SetUniform(ModelMatrixUniformName, overallTransform);
			if (m_CurrentRenderPass == RenderPass::Pick)
				material->GetShader(m_CurrentRenderPass)->SetUniform(EntityIdUniformName, data.Options.EntityId);
			material->Apply(m_CurrentRenderPass, m_Context);
			mesh->Apply(material->GetShader(m_CurrentRenderPass), requirements);

			RenderCommand::DrawIndexed(mesh->GetDrawMode(), mesh->GetVertices());
			m_Context.NewDrawCall();
			m_Stats.DrawCount++;
		}
	}

	CameraData Renderer3D::CreateCameraFromLightSource(const glm::vec3& lightPosition, const glm::vec3& lightDirection, const Ref<Framebuffer>& renderTarget, const Frustum& frustum) const
	{
		CameraData camera;
		camera.Viewport = { 0, 0, renderTarget->GetWidth(), renderTarget->GetHeight() };
		camera.ViewMatrix = glm::lookAt(lightPosition, lightPosition + lightDirection, { 0.0f, 1.0f, 0.0f });
		camera.Frustum = frustum;
		return camera;
	}

	void Renderer3D::GetCameraTransformsFromLightSource(const glm::vec3& lightPosition, float aspect, const Frustum& frustum, glm::mat4 transforms[6])
	{
		glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, frustum.NearPlane, frustum.FarPlane);
		transforms[0] = projection * glm::lookAt(lightPosition, lightPosition + glm::vec3{ 1.0, 0.0, 0.0 }, glm::vec3{ 0, -1,  0 });
		transforms[1] = projection * glm::lookAt(lightPosition, lightPosition + glm::vec3{-1.0, 0.0, 0.0 }, glm::vec3{ 0, -1,  0 });
		transforms[2] = projection * glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0, 1.0, 0.0 }, glm::vec3{ 0,  0,  1 });
		transforms[3] = projection * glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0,-1.0, 0.0 }, glm::vec3{ 0,  0, -1 });
		transforms[4] = projection * glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0, 0.0, 1.0 }, glm::vec3{ 0, -1,  0 });
		transforms[5] = projection * glm::lookAt(lightPosition, lightPosition + glm::vec3{ 0.0, 0.0,-1.0 }, glm::vec3{ 0, -1,  0 });
	}

}
