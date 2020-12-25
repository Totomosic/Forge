#pragma once
#include "ForgePch.h"
#include "Core/Timestep.h"
#include "Renderer/Renderer3D.h"

#include <entt/entt.hpp>

namespace Forge
{

	class Entity;

	class FORGE_API Scene
	{
	private:
		entt::registry m_Registry;
		entt::entity m_PrimaryCamera;

	public:
		Scene();

		inline bool HasPrimaryCamera() { FindPrimaryCamera(); return m_PrimaryCamera != entt::null; }
		Entity GetPrimaryCamera();
		Entity CreateEntity();
		void DestroyEntity(const Entity& entity);

		void SetPrimaryCamera(const Entity& entity);

		void OnUpdate(Timestep ts, Renderer3D& renderer);

		friend class Entity;

	private:
		void FindPrimaryCamera();

	};

}
