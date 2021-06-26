#pragma once
#include "Forge.h"

namespace Editor
{

	class PostProcessingPanel
	{
	private:
		Forge::Renderer3D* m_Renderer;

	public:
		PostProcessingPanel() = default;

		void SetRenderer(Forge::Renderer3D* renderer);
		void OnImGuiRender();

	};

}
