#include <iostream>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "SDLHelper.h"
#include "Gfx.h"
#include "Texture.h"
#include "Game.h"
#include "ViewManager.h"

#include "Font.h"
#include "LBXRepository.h"
#include "Viewport.h"

#include "YAML.h"

#include "ui/views/UnitDetailView.h"

#include "yaml-cpp/node/detail/node_data.h"
std::string YAML::detail::node_data::empty_scalar;

void saveScreenshots();

void init()
{
  Game *game = new Game();
  game->dummyInit();

  //Math::seed(time(nullptr));
  //WorldGenerator gen = WorldGenerator(game->world);
  //gen.generate();
}

int main(int argc, char* arg[])
{
  //return 0;
  SDL::init();


  lbx::Repository::init();
  lbx::Repository::loadLBX(LBXID::ARMYLIST);
  lbx::Repository::loadLBX(LBXID::BACKGRND);
  lbx::Repository::loadLBX(LBXID::CITYSCAP);

  lbx::Repository::loadLBX(LBXID::CITYWALL);
  lbx::Repository::loadLBX(LBXID::CMBTCITY);
  lbx::Repository::loadLBX(LBXID::CMBDESRC);
  lbx::Repository::loadLBX(LBXID::CMBDESRT);
  lbx::Repository::loadLBX(LBXID::CMBGRASC);
  lbx::Repository::loadLBX(LBXID::CMBGRASS);
  lbx::Repository::loadLBX(LBXID::CMBMOUNC);
  lbx::Repository::loadLBX(LBXID::CMBMOUNT);
  lbx::Repository::loadLBX(LBXID::CMBTUNDR);
  lbx::Repository::loadLBX(LBXID::CMBTUNDC);
  lbx::Repository::loadLBX(LBXID::CMBMAGIC);
  lbx::Repository::loadLBX(LBXID::CMBTFX);
  lbx::Repository::loadLBX(LBXID::COMPIX);
  lbx::Repository::loadLBX(LBXID::CONQUEST);

  lbx::Repository::loadLBX(LBXID::DIPLOMAC);

  lbx::Repository::loadLBX(LBXID::FIGURES1);
  lbx::Repository::loadLBX(LBXID::FIGURES3);
  lbx::Repository::loadLBX(LBXID::FIGURES4);
  lbx::Repository::loadLBX(LBXID::FIGURES5);
  lbx::Repository::loadLBX(LBXID::FIGURES8);
  lbx::Repository::loadLBX(LBXID::FIGURES9);
  lbx::Repository::loadLBX(LBXID::FIGURE10);
  lbx::Repository::loadLBX(LBXID::FIGURE11);
  lbx::Repository::loadLBX(LBXID::FIGURE12);
  lbx::Repository::loadLBX(LBXID::FIGURE13);
  lbx::Repository::loadLBX(LBXID::FONTS);
  lbx::Repository::loadLBX(LBXID::HELP);
  lbx::Repository::loadLBX(LBXID::HIRE);
  lbx::Repository::loadLBX(LBXID::INTRO);
  lbx::Repository::loadLBX(LBXID::ITEMS);
  lbx::Repository::loadLBX(LBXID::ITEMISC);
  lbx::Repository::loadLBX(LBXID::LILWIZ);
  lbx::Repository::loadLBX(LBXID::LOAD);
  lbx::Repository::loadLBX(LBXID::MAGIC);
  lbx::Repository::loadLBX(LBXID::MAIN);
  lbx::Repository::loadLBX(LBXID::MAINSCRN);
  lbx::Repository::loadLBX(LBXID::MAPBACK);
  lbx::Repository::loadLBX(LBXID::MONSTER);
  lbx::Repository::loadLBX(LBXID::MOODWIZ);
  lbx::Repository::loadLBX(LBXID::NEWGAME);
  lbx::Repository::loadLBX(LBXID::PORTRAIT);
  lbx::Repository::loadLBX(LBXID::RELOAD);
  lbx::Repository::loadLBX(LBXID::RESOURCE);
  lbx::Repository::loadLBX(LBXID::SCROLL);
  lbx::Repository::loadLBX(LBXID::SPECFX);
  lbx::Repository::loadLBX(LBXID::SPECIAL);
  lbx::Repository::loadLBX(LBXID::SPECIAL2);
  lbx::Repository::loadLBX(LBXID::SPELLS);
  lbx::Repository::loadLBX(LBXID::SPELLSCR);
  lbx::Repository::loadLBX(LBXID::UNITVIEW);
  lbx::Repository::loadLBX(LBXID::UNITS1);
  lbx::Repository::loadLBX(LBXID::UNITS2);
  lbx::Repository::loadLBX(LBXID::VORTEX);
  lbx::Repository::loadLBX(LBXID::WIZLAB);
  lbx::Repository::loadLBX(LBXID::WIZARDS);

  yaml::parse();

  Viewport::createMapTextureAtlas();
  /*return 0;*/


  FontFaces::buildFonts();
  //return 0;

  //const Race &race = Race::race(RACE_BARBARIANS);

  //std::unordered_map<const void*, SpriteInfo> map;

  //const Race* ptRace = reinterpret_cast<const Race *>(&race);

  // init SDL

  init();

  Gfx::init();
  Texture::load();
  SDL::initGVM();

  //saveScreenshots();

  SDL::gvm->switchView(VIEW_MAIN);


  //SDL::gvm->switchView(VIEW_NEW_GAME);

  /*Hero* unit = new Hero(*UnitSpec::heroSpec(UnitID::HERO_DWARF));
  SDL::gvm->unitDetailView()->setHeroHire(unit, 200);*/
  //SDL::gvm->switchView(VIEW_NEW_GAME);

  //SDL::gvm->switchView(VIEW_DIPLOMACY);
  //SDL::gvm->switchView(VIEW_LBX);

  SDL::loop();

  Gfx::saveScreenshot("main-view.png");


  // clean up
  Texture::unload();
  Gfx::deinit();
  SDL::deinit();

  return 0;
}

