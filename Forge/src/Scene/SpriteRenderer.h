#pragma once
#include "Renderer/Texture.h"

namespace Forge
{

	struct FORGE_API SpriteRendererComponent
	{
	public:
		Forge::Color Color;
		Ref<Texture2D> Texture = nullptr;

	public:
		SpriteRendererComponent() = default;
		inline SpriteRendererComponent(const Forge::Color& color)
			: Color(color)
		{}
	};

}
