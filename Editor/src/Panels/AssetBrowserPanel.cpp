#include "AssetBrowserPanel.h"
#include "Logging.h"

#include <imgui.h>

namespace Editor
{

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

		for (const std::filesystem::path& directory : m_Directories)
		{
			std::filesystem::path name = directory.filename();
			std::string nameString = name.string();
			if (ImGui::Button(nameString.c_str()))
			{
				m_CurrentDirectory /= name;
				Refresh();
			}
		}

		for (const std::filesystem::path& file : m_Files)
		{
			std::filesystem::path name = file.filename();
			std::string nameString = name.string();
			ImGui::Button(nameString.c_str());
			if (ImGui::BeginDragDropSource())
			{
				m_DragDropPayload = file.string();
				ImGui::SetDragDropPayload("PATH", m_DragDropPayload.c_str(), m_DragDropPayload.size() + 1);
				ImGui::Text(nameString.c_str());
				ImGui::EndDragDropSource();
			}
		}

		ImGui::End();
	}

}
