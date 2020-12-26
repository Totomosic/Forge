#pragma once
#include "ForgePch.h"

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Renderer/GraphicsCache.h"

#include "Core/Color.h"
#include "Core/EventEmitter.h"
#include "Core/Window.h"
#include "Core/Input.h"

#include "Math/Constants.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Transform.h"
#include "Scene/CameraComponent.h"
#include "Scene/ModelRenderer.h"
#include "Scene/Components.h"

#include "Utils/Readers/GltfReader.h"

namespace Forge
{

	class FORGE_API ForgeInstance
	{
	public:
		static void Init();
	};

}
