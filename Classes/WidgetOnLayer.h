#pragma once
#include <cocos2d.h>
#include "Maps.h"

class WidgetOnLayer : public cocos2d::Layer {
private:
    Maps::StringMap<cocos2d::Node*> _boundNodes;

public:
    static cocos2d::Layer* createWidgetByLayout(std::string layoutFilen);

    void loadXmlLayout(std::string layoutFilen);

    CREATE_FUNC(WidgetOnLayer);

protected:
    const Maps::StringMap<cocos2d::Node*> &getBoundNodes() const;
};