#pragma once
#include "Window.h"
#include "Keycodes.h"

#include <glm/glm.hpp>

namespace Forge
{

	struct FORGE_API MouseMove
	{
	public:
		glm::vec2 Movement;
	};

	class FORGE_API Input
	{
	private:
		static const Window* s_Window;
		static glm::vec2 s_LastMousePos;
		
		static glm::vec2 s_MouseDownPos;
		static MouseButton s_MouseDownButton;

	public:
		static EventEmitter<KeyCode> OnKeyPressed;
		static EventEmitter<KeyCode> OnKeyReleased;

		static EventEmitter<MouseMove> OnMouseMoved;
		static EventEmitter<MouseButton> OnMousePressed;
		static EventEmitter<MouseButton> OnMouseReleased;
		static EventEmitter<MouseButton> OnMouseClicked;

	public:
		static void SetWindow(const Window* window);

		static bool IsKeyDown(KeyCode key);

		static bool IsMouseButtonDown(MouseButton button);
		static glm::vec2 GetMousePosition();
		static glm::vec2 GetRelMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}
