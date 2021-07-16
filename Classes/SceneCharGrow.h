#pragma once
#include "cocos2d.h"
#include "GameCharacter.h"
#include "WidgetCharGrow.h"
#include "WidgetCharScroller.h"

class SceneCharGrow : cocos2d::Scene {
private:
    cocos2d::Layer* _rootLayer;

    WidgetCharGrow *_widgetCharGrow;
    WidgetCharScroller *_widgetCharScroller;

public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(SceneCharGrow);
};