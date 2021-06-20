#pragma once
#include "Forge.h"

namespace Editor
{

	class LibraryBrowserPanel
	{
	private:
		Forge::AssetLocationType m_Type = Forge::AssetLocationType::None;

		std::vector<Forge::AssetLocation> m_ShaderAssets;
		std::vector<Forge::AssetLocation> m_MeshAssets;
		std::vector<Forge::AssetLocation> m_Texture2DAssets;
		Forge::AssetLocation m_DragDropPayload;

	public:
		LibraryBrowserPanel();

		void OnImGuiRender();

	private:
		void CreateDragDropSource(const Forge::AssetLocation& location, const std::string& name);
	};

}
