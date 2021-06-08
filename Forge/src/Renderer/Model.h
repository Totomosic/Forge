#pragma once
#include "Mesh.h"
#include "Material.h"

namespace Forge
{

	class FORGE_API Model
	{
	public:
		struct FORGE_API SubModel
		{
		public:
			Ref<Forge::Mesh> Mesh;
			Ref<Forge::Material> Material;
			glm::mat4 Transform;
		};

	private:
		std::vector<SubModel> m_SubModels;

	public:
		inline Model()
			: m_SubModels()
		{}

		inline Model(std::initializer_list<SubModel> models)
			: m_SubModels(models)
		{}

		inline const std::vector<SubModel>& GetSubModels() const { return m_SubModels; }
		inline std::vector<SubModel>& GetSubModels() { return m_SubModels; }

	public:
		inline static Ref<Model> Create(const Ref<Mesh>& mesh, const Ref<Material>& material)
		{
			return CreateRef<Model>(std::initializer_list<SubModel>{ { mesh, material, glm::mat4(1.0f) } });
		}

	};

}
