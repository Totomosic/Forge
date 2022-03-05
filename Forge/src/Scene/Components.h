#pragma once
#include "Constants.h"
#include "Renderer/Lighting.h"

namespace Forge
{

    struct FORGE_API EnabledFlag
    {
    public:
        // Not used, just here so it is not a void type
        bool Storage;
    };

    struct FORGE_API LayerId
    {
    public:
        LayerMask Mask = 0;

    public:
        LayerId() = default;
        inline LayerId(LayerMask mask) : Mask(mask) {}
    };

    struct FORGE_API TagComponent
    {
    public:
        std::string Tag;

    public:
        TagComponent(const std::string& name) : Tag(name) {}
    };

    constexpr uint32_t DEFAULT_SHADOW_WIDTH = 1024;
    constexpr uint32_t DEFAULT_SHADOW_HEIGHT = 1024;

    struct FORGE_API ShadowPass
    {
    public:
        bool Enabled = false;
        Ref<Framebuffer> RenderTarget = nullptr;
        Forge::LayerMask LayerMask = FULL_LAYER_MASK;
    };

    struct FORGE_API PointLightComponent
    {
    public:
        static constexpr LightType Type = LightType::Point;
        float Ambient = 0.03f;
        Forge::Color Color = COLOR_WHITE;
        float Radius = 10.0f;
        float Cutoff = 1.0f;
        float Intensity = 1.0f;
        ShadowPass Shadows;

    public:
        PointLightComponent(
          float ambient = 0.03f, const Forge::Color& color = COLOR_WHITE, float radius = 10.0f, float cutoff = 1.0f)
            : Ambient(ambient), Color(color), Radius(radius), Cutoff(cutoff)
        {
        }

        void CreateShadowPass(uint32_t width = DEFAULT_SHADOW_WIDTH, uint32_t height = DEFAULT_SHADOW_HEIGHT)
        {
            Shadows.Enabled = true;
            FramebufferProps props;
            props.Width = width;
            props.Height = height;
            props.Attachments = {{FramebufferTextureFormat::Depth, FramebufferTextureType::TextureCube}};
            Shadows.RenderTarget = Framebuffer::Create(props);
        }
    };

    struct FORGE_API DirectionalLightComponent
    {
    public:
        static constexpr LightType Type = LightType::Directional;
        float Ambient = 0.03f;
        Forge::Color Color = COLOR_WHITE;
        float Intensity = 1.0f;
        ShadowPass Shadows;

    public:
        DirectionalLightComponent(float ambient = 0.03f, const Forge::Color& color = COLOR_WHITE)
            : Ambient(ambient), Color(color)
        {
        }

        void CreateShadowPass(uint32_t width = DEFAULT_SHADOW_WIDTH, uint32_t height = DEFAULT_SHADOW_HEIGHT)
        {
            Shadows.Enabled = true;
            FramebufferProps props;
            props.Width = width;
            props.Height = height;
            props.Attachments = {{FramebufferTextureFormat::Depth, FramebufferTextureType::Texture2D}};
            Shadows.RenderTarget = Framebuffer::Create(props);
        }
    };

    inline TagComponent CloneComponent(const TagComponent& component)
    {
        return component;
    }

    inline LayerId CloneComponent(const LayerId& component)
    {
        return component;
    }

    inline EnabledFlag CloneComponent(const EnabledFlag& component)
    {
        return component;
    }

    inline PointLightComponent CloneComponent(const PointLightComponent& component)
    {
        PointLightComponent result = component;
        if (result.Shadows.Enabled)
        {
            result.CreateShadowPass(
              component.Shadows.RenderTarget->GetWidth(), component.Shadows.RenderTarget->GetHeight());
        }
        return result;
    }

    inline DirectionalLightComponent CloneComponent(const DirectionalLightComponent& component)
    {
        DirectionalLightComponent result = component;
        if (result.Shadows.Enabled)
        {
            result.CreateShadowPass(
              component.Shadows.RenderTarget->GetWidth(), component.Shadows.RenderTarget->GetHeight());
        }
        return result;
    }

}
