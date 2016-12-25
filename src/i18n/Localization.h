//
//  Localization.h
//  OpenMoM
//
//  Created by Jack on 7/17/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _LOCALIZATION_H_
#define _LOCALIZATION_H_

#include "Common.h"

#include <unordered_map>
#include <string>

enum class UnitID : u16;
enum class RaceID : u8;
enum class SkillBase : u16;

enum class I18 : u16
{
  CITY_HAMLET,
  CITY_VILLAGE,
  CITY_TOWN,
  CITY_CITY,
  CITY_CAPITOL,

  
  MESSAGE_NEW_BUILDING,
  MESSAGE_OUTPOST_GROWN_TO_CITY,
  
  SURVEYOR_CITY_FORBID_DISTANCE,
  SURVEYOR_CITY_FORBID_WATER,
  SURVEYOR_MAX_POPULATION,
  SURVEYOR_PRODUCTION_BONUS,
  
  WIZARD_NAME_MERLIN,
  WIZARD_NAME_RAVEN,
  WIZARD_NAME_SHAREE,
  WIZARD_NAME_LO_PAN,
  WIZARD_NAME_JAFAR,
  WIZARD_NAME_OBERIC,
  WIZARD_NAME_RJAK,
  WIZARD_NAME_SSS_RA,
  WIZARD_NAME_TAURON,
  WIZARD_NAME_FREYA,
  WIZARD_NAME_HORUS,
  WIZARD_NAME_ARIEL,
  WIZARD_NAME_TLALOC,
  WIZARD_NAME_KALI,
  
  BUILDING_BUILDERS_HALL,
  BUILDING_SMITHY,
  BUILDING_CITY_WALLS,
  BUILDING_SHRINE,
  BUILDING_TEMPLE,
  BUILDING_PARTHENON,
  BUILDING_CATHEDRAL,
  BUILDING_ANIMISTS_GUILD,
  BUILDING_ORACLE,
  BUILDING_ALCHEMISTS_GUILD,
  BUILDING_WIZARDS_GUILD,
  BUILDING_STABLE,
  BUILDING_FANTASTIC_STABLE,
  BUILDING_BARRACKS,
  BUILDING_ARMORY,
  BUILDING_FIGHTERS_GUILD,
  BUILDING_ARMORERS_GUILD,
  BUILDING_WAR_COLLEGE,
  BUILDING_SHIP_WRIGHTS_GUILD,
  BUILDING_SHIP_YARD,
  BUILDING_MARITIME_GUILD,
  BUILDING_MARKETPLACE,
  BUILDING_BANK,
  BUILDING_MERCHANTS_GUILD,
  BUILDING_GRANARY,
  BUILDING_FARMERS_MARKET,
  BUILDING_LIBRARY,
  BUILDING_UNIVERSITY,
  BUILDING_SAGES_GUILD,
  BUILDING_MINERS_GUILD,
  BUILDING_MECHANICIANS_GUILD,
  BUILDING_SAWMILL,
  BUILDING_FORESTERS_GUILD,
  BUILDING_TRADE_GOODS,
  BUILDING_HOUSING,
  BUILDING_MAGE_FORTRESS,
  BUILDING_SUMMONING_CIRCLE,
  
  BUILDING_DESC_BUILDERS_HALL,
  BUILDING_DESC_SMITHY,
  BUILDING_DESC_CITY_WALLS,
  BUILDING_DESC_SHRINE,
  BUILDING_DESC_TEMPLE,
  BUILDING_DESC_PARTHENON,
  BUILDING_DESC_CATHEDRAL,
  BUILDING_DESC_ANIMISTS_GUILD,
  BUILDING_DESC_ORACLE,
  BUILDING_DESC_ALCHEMISTS_GUILD,
  BUILDING_DESC_WIZARDS_GUILD,
  BUILDING_DESC_STABLE,
  BUILDING_DESC_FANTASTIC_STABLE,
  BUILDING_DESC_BARRACKS,
  BUILDING_DESC_ARMORY,
  BUILDING_DESC_FIGHTERS_GUILD,
  BUILDING_DESC_ARMORERS_GUILD,
  BUILDING_DESC_WAR_COLLEGE,
  BUILDING_DESC_SHIP_WRIGHTS_GUILD,
  BUILDING_DESC_SHIP_YARD,
  BUILDING_DESC_MARITIME_GUILD,
  BUILDING_DESC_MARKETPLACE,
  BUILDING_DESC_BANK,
  BUILDING_DESC_MERCHANTS_GUILD,
  BUILDING_DESC_GRANARY,
  BUILDING_DESC_FARMERS_MARKET,
  BUILDING_DESC_LIBRARY,
  BUILDING_DESC_UNIVERSITY,
  BUILDING_DESC_SAGES_GUILD,
  BUILDING_DESC_MINERS_GUILD,
  BUILDING_DESC_MECHANICIANS_GUILD,
  BUILDING_DESC_SAWMILL,
  BUILDING_DESC_FORESTERS_GUILD,
  BUILDING_DESC_TRADE_GOODS,
  BUILDING_DESC_HOUSING,

  
  UNIT_SPEARMEN,
  UNIT_SWORDSMEN,
  UNIT_BOWMEN,
  UNIT_CAVALRY,
  UNIT_SHAMAN,
  UNIT_HALBERDIERS,
  UNIT_PRIESTS,
  UNIT_MAGICIANS,
  UNIT_ENGINEERS,
  UNIT_SETTLERS,
  
