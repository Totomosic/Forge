#pragma once
#include "Forge.h"

namespace Editor
{

	class SceneHierarchyPanel
	{
	private:
		Forge::Scene* m_Scene;
		Forge::Entity m_SelectedEntity;

	public:
		SceneHierarchyPanel() = default;

		inline Forge::Entity GetSelectedEntity() const { return m_SelectedEntity; }
		inline void SetSelectedEntity(Forge::Entity entity) { m_SelectedEntity = entity; }

		void SetScene(Forge::Scene* scene);
		void OnImGuiRender();

	private:
		void DrawEntityNode(Forge::Entity entity);
		void DrawComponents(Forge::Entity entity);

	};

}
