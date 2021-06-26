#pragma once
#include "Scene.h"

namespace Forge
{

	class SceneSerializer
	{
	private:
		Scene* m_Scene;

	public:
		SceneSerializer(Scene* scene);

		void SerializeText(const std::string& filename);
		bool DeserializeText(const std::string& filename);
	};

}
