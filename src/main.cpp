#include <iostream>

#include <SDL2/SDL.h>

#include "SDL.h"
#include "Gfx.h"
#include "Texture.h"

#include "Race.h"

#include <unordered_map>

#include "EventListener.h"

#include "Settings.h"

#include "Game.h"
#include "LocalPlayer.h"
#include "ViewManager.h"

#include "Localization.h"

void init()
{
  Game *game = new Game();
  LocalPlayer *player = new LocalPlayer(game);
  
  LocalGame *localGame = new LocalGame();
  localGame->currentPlayer = player;
  LocalGame::i = localGame;
}



int main(int argc, char * arg[])
{
  //const Race &race = Race::race(RACE_BARBARIANS);
  
  //std::unordered_map<const void*, SpriteInfo> map;
  //map[reinterpret_cast<const Race *>(&race)] = {MAIN_BACKDROP, 15,15};
  
  //const Race* ptRace = reinterpret_cast<const Race *>(&race);
  
  // init SDL
  
  i18n::load();
  
  init();
  
  SDL::init();
  Gfx::init();
  Texture::load();

  SDL::gvm->switchView(VIEW_NEW_GAME);
  
  SDL::loop();
  
  
  // wait 5 secs
  
  // clean up
  Texture::unload();
  Gfx::deinit();
  SDL::deinit();
  
  return 0;
}