#pragma once
#include "ForgePch.h"

#include "Renderer/VertexArray.h"
#include "Renderer/Shader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer3D.h"
#include "Assets/GraphicsCache.h"

#include "Core/Color.h"
#include "Core/EventEmitter.h"
#include "Core/Window.h"
#include "Core/Input.h"
#include "Core/Application.h"

#include "Math/Constants.h"
#include "Math/Math.h"

#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Transform.h"
#include "Scene/CameraComponent.h"
#include "Scene/ModelRenderer.h"
#include "Scene/Components.h"
#include "Scene/AnimatorComponent.h"

#include "Scene/SceneSerializer.h"

#include "Utils/Readers/GltfReader.h"
#include "Utils/Readers/ObjReader.h"

namespace Forge
{

	class FORGE_API ForgeInstance
	{
	public:
		static void Init();
	};

}
