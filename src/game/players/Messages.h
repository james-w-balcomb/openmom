//
//  Messages.h
//  OpenMoM
//
//  Created by Jack on 7/23/14.
//  Copyright (c) 2014 Jack. All rights reserved.
//

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <string>
#include <functional>

#include "Font.h"
#include "Localization.h"

class Skill;
class City;
class Building;

namespace messages {

typedef std::function<void()> Action;



class Message
{
protected:
  std::string message;
  
public:
  enum Type
  {
    MESSAGE,
    ERROR,
    CONFIRM,
    NEW_BUILDING,
    HELP_SKILL
  } type;
  
  const std::string getMessage() { return message; }
  
  Message(std::string message) : message(message), type(MESSAGE) { }

protected:
  Message(std::string message, Type type) : message(message), type(type) { }
};

class Error : public Message
{
public:
  Error(std::string& message) : Message(message, ERROR) { }
};

class Confirm : public Message
{
public:
  Confirm(std::string message, Action&& action) : Message(message, CONFIRM), action(action) { }
  const Action action;
};

class HelpSkill : public Message
{
public:
  HelpSkill(std::string string, const Skill* skill) : Message(message, HELP_SKILL), skill(skill) { }
  
  const Skill* skill;
};

class NewBuilding : public Message
{
  
  NewBuilding(const City* city, const Building* building);
  
  const Building* building;
  const City* city;
};
  
}

#endif