  UNIT_BERSERKERS,
  
  HERO_DWARF,
  HERO_ORC_WARRIOR,
  
  UNIT_MAGIC_SPIRIT,
  UNIT_HELL_HOUNDS,
  UNIT_GREAT_DRAKE,
  
  SPELL_KIND_SUMMONING,
  SPELL_KIND_SPECIAL,
  SPELL_KIND_CITY,
  SPELL_KIND_ENCHANTMENT,
  SPELL_KIND_UNIT_SPELL,
  SPELL_KIND_COMBAT_SPELL,
  SPELL_KIND_RESEARCH, // used for BookView
  SPELL_KIND_SPELLS,
  
  SPELL_BLESS,
  SPELL_ENDURANCE,
  SPELL_GUARDIAN_SPIRIT,
  SPELL_HEALING,
  SPELL_HEROISM,
  SPELL_HOLY_ARMOR,
  SPELL_HOLY_WEAPON,
  SPELL_JUST_CAUSE,
  SPELL_STAR_FIRES,
  SPELL_TRUE_LIGHT,
  
  SPELL_CORRUPTION,
  SPELL_CHANGE_TERRAIN,
  SPELL_RAISE_VOLCANO,
  
  EVENT_BAD_MOON,
  EVENT_RED_CONJUNCTION,
  EVENT_GREEN_CONJUNCTION,
  EVENT_BLUE_CONJUNCTION,
  EVENT_DEPLETION,
  EVENT_DIPLOMATIC_MARRIAGE,
  EVENT_DISJUNCTION,
  EVENT_DONATION,
  EVENT_EARTHQUAKE,
  EVENT_THE_GIFT,
  EVENT_GOOD_MOON,
  EVENT_GREAT_METEOR,
  EVENT_MANA_SHORT,
  EVENT_NEW_MINERALS,
  EVENT_PIRACY,
  EVENT_PLAGUE,
  EVENT_POPULATION_BOOM,
  EVENT_REBELLION,
  
  ITEM_SWORD,
  ITEM_MACE,
  ITEM_AXE,
  ITEM_BOW,
  ITEM_STAFF,
  ITEM_WAND,
  ITEM_AMULET,
  ITEM_RING,
  ITEM_CLOAK,
  ITEM_GAUNTLET,
  ITEM_HELM,
  ITEM_ORB,
  ITEM_SHIELD,
  ITEM_CHAIN_MAIL,
  ITEM_PLATE_MAIL,
  
  RELATION_HATRED,
  RELATION_TROUBLED,
  RELATION_TENSE,
  RELATION_RESTLESS,
  RELATION_UNEASY,
  RELATION_NEUTRAL,
  RELATION_RELAXED,
  RELATION_CALM,
  RELATION_PEACEFUL,
  RELATION_FRIENDLY,
  RELATION_HARMONY,
  
  UI_UNIT_DETAIL_MOVES,
  UI_UNIT_DETAIL_UPKEEP,
  
  EMPTY,
  
  PLACEHOLDER
};

namespace lbx { class LBX; }

class i18n
{
public:
  struct race_names
  {
    std::string name;
    std::string unitName;
  };

  private:
    static std::unordered_map<I18, std::string, enum_hash> data;
    static std::unordered_map<UnitID, std::string, enum_hash> units;
    static std::unordered_map<RaceID, race_names, enum_hash> races;
    static std::unordered_map<SkillBase, std::string, enum_hash> skills;
    static std::unordered_map<TileType, std::vector<std::string>, enum_hash> surveyorDescs;
  
  public:
    static const char* c(I18 ident) { return data[ident].c_str(); }
    static const std::string& s(I18 ident) { return data[ident]; }
    static const std::string& s(UnitID unit) { return units[unit]; }
    static const race_names& s(RaceID race) { return races[race]; }
    static const std::string& s(SkillBase skill) { return skills[skill]; }
  
    static const std::vector<std::string> surveyorDesc(TileType type) { return surveyorDescs[type]; }

    static constexpr const I18 CITY_SIZE_NAMES[] { I18::CITY_HAMLET, I18::CITY_VILLAGE, I18::CITY_TOWN, I18::CITY_CITY, I18::CITY_CAPITOL };
    static constexpr I18 SPELL_KIND_NAMES[] = { I18::SPELL_KIND_SUMMONING, I18::SPELL_KIND_SPECIAL, I18::SPELL_KIND_CITY, I18::SPELL_KIND_ENCHANTMENT, I18::SPELL_KIND_UNIT_SPELL, I18::SPELL_KIND_COMBAT_SPELL };
  
  friend class lbx::LBX;
};


#endif
