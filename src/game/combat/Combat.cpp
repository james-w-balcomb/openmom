//
//  Combat.cpp
//  OpenMoM
//
//  Created by Jack on 7/22/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "Combat.h"

#include "Util.h"

#include "CombatMechanics.h"
#include "CombatMap.h"
#include "Player.h"

#include "Animations.h"

using namespace std;
using namespace combat;

Combat::Combat(Army* defender, Army* attacker, CombatMechanics* mechanics) : players{defender->getOwner(),attacker->getOwner()}, selectedUnit(nullptr), current(attacker->getOwner()), mechanics(mechanics), _map(new CombatMap(this, W,H))
{
  for (auto u1 : defender->getUnits())
  {
    CombatUnit* cu1 = new CombatUnit(Side::DEFENDER, u1);
    units[0].push_back(cu1);
    allUnits.push_back(cu1);
  }
  
  for (auto u2 : attacker->getUnits())
  {
    CombatUnit* cu2 = new CombatUnit(Side::ATTACKER, u2);
    units[1].push_back(cu2);
    allUnits.push_back(cu2);
  }
    
  current->combatTurnBegun();
}

Combat::~Combat()
{
  
}

void Combat::endTurn()
{
  deselect();
  current->combatTurnEnded();
  current = current == players[0] ? players[1] : players[0];
  current->combatTurnBegun();
  
  //TODO: check if (current instanceof LocalPlayer)
  //  current.game.nextLocal();
}

const CombatTile* Combat::tileAt(s16 x, s16 y) { return _map->tileAt(x,y); }

CombatUnit* Combat::unitAtTile(CombatCoord position) const
{
  auto it = std::find_if(allUnits.begin(), allUnits.end(), [&](const CombatUnit* unit) { return unit->position == position; });
  return it != allUnits.end() ? *it : nullptr;
}

void Combat::attack(CombatUnit *u1, CombatUnit *u2)
{
  u1->setFacing(relativeFacing(u1, u2));
  u2->setFacing(relativeFacing(u2, u1));
  
  //TODO: probably both animations should be pushed on both players
  u1->getOwner()->push(new anims::CombatAttack(u1));
  u2->getOwner()->push(new anims::CombatAttack(u2));
}

Dir Combat::relativeFacing(CombatUnit *u1, CombatUnit *u2)
{
  for (int i = 0; i < 8; ++i)
    if (u1->x()+CombatCoord::dirs(i,u1->y()%2 == 0)[0] == u2->x() && u1->y()+CombatCoord::dirs(i,u1->y()%2 == 0)[1] == u2->y())
      return static_cast<Dir>(i);
  
  return Dir::NORTH;
}

void Combat::deployUnits()
{
  // TODO
  
  auto it = units[0].begin();
  for (size_t i = 0; i < units[0].size(); ++i, ++it)
  {
    auto position = mechanics->positionForDeployedUnit(_map.get(), *it, i);
    (*it)->setPosition(position);
  }
  
  it = units[1].begin();
  for (size_t i = 0; i < units[1].size(); ++i, ++it)
  {
    auto position = mechanics->positionForDeployedUnit(_map.get(), *it, i);
    (*it)->setPosition(position);
  }
}

void Combat::moveUnit(CombatUnit *unit, CombatPosition position)
{
  unit->setPosition(position);
}
