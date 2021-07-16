#pragma once
#include <cocos2d.h>
#include "GameCharacter.h"

class WidgetCharScroller : public cocos2d::Layer {
private:
	std::shared_ptr<GameCharacter> _currChar;
	std::unordered_map<std::string, std::shared_ptr<GameCharacter>> _chars;
    Maps::StringMap<cocos2d::Node*> _boundNodes;

public:
    static cocos2d::Layer* createWidget();

    virtual bool init();

    CREATE_FUNC(WidgetCharScroller);
};