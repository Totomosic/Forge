#pragma once
#include <filesystem>
#include <vector>
#include "Forge.h"

namespace Editor
{

	class AssetBrowserPanel
	{
	private:
		std::filesystem::path m_RootDirectory;
		std::filesystem::path m_CurrentDirectory;

		std::vector<std::filesystem::path> m_Directories;
		std::vector<std::filesystem::path> m_Files;

		std::string m_DragDropPayload;

		Forge::Ref<Forge::Texture2D> m_DirectoryIcon;
		Forge::Ref<Forge::Texture2D> m_FileIcon;

	public:
		AssetBrowserPanel();

		inline bool IsBrowsingRootDirectory() const { return m_CurrentDirectory == m_RootDirectory; }

		void SetRootDirectory(const std::filesystem::path& directory);
		void Refresh();

		void OnImGuiRender();
	};

}
