#include "ForgePch.h"
#include "Application.h"

#include "Input.h"
#include "Renderer/RenderCommand.h"

#include <imgui.h>

namespace Forge
{

    Application::Application(const WindowProps& props)
        : m_Window(nullptr),
          m_Renderer(nullptr),
          m_Scenes(),
          m_PrevFrameTime(std::chrono::high_resolution_clock::now()),
          m_ImGuiLayer(nullptr),
          m_LayerStack()
    {
        if (!props.NoGraphics)
        {
            m_Window = std::make_unique<Window>(props);
            m_Renderer = std::make_unique<Renderer3D>();
            RenderCommand::Init();
            Input::SetWindow(GetWindow());
        }
    }

    void Application::EnableImGui()
    {
        if (m_Window)
            m_ImGuiLayer = &PushOverlay<ImGuiLayer>();
    }

    void Application::SetClearColor(const Color& color)
    {
        if (m_Window)
            RenderCommand::SetClearColor(color);
    }

    Scene& Application::CreateScene()
    {
        Ref<Framebuffer> framebuffer = nullptr;
        if (m_Window)
            framebuffer = m_Window->GetFramebuffer();
        Scope<Scene> scene = CreateScope<Scene>(framebuffer, GetRenderer());
        Scene& ref = *scene;
        m_Scenes.push_back(std::move(scene));
        return ref;
    }

    RendererStats Application::OnUpdate()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        Timestep ts = float(std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_PrevFrameTime).count()) / 1e9f;
        for (const std::unique_ptr<Layer>& layer : m_LayerStack)
        {
            layer->OnUpdate(ts);
        }
        for (const Scope<Scene>& scene : m_Scenes)
        {
            scene->OnUpdate(ts);
        }
        if (m_ImGuiLayer && m_Window)
        {
            m_ImGuiLayer->Begin();
            CameraData camera;
            camera.Viewport = m_Window->GetFramebuffer()->GetViewport();
            camera.ViewMatrix = glm::mat4(1.0f);
            camera.UsePostProcessing = false;
            m_Renderer->BeginScene(m_Window->GetFramebuffer(), camera);
            for (const std::unique_ptr<Layer>& layer : m_LayerStack)
            {
                layer->OnImGuiRender();
            }
            m_Renderer->RenderImGui();
            m_Renderer->EndScene();
            m_ImGuiLayer->End();
        }
        RendererStats stats;
        if (m_Renderer && m_Window)
        {
            stats = m_Renderer->GetStats();
            m_Renderer->Flush();
            m_Window->Update();
        }
        m_PrevFrameTime = now;
        return stats;
    }

}
