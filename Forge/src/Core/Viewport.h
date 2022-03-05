#pragma once
#include "ForgePch.h"
#include <glm/glm.hpp>

namespace Forge
{

    struct FORGE_API Viewport
    {
    public:
        uint32_t Left;
        uint32_t Bottom;
        uint32_t Width;
        uint32_t Height;
    };

    inline glm::vec2 ConvertCoordinateBetweenViewports(
      const glm::vec2& coordinate, const Viewport& source, const Viewport& dest)
    {
        return glm::vec2 {(coordinate.x - source.Left) / source.Width * dest.Width + dest.Left,
          (coordinate.y - source.Bottom) / source.Height * dest.Height + dest.Bottom};
    }

    inline glm::vec3 ConvertCoordinateBetweenViewports(
      const glm::vec3& coordinate, const Viewport& source, const Viewport& dest)
    {
        return glm::vec3 {(coordinate.x - source.Left) / source.Width * dest.Width + dest.Left,
          (coordinate.y - source.Bottom) / source.Height * dest.Height + dest.Bottom,
          coordinate.z};
    }

}
