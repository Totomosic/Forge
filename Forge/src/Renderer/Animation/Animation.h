#pragma once
#include "Joint.h"

namespace Forge
{

	struct FORGE_API AnimationKeyFrame
	{
	public:
		std::vector<JointTransform> Transforms;
		float TimeStamp;
	};

	struct FORGE_API Animation
	{
	public:
		std::vector<AnimationKeyFrame> KeyFrames;

	public:
		inline float GetLength() const
		{
			if (KeyFrames.empty())
				return 0.0f;
			return KeyFrames.back().TimeStamp;
		}
	};

}
