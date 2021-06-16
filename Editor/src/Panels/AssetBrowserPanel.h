#pragma once
#include <filesystem>
#include <vector>

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

	public:
		AssetBrowserPanel() = default;

		inline bool IsBrowsingRootDirectory() const { return m_CurrentDirectory == m_RootDirectory; }

		void SetRootDirectory(const std::filesystem::path& directory);
		void Refresh();

		void OnImGuiRender();
	};

}
