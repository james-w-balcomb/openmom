#include "NewGameView.h"

#include "Font.h"
#include "Buttons.h"
#include "Gfx.h"

NewGameView::NewGameView() : View()
{
  buttons.resize(BUTTON_COUNT);
  
  buttons[OPTIONS_OK] = new OffsetButton("Ok", 252, 179, SpriteInfo(NEW_GAME_BUTTONS1, 0, 0));
  buttons[OPTIONS_CANCEL] = new OffsetButton("Cancel", 171, 179, SpriteInfo(NEW_GAME_BUTTONS1, 1, 0));
  
  buttons[OPTIONS_DIFFICULTY] = new LabeledSimpleButton("Difficulty", 251, 39, SpriteInfo(NEW_GAME_BUTTONS1, 2, 0), "", BROWN_MEDIUM_BOLD_SHADOW);
  buttons[OPTIONS_OPPONENTS] = new LabeledSimpleButton("Opponents", 251, 66, SpriteInfo(NEW_GAME_BUTTONS1, 3, 0), "", BROWN_MEDIUM_BOLD_SHADOW);
  buttons[OPTIONS_LAND_SIZE] = new LabeledSimpleButton("Land Size", 251, 93, SpriteInfo(NEW_GAME_BUTTONS1, 4, 0), "", BROWN_MEDIUM_BOLD_SHADOW);
  buttons[OPTIONS_MAGIC] = new LabeledSimpleButton("Magic", 251, 120, SpriteInfo(NEW_GAME_BUTTONS1, 5, 0), "", BROWN_MEDIUM_BOLD_SHADOW);
  
  buttons[OPTIONS_DIFFICULTY]->setAction([&](){ settings.group(SETTING_DIFFICULTY).next(); updateLabelsOptions(); });
  buttons[OPTIONS_OPPONENTS]->setAction([&](){ settings.group(SETTING_OPPONENTS).next(); updateLabelsOptions(); });
  buttons[OPTIONS_LAND_SIZE]->setAction([&](){ settings.group(SETTING_LAND_SIZE).next(); updateLabelsOptions(); });
  buttons[OPTIONS_MAGIC]->setAction([&](){ settings.group(SETTING_MAGIC_POWER).next(); updateLabelsOptions(); });
  
  updateLabelsOptions();
}

void NewGameView::updateLabelsOptions()
{
  static_cast<LabeledSimpleButton*>(buttons[OPTIONS_DIFFICULTY])->setLabel(settings.group(SETTING_DIFFICULTY).currentValue());
  static_cast<LabeledSimpleButton*>(buttons[OPTIONS_LAND_SIZE])->setLabel(settings.group(SETTING_LAND_SIZE).currentValue());
  static_cast<LabeledSimpleButton*>(buttons[OPTIONS_OPPONENTS])->setLabel(settings.group(SETTING_OPPONENTS).currentValue());
  static_cast<LabeledSimpleButton*>(buttons[OPTIONS_MAGIC])->setLabel(settings.group(SETTING_MAGIC_POWER).currentValue());
}

void NewGameView::draw()
{
  Gfx::draw(NEW_GAME_BACKDROP, 0, 0);
  Gfx::draw(NEW_GAME_PARTIALS, 0, 0, 165, 0);
  
  switch (phase)
  {
    case GAME_OPTIONS:
    {
      
      
    }
  }
}