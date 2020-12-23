#pragma once
#include "Logging.h"

struct GLFWwindow;

namespace Forge
{

	class FORGE_API GraphicsContext
	{
	private:
		GLFWwindow* m_Handle;

	public:
		GraphicsContext(GLFWwindow* handle);

		void Init();
		void SwapBuffers();
	};

}
