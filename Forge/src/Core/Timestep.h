#pragma once
#include "ForgePch.h"

namespace Forge
{

	class FORGE_API Timestep
	{
	private:
		float m_Seconds;

	public:
		Timestep(float seconds = 0.0f)
			: m_Seconds(seconds)
		{}

		inline operator float() const { return m_Seconds; }
		inline float Seconds() const { return m_Seconds; }
		inline float Milliseconds() const { return Seconds() * 1000.0f; }
	};

}
