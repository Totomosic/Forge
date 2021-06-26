#pragma once
#include "Renderer/Model.h"

namespace Forge
{

	struct FORGE_API ModelRendererComponent
	{
	public:
		Ref<Forge::Model> Model;

	public:
		ModelRendererComponent() = default;
		ModelRendererComponent(const Ref<Forge::Model>& model)
			: Model(model)
		{}
	};

	inline ModelRendererComponent CloneComponent(const ModelRendererComponent& component)
	{
		ModelRendererComponent result;
		result.Model = CreateRef<Forge::Model>(*component.Model);
		for (Model::SubModel& submodel : result.Model->GetSubModels())
		{
			submodel.Material = CreateRef<Material>(*submodel.Material);
		}
		return result;
	}

}
