//
//  UnitDraw.cpp
//  OpenMoM
//
//  Created by Jack on 7/26/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#include "UnitDraw.h"

#include "Gfx.h"
#include "GfxData.h"
#include "ColorMap.h"
#include "Texture.h"
#include "Army.h"
#include "Unit.h"
#include "Player.h"

#include "LocalPlayer.h"
#include "Game.h"

/* color map of colors that change according to player color
   these should be correct because they've been hand picked from the game
*/

/*
 green
   indices: 216 217 218
   colors: { 0, 188, 0 } { 0, 164, 0 } { 0, 124 0, }

 yellow
   indices: 162 178 180
   colors: { 252 200 36 } { 236 164 36 } { 204 136 024 }

 red
   indices: 203 166 45
   colors: { 172 0 0 } { 152 0 0 } { 116 36 36 }

 purple
   indices: 123 124 125
   colors: { 164 084 164 } { 140 056 140 } { 116 036 116 }
 
 blue
   indices: 107 108 109
   colors: { 84 84 164 } { 60 56 140 } { 36 36 116 }
 
 brown
   indices: 50 51 52
   colors: { 188 152 116 } { 164 124 84 } { 140 96 56 }
 
*/

constexpr Color INVISBLE_UNIT_COLOR = Color(255, 255, 255, 100);


void UnitDraw::bindPlayerColorPalette(PlayerColor color)
{
  const Palette* palette = nullptr;
  
  switch (color)
  {
    case PlayerColor::YELLOW:
    {
      static const DerivedPalette dpalette = DerivedPalette(Gfx::mainPalette, 216, 3, { Gfx::mainPalette->get(162), Gfx::mainPalette->get(178), Gfx::mainPalette->get(180) });
      palette = &dpalette;
      break;
    }
    case PlayerColor::RED:
    {
      static const DerivedPalette dpalette = DerivedPalette(Gfx::mainPalette, 216, 3, { Gfx::mainPalette->get(203), Gfx::mainPalette->get(166), Gfx::mainPalette->get(45) });
      palette = &dpalette;
      break;
    }
    case PlayerColor::PURPLE:
    {
      static const DerivedPalette dpalette = DerivedPalette(Gfx::mainPalette, 216, 3, { Gfx::mainPalette->get(123), Gfx::mainPalette->get(124), Gfx::mainPalette->get(125) });
      palette = &dpalette;
      break;
    }
    case PlayerColor::BLUE:
    {
      static const DerivedPalette dpalette = DerivedPalette(Gfx::mainPalette, 216, 3, { Gfx::mainPalette->get(107), Gfx::mainPalette->get(108), Gfx::mainPalette->get(109) });
      palette = &dpalette;
      break;
    }
    case PlayerColor::NEUTRAL:
    {
      static const DerivedPalette dpalette = DerivedPalette(Gfx::mainPalette, 216, 3, { Gfx::mainPalette->get(50), Gfx::mainPalette->get(51), Gfx::mainPalette->get(52) });
      palette = &dpalette;
      break;
    }
    case PlayerColor::GREEN:
    {
      /* nothing to do */
      break;
    }
  }

  if (palette)
    Gfx::bindPalette(palette);
}

void UnitDraw::unbindPlayerColorPalette()
{
  Gfx::unbindPalette();
}

bool UnitDraw::isInvisible(const UnitSpec* spec, const Unit* unit)
{
  return (unit && unit->skills()->hasSimpleEffect(SimpleEffect::Type::INVISIBILITY)) || (!unit && spec && spec->skills.hasSimpleEffect(SimpleEffect::Type::INVISIBILITY));
}

std::string UnitDraw::stringForDoubleMovement(s16 moves, bool hideZero)
{
  if (hideZero && moves == 1)
    return ".5";
  else
    return fmt::sprintf("%d%s",moves/2, moves%2 == 0 ? "" : ".5");
}

Color UnitDraw::colorForHealth(float percent)
{
  static const Color greenColor = Color(0,172,0);
  static const Color yellowColor = Color(238,210,0);
  static const Color redColor = Color(172,0,0);
  
  if (percent > 0.8f)
    return greenColor;
  else if (percent > 0.5)
  {
    float dx = (0.8f - percent) / 0.3f;
    return greenColor.blend(yellowColor, dx);
  }
  else if (percent > 0.2f)
  {
    float dx = (0.5f - percent) / 0.3f;
    return yellowColor.blend(redColor, dx);
  }
  else
    return redColor;
}

void UnitDraw::drawHealthBar(u16 x, u16 y, u16 width, float health)
{
  /* draw hits */
  const int BAR_LENGTH = width;
  int currentBarLength = health*BAR_LENGTH;
  Gfx::drawLine(UnitDraw::colorForHealth(health), x, y, x + currentBarLength, y);
  Gfx::drawLine({0,0,0,120}, x + currentBarLength, y, x + BAR_LENGTH, y);
}

