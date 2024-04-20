#include "EntityManager.h"
#include <algorithm>

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	//TODO:	add entities from m_entitiesToAdd to the proper location
	//		-add them to vector of all entities
	//		-add them to the vector inside the map, with the tag as a key

	for (auto& a : m_entitiesToAdd)
	{
		m_entities.push_back(a);
		m_entityMap[a->m_tag].push_back(a);
	}

	m_entitiesToAdd.clear();

	removeDeadEntites(m_entities);

	//remove dead entities from each vector in the entity map
	//c++20 way off iterating through [key,value] pairs in a map
	for (auto& [tag, entityVec] : m_entityMap)
	{
		removeDeadEntites(entityVec);
	}
}

void EntityManager::removeDeadEntites(EntityVec& vec)
{
	//TODO:	remove all dead entities from the input vector
	//		this is called by update() funtion

	auto n_end = std::remove_if(vec.begin(), vec.end(),
		[](std::shared_ptr<Entity> & a)
		{
			return (!a->isActive());
		}

	);

	vec.erase(n_end, vec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));

	m_entitiesToAdd.push_back(entity);

	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	//TODO:	this is incorrect, return the correct vector from the map
	return m_entityMap[tag];
}

const std::map< std::string, EntityVec>& EntityManager::getEntityMap()
{
	return m_entityMap;
}