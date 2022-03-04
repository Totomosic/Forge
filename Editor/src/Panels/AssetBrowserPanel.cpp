#include "AssetBrowserPanel.h"
#include "Logging.h"

#include <imgui.h>
using namespace Forge;

namespace Editor
{
	AssetBrowserPanel::AssetBrowserPanel()
	{
		m_DirectoryIcon = GraphicsCache::LoadTexture2D("resources/Icons/DirectoryIcon.png");
		m_FileIcon = GraphicsCache::LoadTexture2D("resources/Icons/FileIcon.png");
	}

	void AssetBrowserPanel::SetRootDirectory(const std::filesystem::path& directory)
	{
		m_RootDirectory = directory;
		m_CurrentDirectory = directory;
		Refresh();
	}

	void AssetBrowserPanel::Refresh()
	{
		m_Directories.clear();
		m_Files.clear();

		if (std::filesystem::exists(m_CurrentDirectory))
		{
			for (auto it : std::filesystem::directory_iterator(m_CurrentDirectory))
			{
				if (it.is_directory())
				{
					m_Directories.push_back(it.path());
				}
				else
				{
					m_Files.push_back(it.path());
				}
			}
		}
	}

	void AssetBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Asset Browser");

		if (!IsBrowsingRootDirectory())
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
				Refresh();
			}
		}

		float padding = 10;
		float thumbnailSize = 80;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columns = std::max(int(panelWidth / cellSize), 1);

		ImGui::Columns(columns, 0, false);

		for (const std::filesystem::path& directory : m_Directories)
		{
			std::filesystem::path name = directory.filename();
			std::string nameString = name.string();
			ImGui::PushID(nameString.c_str());
			if (ImGui::ImageButton((void*)m_DirectoryIcon->GetId(), { thumbnailSize, thumbnailSize }))
			{
				m_CurrentDirectory /= name;
				Refresh();
			}
			ImGui::Text(nameString.c_str());
			ImGui::PopID();
			ImGui::NextColumn();
		}

		for (const std::filesystem::path& file : m_Files)
		{
			std::filesystem::path name = file.filename();
			std::string nameString = name.string();
			ImGui::PushID(nameString.c_str());
			ImGui::ImageButton((void*)m_FileIcon->GetId(), { thumbnailSize, thumbnailSize });
			if (ImGui::BeginDragDropSource())
			{
				m_DragDropPayload = file.string();
				ImGui::SetDragDropPayload("PATH", m_DragDropPayload.c_str(), m_DragDropPayload.size() + 1);
				ImGui::Text(nameString.c_str());
				ImGui::EndDragDropSource();
			}
			ImGui::Text(nameString.c_str());
			ImGui::PopID();
			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

}
