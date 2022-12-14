#pragma once

#include "Entity.h"
#include "WireGUID.h"
#include "ComponentPool.hpp"

namespace Wire
{
	class Registry
	{
	public:
		Registry() = default;
		Registry(const Registry& registry);
		~Registry();

		EntityId CreateEntity();
		EntityId AddEntity(EntityId aId);

		void RemoveEntity(EntityId aId);
		void Clear();
		bool Exists(EntityId aId) const;

		inline const std::vector<EntityId>& GetAllEntities() const { return m_usedIds; }
		inline const std::unordered_map<WireGUID, std::shared_ptr<ComponentPoolBase>>& GetPools() const { return m_pools; }

		bool HasComponent(WireGUID guid, EntityId id) const;
		void RemoveComponent(WireGUID guid, EntityId id);
		void* GetComponentPtr(WireGUID guid, EntityId id) const;

		void* AddComponent(WireGUID guid, EntityId id);

		template<typename T>
		T& AddComponent(EntityId aEntity, void* componentInitData = nullptr);

		template<typename T>
		T& GetComponent(EntityId aEntity);

		template<typename T>
		bool HasComponent(EntityId aEntity) const;

		template<typename ... T>
		bool HasComponents(EntityId aEntity) const;

		template<typename T>
		void RemoveComponent(EntityId aEntity);

		template<typename T>
		const std::vector<T>& GetAllComponents() const;

		template<typename T>
		const std::vector<EntityId> GetComponentView() const;

		template<typename T>
		void SetOnCreateFunction(std::function<void(EntityId, void*)>&& func);

		template<typename T>
		void SetOnRemoveFunction(std::function<void(EntityId, void*)>&& func);

		template<typename ... T, typename F>
		void ForEach(F&& func);

	private:
		std::unordered_map<WireGUID, std::shared_ptr<ComponentPoolBase>> m_pools;

		EntityId m_nextEntityId = 1; // ID zero is null
		std::vector<EntityId> m_availiableIds;
		std::vector<EntityId> m_usedIds;
	};

	template<typename T>
	inline T& Registry::AddComponent(EntityId aEntity, void* componentInitData)
	{
		const WireGUID guid = T::comp_guid;

		auto it = m_pools.find(guid);
		if (it != m_pools.end())
		{
			return *(T*)it->second->AddComponent(aEntity, componentInitData);
		}
		else
		{
			m_pools.emplace(guid, CreateRef<ComponentPool<T>>());
			return *(T*)m_pools[guid]->AddComponent(aEntity, componentInitData);
		}
	}

	template<typename T>
	inline T& Registry::GetComponent(EntityId aEntity)
	{
		assert(HasComponent<T>(aEntity));
		const WireGUID guid = T::comp_guid;

		return *(T*)m_pools.at(guid)->GetComponent(aEntity);
	}

	template<typename T>
	inline bool Registry::HasComponent(EntityId aEntity) const
	{
		const WireGUID guid = T::comp_guid;
		if (m_pools.find(guid) == m_pools.end())
		{
			return false;
		}

		return m_pools.at(guid)->HasComponent(aEntity);
	}

	template<typename ...T>
	inline bool Registry::HasComponents(EntityId aEntity) const
	{
		return (HasComponent<T>(aEntity) && ...);
	}

	template<typename T>
	inline void Registry::RemoveComponent(EntityId aEntity)
	{
		const WireGUID guid = T::comp_guid;

		auto it = m_pools.find(guid);
		assert(it != m_pools.end());

		it->second->RemoveComponent(aEntity);
	}

	template<typename T>
	inline const std::vector<T>& Registry::GetAllComponents() const
	{
		const WireGUID guid = T::comp_guid;

		auto it = m_pools.find(guid);
		assert(it != m_pools.end());

		return reinterpret_cast<const std::vector<T>&>(it->second.GetAllComponents());
	}

	template<typename T>
	inline const std::vector<EntityId> Registry::GetComponentView() const
	{
		const WireGUID guid = T::comp_guid;

		auto it = m_pools.find(guid);
		if (it != m_pools.end())
		{
			return it->second.GetComponentView();
		}

		return std::vector<EntityId>();
	}

	template<typename T>
	inline void Registry::SetOnCreateFunction(std::function<void(EntityId, void*)>&& func)
	{
		const WireGUID guid = T::comp_guid;

		auto it = m_pools.find(guid);
		if (it == m_pools.end())
		{
			m_pools.emplace(guid, CreateRef<ComponentPool<T>>());
		}

		m_pools.at(guid)->SetOnCreateFunction(func);
	}

	template<typename T>
	inline void Registry::SetOnRemoveFunction(std::function<void(EntityId, void*)> && func)
	{
		const WireGUID guid = T::comp_guid;

		auto it = m_pools.find(guid);
		if (it == m_pools.end())
		{
			m_pools.emplace(guid, CreateRef<ComponentPool<T>>());
		}

		m_pools.at(guid)->SetOnRemoveFunction(func);
	}

	template<typename ...T, typename F>
	inline void Registry::ForEach(F&& func)
	{
		const std::vector<EntityId> currentEntities = m_usedIds;
		const int64_t size = (int64_t)currentEntities.size();

		for (int64_t i = size - 1; i >= 0; --i)
		{
			const auto& id = currentEntities.at(i);

			if (HasComponents<T...>(id))
			{
				func(id, GetComponent<T>(id)...);
			}
		}
	}
}