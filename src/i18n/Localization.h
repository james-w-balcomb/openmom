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

enum I18 : u16
{
  I18_RACE_NAME_BARBARIANS,
  I18_RACE_NAME_BEASTMEN,
  I18_RACE_NAME_DARK_ELVES,
  I18_RACE_NAME_DRACONIANS,
  I18_RACE_NAME_DWARVES,
  I18_RACE_NAME_GNOLLS,
  I18_RACE_NAME_HALFLINGS,
  I18_RACE_NAME_HIGH_ELVES,
  I18_RACE_NAME_HIGH_MEN,
  I18_RACE_NAME_KLACKONS,
  I18_RACE_NAME_LIZARDMEN,
  I18_RACE_NAME_NOMADS,
  I18_RACE_NAME_ORCS,
  I18_RACE_NAME_TROLLS,
  
  I18_UNIT_NAME_BARBARIANS,
  I18_UNIT_NAME_BEASTMEN,
  I18_UNIT_NAME_DARK_ELVES,
  I18_UNIT_NAME_DRACONIANS,
  I18_UNIT_NAME_DWARVES,
  I18_UNIT_NAME_GNOLLS,
  I18_UNIT_NAME_HALFLINGS,
  I18_UNIT_NAME_HIGH_ELVES,
  I18_UNIT_NAME_HIGH_MEN,
  I18_UNIT_NAME_KLACKONS,
  I18_UNIT_NAME_LIZARDMEN,
  I18_UNIT_NAME_NOMADS,
  I18_UNIT_NAME_ORCS,
  I18_UNIT_NAME_TROLLS
};

namespace std
{
  template<>
  struct hash<I18>
  {
    std::size_t operator()(const I18& k) const { return k; }
  };
}

class i18n
{
  private:
    static std::unordered_map<I18, const std::string> data;
  
  public:
    static void load();
    static const std::string& s(I18 ident) { return data[ident]; }
};


#endif
