#pragma once

#include "common/Common.h"

#include <string>
#include <map>
#include <unordered_map>
#include <list>

#if defined(DEBUG)
#include <sstream>
#endif

class UnitSpec;
class RaceUnitSpec;
class Race;
class Building;
class Skill;
class Spell;
class Level;

using experience_levels = std::vector<std::unique_ptr<const Level>>;

class Data
{
public:
  using key_type = std::string;
  template<typename T> using map_t = std::unordered_map<key_type, T>;
  
  template<typename T>
  struct data_set
  {
    using iterator = typename map_t<T>::const_iterator;
    iterator begin;
    iterator end;
    size_t size;
  };
  
  using unit_dependency_map_t = std::unordered_map<const UnitSpec*, const Building*>;
  
  using skill_replacement_map_t = std::unordered_map<const Skill*, const Skill*>;
  
  static experience_levels normalUnitLevels, heroLevels;
  
private:
  template<typename T> static std::unordered_map<key_type, T>& containerFor();
  
  static unit_dependency_map_t unitDependsOnBuilding;
  
  static skill_replacement_map_t skillReplacementMap;
  
public:  
  template<typename T> static bool registerData(const key_type& ident, const T data)
  {
    auto& container = containerFor<T>();
    if (container.find(ident) != container.end())
      return false;
    
    container.insert(std::make_pair(ident, data));
    return true;
  }
  
  template<typename T> static const T get(const key_type& ident)
  {
    const auto& map = containerFor<T>();
    const auto it = map.find(ident);
    if (it != map.end())
      return it->second;
    else
    {
      LOGD("Data value not found for '%s' of type '%s'", ident.c_str(), nameForDataType<T>());
      assert(false);
    }
  }
  
  template<typename T> static void isValidKey(const key_type& ident)
  {
    const auto& map = containerFor<T>();
    const auto it = map.find(ident);
    LOGD("Identifier not found for %s", ident.c_str());
    assert(it != map.end());
  }
  
  static const experience_levels& experienceLevelsForUnits() { return normalUnitLevels; }
  static const experience_levels& experienceLevelsForHeroes() { return heroLevels; }
  
  static const Skill* skill(const key_type& ident) { return get<const Skill*>(ident); }
  static const Building* building(const key_type& ident) { return get<const Building*>(ident); }
  static const UnitSpec* unit(const key_type& ident) { return get<const UnitSpec*>(ident); }
  static const Spell* spell(const key_type& ident) { return get<const Spell*>(ident); }
  static const Race* race(const key_type& ident) { return get<const Race*>(ident); }
  static const Trait* trait(const key_type& ident) { return get<const Trait*>(ident); }
  static const Wizard* wizard(const key_type& ident) { return get<const Wizard*>(ident); }
  
  template <typename T> static data_set<T> values() {
    const auto& map = containerFor<T>();
    return { map.begin(), map.end(), map.size() };
  }
  
  static const std::unordered_map<key_type, const UnitSpec*> units();
  
  static std::vector<const RaceUnitSpec*> unitsForRace(const Race* race);
  
  static std::pair<unit_dependency_map_t::const_iterator, unit_dependency_map_t::const_iterator> requiredBuildingsForUnit(const UnitSpec* unit)
  {
    return unitDependsOnBuilding.equal_range(unit);
  }
  
#if defined(DEBUG)
  template<typename T> static const char* nameForDataType() { return "unnamed"; }
  
  template<typename T> static std::string getInfo(T data)
  {
    char buffer[128];
    sprintf(buffer, "%p", data);
    return buffer;
  }
  
  template<typename T> static void getInfo()
  {
    auto& container = containerFor<T>();
    LOGD("[data] size: %zu", container.size())
    for (const auto& entry : container)
      LOGD("  %s -> %s", entry.first.c_str(), getInfo(entry.second).c_str());
  }
#endif
  
  friend class yaml;
};
