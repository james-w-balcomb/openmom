#include "YAML.h"

#include <iostream>

#include "yaml-cpp/yaml.h"
#include "Platform.h"

#include "Data.h"
#include "GfxData.h"
#include "Localization.h"

#include "UnitSpec.h"
#include "Skill.h"
#include "Effects.h"

#define PARSE_ERROR(x, ...) do { LOGD("[yaml] parse error: " x, __VA_ARGS__); } while (false)

#define FETCH_OR_FAIL(name, map, n) do { \
  auto it = map.find(n.asString()); \
  if (it != map.end()) return it->second; \
  PARSE_ERROR("mapped value '%s' not found for %s", n.asString().c_str(), name); \
  assert(false); \
  } while(false)

using namespace YAML;
using namespace yaml;

class yaml::Node : public YAML::Node
{
  
public:
  Node(const YAML::Node& node) : YAML::Node(node) { }
  
  template<typename T> void keyNotFoundError(const T& key) const;
  
  template<typename T> const
  YAML::Node operator[](const T& key) const
  {
    auto node = YAML::Node::operator[](key);
    
    if (node.IsDefined())
      return node;
    else
    {
      keyNotFoundError(key);
      assert(false);
    }
  }
};

template<> void yaml::Node::keyNotFoundError(const int& key) const
{
  PARSE_ERROR("index '%d' not found", key);
}

template<typename T> void yaml::Node::keyNotFoundError(const T& key) const
{
  PARSE_ERROR("key '%s' not found", key);
}

using NNN = YAML::Node;

bool operator==(const N& n, const std::string& string) { return n.Scalar() == string; }

N yaml::parse(const std::string& fileName)
{
  return N(YAML::LoadFile(yamlPath(fileName)));
}

std::string yaml::yamlPath(const std::string& fileName)
{
  return Platform::instance()->getResourcePath() + "/data/yaml/" + fileName;
}

template<> s16 yaml::parse(const N& node) { return node.operator s16(); }
template<> u16 yaml::parse(const N& node) { return node.operator u16(); }
template<> bool yaml::parse(const N& node) { return node.as<bool>(); }

template<typename T, typename std::enable_if<!std::is_pointer<T>::value, int>::type> T yaml::parse(const N& node)
{
  T value;
  parse(node, value);
  return value;
}

/*template<typename T, typename std::enable_if<!std::is_pointer<T>::value, int>::type> T yaml::parse(const N& node)
{
  return node.as<T>();
}*/

template<> LBXID yaml::parse(const N& node)
{
  static const std::unordered_map<std::string, LBXID> mapping = {
    { "figures1", LBXID::FIGURES1 },
    { "figures3", LBXID::FIGURES3 },
    { "figures4", LBXID::FIGURES4 },
    { "figure11", LBXID::FIGURE11 },
    
    { "monster", LBXID::MONSTER },
    { "portrait", LBXID::PORTRAIT },
    
    { "special", LBXID::SPECIAL },
    { "special2", LBXID::SPECIAL2 },
    
    { "units1", LBXID::UNITS1 },
    { "units2", LBXID::UNITS2 },


  };
  
  FETCH_OR_FAIL("LBXID", mapping, node);
}

template<> School yaml::parse(const N& node)
{
  static const std::unordered_map<std::string, School> mapping = {
    { "arcane", School::ARCANE },
    { "nature", School::NATURE },
    { "sorcery", School::SORCERY },
    { "chaos", School::CHAOS },
    { "life", School::LIFE },
    { "death", School::DEATH }
  };
  
  FETCH_OR_FAIL("School", mapping, node);
}

template<> Property yaml::parse(const N& node)
{
  static const std::unordered_map<std::string, Property> mapping = {
    { "to_hit", Property::TO_HIT },
    { "melee", Property::MELEE },
    { "defense", Property::SHIELDS },
    { "defense_ranged", Property::SHIELDS_RANGED },
    { "defense_chaos", Property::SHIELDS_CHAOS },
    { "defense_sorcery", Property::SHIELDS_SORCERY },
    { "defense_nature", Property::SHIELDS_NATURE },
    { "defense_life", Property::SHIELDS_LIFE },
    { "defense_death", Property::SHIELDS_DEATH },
    { "resistance", Property::RESIST },
    { "hits", Property::HIT_POINTS },
    { "figures", Property::FIGURES }
  };
  
  FETCH_OR_FAIL("Property", mapping, node);
}

template<> skills::Type yaml::parse(const N& node)
{
  if (node == "native")
    return skills::Type::NATIVE;
  else
    assert(false);
}

