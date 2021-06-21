#include "LibraryBrowserPanel.h"
using namespace Forge;

#include <imgui.h>

namespace Editor
{

	LibraryBrowserPanel::LibraryBrowserPanel()
		: m_Type(AssetLocationType::None), m_ShaderAssets(), m_MeshAssets(), m_Texture2DAssets(), m_DragDropPayload()
	{
		m_ShaderAssets = {
			DefaultColorShaderAssetLocation,
			DefaultTextureShaderAssetLocation,
			LitColorShaderAssetLocation,
			LitTextureShaderAssetLocation,
			PbrColorShaderAssetLocation,
		};
		m_MeshAssets = {
			SquareMeshAssetLocation,
			CubeMeshAssetLocation,
			SphereMeshAssetLocation
		};
		m_Texture2DAssets = {
			WhiteTextureAssetLocation,
		};
	}

	void LibraryBrowserPanel::OnImGuiRender()
	{
		ImGui::Begin("Library");

		if (m_Type != AssetLocationType::None)
		{
			if (ImGui::Button("<-"))
			{
				m_Type = AssetLocationType::None;
			}
			if (m_Type == AssetLocationType::Shader)
			{
				for (const AssetLocation& location : m_ShaderAssets)
				{
					CreateDragDropSource(location, "SHADER_ASSET_LOCATION_POINTER");
				}
			}
			if (m_Type == AssetLocationType::Mesh)
			{
				for (const AssetLocation& location : m_MeshAssets)
				{
					CreateDragDropSource(location, "MESH_ASSET_LOCATION_POINTER");
				}
			}
			if (m_Type == AssetLocationType::Texture2D)
			{
				for (const AssetLocation& location : m_Texture2DAssets)
				{
					CreateDragDropSource(location, "TEXTURE_ASSET_LOCATION_POINTER");
				}
			}
		}
		else
		{
			if (ImGui::Button("Shaders"))
			{
				m_Type = AssetLocationType::Shader;
			}
			if (ImGui::Button("Meshes"))
			{
				m_Type = AssetLocationType::Mesh;
			}
			if (ImGui::Button("Textures"))
			{
				m_Type = AssetLocationType::Texture2D;
			}
		}

		ImGui::End();
	}

	void LibraryBrowserPanel::CreateDragDropSource(const Forge::AssetLocation& location, const std::string& name)
	{
		ImGui::Button(location.Path.c_str());
		if (ImGui::BeginDragDropSource())
		{
			m_DragDropPayload = location;
			AssetLocation* pointer = &m_DragDropPayload;
			ImGui::SetDragDropPayload(name.c_str(), &pointer, sizeof(pointer));
			ImGui::Text(location.Path.c_str());
			ImGui::EndDragDropSource();
		}
	}

}
