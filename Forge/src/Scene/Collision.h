#pragma once
#include "ForgePch.h"
#include <glm/glm.hpp>

namespace Forge
{

    struct FORGE_API AABB
    {
    public:
        glm::vec3 Min;
        glm::vec3 Max;
    };

    struct FORGE_API OBB
    {
    public:
        glm::vec3 Axes[3];
        glm::vec3 Center;
        glm::vec3 HalfSize;
    };

    namespace Detail
    {

        inline bool IsSeparatingPlane(
          const glm::vec3& relativePos, const glm::vec3& planeNormal, const OBB& box1, const OBB& box2)
        {
            return std::fabs(glm::dot(relativePos, planeNormal)) >
                   (std::fabs(glm::dot(box1.Axes[0] * box1.HalfSize.x, planeNormal)) +
                     std::fabs(glm::dot(box1.Axes[1] * box1.HalfSize.y, planeNormal)) +
                     std::fabs(glm::dot(box1.Axes[2] * box1.HalfSize.z, planeNormal)) +
                     std::fabs(glm::dot(box2.Axes[0] * box2.HalfSize.x, planeNormal)) +
                     std::fabs(glm::dot(box2.Axes[1] * box2.HalfSize.y, planeNormal)) +
                     std::fabs(glm::dot(box2.Axes[2] * box2.HalfSize.z, planeNormal)));
        }

    }

    struct FORGE_API CollisionResult
    {
    public:
        bool Collision;
    };

    inline OBB CreateOBB(const glm::vec3& dimensions, const glm::mat4& transform)
    {
        glm::vec4 xAxis = transform * glm::vec4 {1.0f, 0.0f, 0.0f, 0.0f};
        glm::vec4 yAxis = transform * glm::vec4 {0.0f, 1.0f, 0.0f, 0.0f};
        glm::vec4 zAxis = transform * glm::vec4 {0.0f, 0.0f, 1.0f, 0.0f};
        glm::vec4 center = transform[3];

        OBB result;
        result.Center = center;
        result.Axes[0] = xAxis;
        result.Axes[1] = yAxis;
        result.Axes[2] = zAxis;
        result.HalfSize = dimensions / 2.0f;
        return result;
    }

    inline CollisionResult TestOBBIntersection(const OBB& box1, const OBB& box2)
    {
        glm::vec3 rpos = box2.Center - box1.Center;
        CollisionResult result;
        result.Collision = !(Detail::IsSeparatingPlane(rpos, box1.Axes[0], box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, box1.Axes[1], box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, box1.Axes[2], box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, box2.Axes[0], box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, box2.Axes[1], box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, box2.Axes[2], box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[0], box2.Axes[0]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[0], box2.Axes[1]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[0], box2.Axes[2]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[1], box2.Axes[0]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[1], box2.Axes[1]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[1], box2.Axes[2]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[2], box2.Axes[0]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[2], box2.Axes[1]), box1, box2) ||
                             Detail::IsSeparatingPlane(rpos, glm::cross(box1.Axes[2], box2.Axes[2]), box1, box2));
        return result;
    }

}
