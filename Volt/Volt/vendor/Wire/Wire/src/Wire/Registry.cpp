#include "Registry.h"

#include "Serialization.h"

namespace Wire
{
	Registry::Registry(const Registry& registry)
	{
		m_nextEntityId = registry.m_nextEntityId;
		m_availiableIds = registry.m_availiableIds;
		m_pools = registry.m_pools;
	}

	Registry::~Registry()
	{
		Clear();
	}

	EntityId Registry::CreateEntity()
	{
		EntityId id;
		if (!m_availiableIds.empty())
		{
			id = m_availiableIds.back();
			m_availiableIds.pop_back();
		}
		else
		{
			id = m_nextEntityId++;
		}
		
		m_usedIds.emplace_back(id);

		return id;
	}

	EntityId Registry::AddEntity(EntityId aId)
	{
		assert(aId != 0);
		assert(std::find(m_usedIds.begin(), m_usedIds.end(), aId) == m_usedIds.end());

		if (m_nextEntityId <= aId)
		{
			m_nextEntityId = aId + 1;
		}

		if (auto it = std::find(m_availiableIds.begin(), m_availiableIds.end(), aId); it != m_availiableIds.end())
		{
			m_availiableIds.erase(it);
		}

		m_usedIds.emplace_back(aId);

		return aId;
	}

	void Registry::RemoveEntity(EntityId aId)
	{
		assert(aId != 0);
		assert(std::find(m_usedIds.begin(), m_usedIds.end(), aId) != m_usedIds.end());

		for (auto& [guid, pool] : m_pools)
		{
			if (pool->HasComponent(aId))
			{
				pool->RemoveComponent(aId);
			}
		}

		auto it = std::find(m_usedIds.begin(), m_usedIds.end(), aId);
		m_usedIds.erase(it);
		m_availiableIds.emplace_back(aId);
	}

	void Registry::Clear()
	{
		m_pools.clear();
		m_availiableIds.clear();
		m_usedIds.clear();
		m_nextEntityId = 1;
	}

	bool Registry::Exists(EntityId aId) const
	{
		return std::find(m_usedIds.begin(), m_usedIds.end(), aId) != m_usedIds.end();
	}

	bool Registry::HasComponent(WireGUID guid, EntityId id) const
	{
		if (m_pools.find(guid) == m_pools.end())
		{
			return false;
		}

		return m_pools.at(guid)->HasComponent(id);
	}

	void Registry::RemoveComponent(WireGUID guid, EntityId id)
	{
		if (m_pools.find(guid) == m_pools.end())
		{
			return;
		}

		m_pools.at(guid)->RemoveComponent(id);
	}

	void* Registry::GetComponentPtr(WireGUID guid, EntityId id) const
	{
		assert(HasComponent(guid, id));
		return m_pools.at(guid)->GetComponent(id);
	}

	void* Registry::AddComponent(WireGUID guid, EntityId id)
	{
		assert(!HasComponent(guid, id));

		if (m_pools.find(guid) == m_pools.end())
		{
			const auto info = ComponentRegistry::GetRegistryDataFromGUID(guid);
			if (info.size == 0)
			{
				return nullptr;
			}

			m_pools[guid] = info.poolCreateMethod();
			return m_pools[guid]->AddComponent(id, nullptr);
		}
		else
		{
			return m_pools.at(guid)->AddComponent(id, nullptr);
		}
	}

}