void UnitDraw::drawStatic(const Army *army, s16 x, s16 y, bool forceDraw)
{
  if (forceDraw || (army != LocalGame::i->currentPlayer()->getSelectedArmy()) || (Gfx::fticks % 6) < 3)
  {
    Gfx::draw(GfxData::playerGfxSpec(army->getOwner()->color).unitBack, x, y);
    
    const Unit* first = nullptr;
    if (army == LocalGame::i->currentPlayer()->getSelectedArmy())
    {
      for (auto u : *army)
        if (LocalGame::i->currentPlayer()->isSelectedUnit(u))
        {
          first = u;
          break;
        }
    }
    else
      first = army->get(0);
    
    drawStatic(first, x, y, false, army->isPatrolling());
  }
}

void UnitDraw::drawStatic(const Unit* unit, s16 x, s16 y, bool backdrop, bool grayScale)
{
  if (backdrop)
    Gfx::draw(GfxData::playerGfxSpec(unit->getArmy()->getOwner()->color).unitBack, x, y);
  
  bindPlayerColorPalette(unit->getArmy()->getOwner()->color);
  
  const SpriteInfo& info = GfxData::unitGfx(unit->spec).still;
  
  /* if unit is invisibile bind buffer to draw unit on it so that
      we can make it transparent afterwards through mergeBuffer
   */
  if (isInvisible(nullptr, unit))
  {
    Gfx::drawSolidOutline(info, x+1, y+1, Gfx::mainPalette->get(1));
    Gfx::bindBuffer();
    Gfx::resetBuffer(info.sw(), info.sh(), x+1, y+1);
  }
  
  if (grayScale)
    Gfx::drawGrayScale(info, x+1, y+1);
  else
    Gfx::draw(info, x+1, y+1);
  
  if (isInvisible(nullptr, unit))
  {
    rgb_filters::ConstantColor filter(INVISBLE_UNIT_COLOR);
    Gfx::mergeBuffer(x+1, y+1, x+1, y+1, info.sw(), info.sh(), &filter);
    Gfx::bindCanvas();
  }
  
  unbindPlayerColorPalette();
  
  School school = unit->glow();
  if (school != NO_SCHOOL)
    Gfx::drawGlow(info, x+1, y+1, school);
  
}

void UnitDraw::drawHeroPortrait(const Hero *unit, s16 x, s16 y)
{
  Gfx::draw(GfxData::unitGfx(unit->spec).hero.portrait, x, y);
}

using IsoOffset = Point;

static const IsoOffset ISO_1FIGURES[] = {{0,0}};
static const IsoOffset ISO_2FIGURES[] = {{-8,+1},{+8,+1}};
// TODO: check order, it should match the ingame one when an unit is damaged and less than all the units are drawn
static const IsoOffset ISO_4FIGURES[] = {{+2,-4},{+11,+1},{-8,+1},{+2,+6}};
static const IsoOffset ISO_6FIGURES[] = {{+1,-5},{+4,-1},{+10,+2},{-10,0},{-3,+3},{+1,+7}};
static const IsoOffset ISO_8FIGURES[] = {{+1,-5},{+7,-2},{+11,0},{-2,-1},{+4,+1},{-10,0},{-4,+3},{+2,+6}};

static const IsoOffset ISOC_1FIGURES[] = {{1,2}}; //TODO 2 figures missing
static const IsoOffset ISOC_4FIGURES[] = {{2,-4},{11,1},{-8,1,},{2,6}};
static const IsoOffset ISOC_6FIGURES[] = {{1,-5},{4,-1},{10,2},{-10,0},{-3,3},{1,7}};
static const IsoOffset ISOC_8FIGURES[] = {{1,-5},{7,-2},{11,0},{-2,-1},{4,1},{-10,0},{-4,3},{2,6}};

static const IsoOffset* ISOC[] = {ISOC_1FIGURES, nullptr, nullptr, ISOC_4FIGURES, nullptr, ISOC_6FIGURES, nullptr, ISOC_8FIGURES };
static const IsoOffset* ISO[] = {ISO_1FIGURES, ISO_2FIGURES, nullptr, ISOC_4FIGURES, nullptr, ISOC_6FIGURES, nullptr, ISOC_8FIGURES };

const IsoOffset* UnitDraw::offsetForFigures(bool combat, u32 figures)
{
  assert(figures >= 1 && figures <= 8);
  const IsoOffset* offsets = (combat ? ISOC : ISO)[figures-1];
  assert(offsets);
  return offsets;
}