template<typename T> void yaml::parse(const N& node, std::vector<T>& dest)
{
  assert(node.IsSequence());
  //dest.reserve(node.size());
  for (const auto& entry : node)
    dest.push_back(parse<T>(entry));
}

template<typename T> T yaml::optionalParse(const N& node, T def)
{
  return node.IsDefined() ? parse<T>(node) : def;
}

template<> void yaml::parse(const N& node, SpriteInfo& v)
{
  assert(node.IsSequence());
  if (node[0] == "lbx")
    v = SpriteInfo(parse<LBXID>(node[1]), node[2]);
  else
    assert(false);
}

template<> const SkillEffect* yaml::parse(const N& node)
{
  const std::string& type = node["type"];
  
  if (type == "unit_bonus")
  {
    Property property = parse<Property>(node["property"]);
    s16 value = parse<s16>(node["value"]);
    return new UnitBonus(property, value);
  }
  else
  {
    assert(false);
    return nullptr;
  }
}

template<> const Skill* yaml::parse(const N& node)
{
  skills::Type type = optionalParse(node["type"], skills::Type::NATIVE);
  
  effect_list effects;
  parse(node["effects"], effects);
  
  auto visuals = node["visuals"];
  
  skills::VisualInfo visualInfo;
  visualInfo.name = i18n::keyForString(visuals["i18n"]);
  visualInfo.hideValue =  optionalParse(visuals["hideValue"], false);
  visualInfo.icon = parse<SpriteInfo>(visuals["icon"]);
  
  return new skills::ConcreteSkill(type, effects, visualInfo);
}

template<> const UnitSpec* yaml::parse(const N& node)
{
  assert(node.IsMap());
  
  const UnitSpec* spec = nullptr;
  
  const std::string& ident = node["identifier"];
  const std::string& type = node["type"];
  
  /* common properties */
  s16 melee = node["melee"];
  s16 defense = node["defense"];
  s16 resistance = node["resistance"];
  s16 hits = node["hits"];
  s16 figures = node["figures"];
  s16 movement = node["movement"];
  s16 sight = node["sight"];
  s16 upkeep = node["upkeep"];
  s16 cost = node["cost"];

  
  
  SpriteInfo gfxIcon = parse<SpriteInfo>(node["visuals"]["icon"]);
  SpriteInfo gfxFigure = parse<SpriteInfo>(node["visuals"]["figure"]);
  I18 gfxName = i18n::keyForString(node["visuals"]["i18n"]);
  bool gfxIsFlying = optionalParse(node["visuals"]["is_flying"], false);
  
  if (type == "racial")
  {
    const Race* race = Data::get<const Race*>(node["race"]);
    
    spec = new RaceUnitSpec(
                            race,
                            upkeep,
                            cost,
                            melee,
                            0, // TODO
                            Ranged::NONE, // TODO
                            0, // TODO
                            defense,
                            resistance,
                            hits,
                            figures,
                            movement,
                            sight,
                            { } // TODO
    );
  }
  else if (type == "hero")
  {
    SpriteInfo gfxPortrait = parse<SpriteInfo>(node["visuals"]["portrait"]);
    
  }
  else if (type == "fantastic")
  {
    School school = parse<School>(node["school"]);
    
    
    SpriteInfo gfxSummon = parse<SpriteInfo>(node["visuals"]["summon"]);
  }
  
  return spec;
}

void yaml::parseSkills()
{
  N file = parse("skills.yaml");
  auto skills = file["skills"];
  
  for (const auto& yskill : skills)
  {
    const std::string& identifier = yskill["identifier"];
    const Skill* skill = parse<const Skill*>(yskill);
    Data::registerData(identifier, skill);
  }
}

void yaml::parseUnits()
{
  N file = parse("units.yaml");
  auto units = file["units"];
  
  for (const auto& yunit : units)
  {
    const std::string& identifier = yunit["identifier"];
    const UnitSpec* unit = parse<const UnitSpec*>(yunit);
  }

}

void yaml::parseLocalization()
{
  N file = parse("english.yaml");
  auto entries = file["entries"];
  
  for (const auto entry : entries)
  {
    for (const auto& map : entry)
    {
      const std::string& key = map.first;
      const std::string& value = map.second;
      i18n::mapCustomEntry(key, value);
    }
  }
}

void yaml::parse()
{
  parseLocalization();
  parseSkills();
  parseUnits();
  
  Data::getInfo<const Skill*>();
}
