#pragma once
#include <cocos2d.h>
#include "GameCharacter.h"
#include "WidgetOnLayer.h"

class WidgetCharScroller : public WidgetOnLayer {
private:
	std::shared_ptr<GameCharacter> _currChar;
	std::unordered_map<std::string, std::shared_ptr<GameCharacter>> _chars;
    Maps::StringMap<cocos2d::Node*> _boundNodes;

public:
    static cocos2d::Layer* createWidget();

    virtual bool init();

    CREATE_FUNC(WidgetCharScroller);
};