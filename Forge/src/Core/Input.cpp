#include "ForgePch.h"
#include "Input.h"

namespace Forge
{

    const Window* Input::s_Window = nullptr;
    glm::vec2 Input::s_LastMousePos = { 0, 0 };

    EventEmitter<KeyCode> Input::OnKeyPressed;
    EventEmitter<KeyCode> Input::OnKeyReleased;

    EventEmitter<MouseMove> Input::OnMouseMoved;
    EventEmitter<MouseButton> Input::OnMousePressed;
    EventEmitter<MouseButton> Input::OnMouseReleased;

    void Input::SetWindow(const Window* window)
    {
        s_Window = window;
        GLFWwindow* handle = (GLFWwindow*)window->GetNativeHandle();

        glfwSetKeyCallback(handle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            switch (action)
            {
            case GLFW_PRESS:
                OnKeyPressed.Trigger(KeyCode(key));
                break;
            case GLFW_RELEASE:
                OnKeyReleased.Trigger(KeyCode(key));
                break;
            default:
                break;
            }
        });

        glfwSetMouseButtonCallback(handle, [](GLFWwindow* window, int button, int action, int mods)
        {
            switch (action)
            {
            case GLFW_PRESS:
                OnMousePressed.Trigger(MouseButton(button));
                break;
            case GLFW_RELEASE:
                OnMouseReleased.Trigger(MouseButton(button));
                break;
            default:
                break;
            }
        });

        glfwSetCursorPosCallback(handle, [](GLFWwindow* window, double xPos, double yPos)
        {
            glm::vec2 position = { float(xPos), float(yPos) };
            MouseMove evt;
            evt.Movement = position - s_LastMousePos;
            evt.Movement.y *= -1;
            s_LastMousePos = position;
            OnMouseMoved.Trigger(evt);
        });
    }

    bool Input::IsKeyDown(KeyCode key)
    {
        int state = glfwGetKey((GLFWwindow*)s_Window->GetNativeHandle(), int(key));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonDown(MouseButton button)
    {
        int state = glfwGetMouseButton((GLFWwindow*)s_Window->GetNativeHandle(), int(button));
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition()
    {
        double x;
        double y;
        glfwGetCursorPos((GLFWwindow*)s_Window->GetNativeHandle(), &x, &y);
        return glm::vec2{ float(x), s_Window->GetHeight() - float(y) };
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }

}
