#pragma once
#include <random>
#include "ForgePch.h"

namespace Forge
{

    class FORGE_API Random
    {
    private:
        static std::mt19937 s_Engine;

    public:
        inline static float Float(float min, float max)
        {
            auto dist = std::uniform_real_distribution<float>(min, max);
            return dist(s_Engine);
        }
    };

}
