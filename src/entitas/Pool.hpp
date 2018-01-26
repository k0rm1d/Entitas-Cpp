// Copyright (c) 2016 Juan Delgado (JuDelCo)
// License: MIT License
// MIT License web page: https://opensource.org/licenses/MIT

#pragma once

#include "Entity.hpp"
#include "Group.hpp"

#include <functional>
#include <unordered_map>
#include <map>

namespace entitas
{
  class ISystem;

  class Pool
  {
  public:
    class EntitiesRetained
      : public std::runtime_error
    {
    public:
      EntitiesRetained(const int count);
    };

  public:
    Pool(const unsigned int startCreationIndex = 0,
         const bool reuseEntities = false);
    ~Pool();

    auto CreateEntity() -> EntityPtr;
    bool HasEntity(const EntityPtr& entity) const;
    void DestroyEntity(EntityPtr entity);
    void DestroyAllEntities();

    auto ReuseEntities() const -> bool;
    auto GetEntities() -> std::vector<EntityPtr>;
    auto GetEntities(const Matcher matcher) -> std::vector<EntityPtr>;
    auto GetGroup(Matcher matcher) -> std::shared_ptr<Group>;

    void ClearGroups();
    void ResetCreationIndex();
    void ClearComponentPool(const ComponentId index);
    void ClearComponentPools();
    void Reset();

    auto GetEntityCount() const -> unsigned int;
    auto GetReusableEntitiesCount() const -> unsigned int;
    auto GetRetainedEntitiesCount() const -> unsigned int;

    auto CreateSystem(std::shared_ptr<ISystem> system) -> std::shared_ptr<ISystem>;
    template <typename T> inline auto CreateSystem() -> std::shared_ptr<ISystem>;

    using PoolChanged = Delegate<void(Pool* pool, EntityPtr entity)>;
    using GroupChanged = Delegate<void(Pool* pool, std::shared_ptr<Group> group)>;

    PoolChanged OnEntityCreated;
    PoolChanged OnEntityWillBeDestroyed;
    PoolChanged OnEntityDestroyed;
    GroupChanged OnGroupCreated;
    GroupChanged OnGroupCleared;

  private:
    void UpdateGroupsComponentAddedOrRemoved(EntityPtr entity, ComponentId index, IComponent* component);
    void UpdateGroupsComponentReplaced(EntityPtr entity, ComponentId index, IComponent* previousComponent, IComponent* newComponent);
    void OnEntityReleased(Entity* entity);

    unsigned int mCreationIndex;
    bool mReuseEntities;
    std::unordered_set<EntityPtr> mEntities;
    std::unordered_map<Matcher, std::shared_ptr<Group>> mGroups;
    std::stack<Entity*> mReusableEntities;
    std::unordered_set<Entity*> mRetainedEntities;

    std::map<ComponentId, std::stack<IComponent*>> mComponentPools;
    std::map<ComponentId, std::vector<std::weak_ptr<Group>>> mGroupsForIndex;

    std::vector<EntityPtr> mEntitiesCache;
    std::function<void(Entity*)> mOnEntityReleasedCache;
  };

  template <typename T>
  auto Pool::CreateSystem() -> std::shared_ptr<ISystem>
  {
    return CreateSystem(std::dynamic_pointer_cast<ISystem>(std::shared_ptr<T>(new T())));
  }
}
