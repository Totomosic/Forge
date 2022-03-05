#pragma once
#include "Constants.h"
#include "Renderer/Framebuffer.h"
#include "Renderer/CameraData.h"

#include <glm/glm.hpp>

namespace Forge
{

    struct FORGE_API CameraComponent
    {
    public:
        Forge::Frustum Frustum;
        Forge::Viewport Viewport;
        LayerMask LayerMask = FULL_LAYER_MASK;
        int Priority = 0;
        Ref<Framebuffer> RenderTarget = nullptr;
        std::vector<glm::vec4> ClippingPlanes = {};
        Color ClearColor = COLOR_BLACK;
        CameraMode Mode = CameraMode::Normal;
        bool UsePostProcessing = true;

    public:
        CameraComponent() = default;
        inline CameraComponent(const Forge::Frustum& frustum) : Frustum(frustum), Viewport() {}
    };

    inline CameraComponent CloneComponent(const CameraComponent& component)
    {
        CameraComponent result = component;
        return result;
    }

}