#include "views/CityView.h"
#include "views/ArmyView.h"
#include "views/ProductionView.h"
#include "views/OutpostView.h"

void saveScreenshots()
{
  static const char* VIEW_NAMES[] = {
    "main", "spellbook", "magic", "research", "alchemy", "armies", "armies-items", "item-craft", "item-craft-charges", "merchant",
    "city", "unit", "army", "message", "cities", "mirror", "info-menu", "cartographer", "diplomacy", "astrologer", "historian",
    "combat", "production", "outpost", "new-game", "load", "options", "start", "intro"
  };

  Game* game = LocalGame::i->getGame();

  game->cityMechanics.updateProduction(game->getCities().front());

  SDL::gvm->cityView()->setCity(game->getCities().front());
  SDL::gvm->productionView()->setCity(game->getCities().front());
  SDL::gvm->outpostView()->setCity(game->getCities().front());
  SDL::gvm->unitDetailView()->setUnit(game->getPlayers().front()->getArmies().front()->get(0));
  SDL::gvm->armyView()->setArmy(game->getPlayers().front()->getArmies().front());
  
  for (ViewID view = VIEW_MAIN; view <= VIEW_INTRO; view = (ViewID)(view+1))
  {
    if (view == VIEW_MESSAGE)
      game->getPlayers().front()->send(new msgs::Error("This is a test error message."));

    SDL::gvm->switchView(view);
    SDL::render();
    SDL::handleEvents();

    std::string fileName = std::string("screenshot-") + VIEW_NAMES[view] + ".png";
    Gfx::saveScreenshot(fileName.c_str());

    if (view == VIEW_MESSAGE)
      static_cast<LocalPlayer*>(game->getPlayers().front())->clearMessages();
  }
}