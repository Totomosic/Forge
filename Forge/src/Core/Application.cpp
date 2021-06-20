#include "ForgePch.h"
#include "Application.h"

#include "Input.h"
#include "Renderer/RenderCommand.h"

#include <imgui.h>

namespace Forge
{

    Application::Application(const WindowProps& props)
        : m_Window(props), m_Scenes(), m_Renderer(), m_PrevFrameTime(std::chrono::high_resolution_clock::now()), m_ImGuiLayer(nullptr), m_LayerStack()
    {
        RenderCommand::Init();
        Input::SetWindow(&m_Window);
    }

    void Application::EnableImGui()
    {
        m_ImGuiLayer = &PushOverlay<ImGuiLayer>();
    }

    void Application::SetClearColor(const Color& color)
    {
        RenderCommand::SetClearColor(color);
    }

    Scene& Application::CreateScene()
    {
        Scope<Scene> scene = CreateScope<Scene>(m_Window.GetFramebuffer(), &m_Renderer);
        Scene& ref = *scene;
        m_Scenes.push_back(std::move(scene));
        return ref;
    }

    void Application::OnUpdate()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        Timestep ts = float(std::chrono::duration_cast<std::chrono::microseconds>(now - m_PrevFrameTime).count()) / 1e6;
        for (const std::unique_ptr<Layer>& layer : m_LayerStack)
        {
            layer->OnUpdate(ts);
        }
        for (const Scope<Scene>& scene : m_Scenes)
        {
            scene->OnUpdate(ts);
        }
        if (m_ImGuiLayer)
        {
            m_ImGuiLayer->Begin();
            CameraData camera;
            camera.Viewport = m_Window.GetFramebuffer()->GetViewport();
            camera.ViewMatrix = glm::mat4(1.0f);
            m_Renderer.BeginScene(m_Window.GetFramebuffer(), camera);
            for (const std::unique_ptr<Layer>& layer : m_LayerStack)
            {
                layer->OnImGuiRender();
            }
            m_Renderer.RenderImGui();
            m_Renderer.EndScene();
            m_ImGuiLayer->End();
        }
        m_Renderer.Flush();
        m_Window.Update();
        m_PrevFrameTime = now;
    }

}
