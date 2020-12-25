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

}
