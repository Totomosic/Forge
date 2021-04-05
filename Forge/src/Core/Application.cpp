#include "ForgePch.h"
#include "Application.h"

#include "Input.h"
#include "Renderer/RenderCommand.h"

namespace Forge
{

    Application::Application(const WindowProps& props)
        : m_Window(props), m_Scenes(), m_Renderer(), m_PrevFrameTime(std::chrono::high_resolution_clock::now())
    {
        RenderCommand::Init();
        Input::SetWindow(&m_Window);

        m_Window.Events.Resize.AddEventListener([&](const WindowResize& evt)
        {
            m_Window.GetFramebuffer()->SetViewport({ 0, 0, evt.NewWidth, evt.NewHeight });
            return false;
        });
    }

    void Application::SetClearColor(const Color& color)
    {
        RenderCommand::SetClearColor(color);
    }

    Scene& Application::CreateScene()
    {
        Scope<Scene> scene = CreateScope<Scene>(m_Window.GetFramebuffer());
        Scene& ref = *scene;
        m_Scenes.push_back(std::move(scene));
        return ref;
    }

    void Application::OnUpdate()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
        Timestep ts = float(std::chrono::duration_cast<std::chrono::microseconds>(now - m_PrevFrameTime).count()) / 1e6;
        for (const Scope<Scene>& scene : m_Scenes)
        {
            scene->OnUpdate(ts, m_Renderer);
        }
        m_Renderer.Flush();
        m_Window.Update();
        m_PrevFrameTime = now;
    }

}
