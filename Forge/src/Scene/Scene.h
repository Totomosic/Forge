#pragma once
#include "ForgePch.h"
#include "Core/Timestep.h"
#include "Renderer/Renderer3D.h"

#include <entt/entt.hpp>
#include <map>

namespace Forge
{

	class Entity;

	class FORGE_API Scene
	{
	private:
		static constexpr uint8_t DEFAULT_LAYER = 0;

		entt::registry m_Registry;
		entt::entity m_PrimaryCamera;

		Ref<Framebuffer> m_DefaultFramebuffer;

	public:
		Scene(const Ref<Framebuffer>& defaultFramebuffer);

		inline bool HasPrimaryCamera() { FindPrimaryCamera(); return m_PrimaryCamera != entt::null; }
		Entity GetPrimaryCamera();
		Entity CreateEntity(uint8_t layer = DEFAULT_LAYER);
		void DestroyEntity(const Entity& entity);
		void SetLayer(Entity entity, uint8_t layer);
		void AddLayer(Entity entity, uint8_t layer);
		void RemoveLayer(Entity entity, uint8_t layer);
		void AddToAllLayers(Entity entity);

		void SetPrimaryCamera(const Entity& entity);
		Entity CreateCamera(const glm::mat4& projection);

		void OnUpdate(Timestep ts, Renderer3D& renderer);

		friend class Entity;

	private:
		void FindPrimaryCamera();
		bool CheckLayerMask(entt::entity entity, uint64_t layerMask) const;

	};

}