void UnitDraw::drawUnitIso(const UnitSpec *unit, s16 x, s16 y, const Unit *realUnit, const Player* owner)
{
  
  if (realUnit && (realUnit->skills()->has(MovementType::SWIMMING) || realUnit->skills()->has(MovementType::SAILING)))
    Gfx::draw(LSI(UNITVIEW, 34), x+1, y+13);
  else
    Gfx::drawClipped(TSI(UNIT_DETAIL_SPECIAL_THINGS,0,0), x, y, 0, 0, 40, 33);

  x += 6;
  
  const IsoOffset* o = offsetForFigures(false, unit->figures);
  
  // TODO: bind color map flags missing, probably sohuld be passed by production view since it could come from a city (so no realUnit)

  School glow = School::NO_SCHOOL;
  
  if (realUnit)
    glow = realUnit->glow();
  
  const auto& unitGfx = GfxData::unitGfx(unit);
  const SpriteInfo sprite = unitGfx.fullFigure.frame(2, 2);
  
  for (int i = 0; i < unit->figures; ++i)
  {
    const u16 fx = x + o[i].x, fy = y + o[i].y;
    
    if (realUnit)
      bindPlayerColorPalette(realUnit->getArmy()->getOwner()->color);
    else if (owner)
      bindPlayerColorPalette(owner->color);
    
    if (isInvisible(unit, realUnit))
    {
      Gfx::bindBuffer();
      Gfx::resetBuffer(sprite.sw(), sprite.sh(), fx, fy);
    }

    Gfx::draw(sprite, fx, fy);
    
    if (isInvisible(unit, realUnit))
    {
      rgb_filters::ConstantColor filter(INVISBLE_UNIT_COLOR);
      Gfx::mergeBuffer(x+1, y+1, x+1, y+1, sprite.sw(), sprite.sh(), &filter);
      Gfx::bindCanvas();
    }
    
    if (realUnit || owner)
      unbindPlayerColorPalette();
    
    if (glow != School::NO_SCHOOL)
      Gfx::drawGlow(sprite, fx, fy, glow);
    
  }
}

void UnitDraw::drawUnitIsoCombat(const Unit *unit, s16 x, s16 y, Dir facing, CombatAction caction)
{
  int action = 1;
  
  if (caction == CombatAction::MOVE || (caction == CombatAction::STAY && GfxData::unitGfx(unit->spec).isFlyingFigure))
  {
    if (Gfx::fticks%6 < 2)
      action = 0;
    else if (Gfx::fticks%6 < 4)
      action = 1;
    else
      action = 2;
  }
  else if (caction == CombatAction::ATTACK)
  {
    if (Gfx::fticks%2 == 0)
      action = 1;
    else if (Gfx::fticks%2 == 1)
      action = 3;
  }
  

  SpriteInfo sprite = GfxData::unitGfx(unit->spec).fullFigure.relative(static_cast<u8>(facing));
  
  School glow = unit->glow();
  
  const IsoOffset* offsets = offsetForFigures(true, unit->spec->figures);
  
  for (int i = 0; i < unit->getProperty(Property::ALIVE_FIGURES); ++i)
  {
    u32 fx = x + offsets[i].x, fy = y + offsets[i].y;
    
    bindPlayerColorPalette(unit->getArmy()->getOwner()->color);
    
    if (isInvisible(nullptr, unit))
    {
      Gfx::bindBuffer();
      Gfx::resetBuffer(sprite.sw(), sprite.sh(), fx, fy);
    }
    
    Gfx::draw(sprite.frame(action), fx, fy);
    
    if (isInvisible(nullptr, unit))
    {
      rgb_filters::ConstantColor filter(INVISBLE_UNIT_COLOR);
      Gfx::mergeBuffer(x+1, y+1, x+1, y+1, sprite.sw(), sprite.sh(), &filter);
      Gfx::bindCanvas();
    }
    
    unbindPlayerColorPalette();
    
    if (glow != School::NO_SCHOOL)
      Gfx::drawGlow(sprite.frame(action), fx, fy, glow); // TODO: check if it works with new management
  }

}

void UnitDraw::drawUnitLevel(const Level* level, u16 x, u16 y, u16 spacing, bool shadowed)
{
  if (level)
  {
    u16 index = level->index();
    
    if (index > 0)
    {
      u16 count = ((index-1) % 3) + 1;
      u16 rank = (index-1) / 3;
      
      const SpriteInfo badge_lbx = shadowed ? LSI(COMPIX,76) : LSI(MAIN,51);

      for (int u = 0; u < count; ++u)
        Gfx::draw(badge_lbx.relative(rank), x + spacing*u, y);
    }
  }
}
