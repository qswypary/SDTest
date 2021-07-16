#pragma once
#include <cocos2d.h>
#include "GameCharacter.h"

class WidgetCharGrow : public cocos2d::Layer {
private:
    std::shared_ptr<GameCharacter> _char;
    Maps::StringMap<cocos2d::Node*> _boundNodes;
    std::shared_ptr<std::vector<std::string>> _growItems;

    Maps::StringMap<int> _rawAttrs;
    Maps::StringMap<int> _grownAttrs;
    int _leftPoints = 0;
    Maps::StringMap<double> _addPoints;

public:
    static cocos2d::Layer* createWidget(std::shared_ptr<GameCharacter> chara);

    virtual bool init();

    void refresh();

    CREATE_FUNC(WidgetCharGrow);

    void changeGrowthPoint(Ref* pSender, std::string item, int quant);

    void okButtonCallback(Ref* pSender);
    void clearButtonCallback(Ref* pSender);
    void autoButtonCallback(Ref* pSender);
    void exitButtonCallback(Ref* pSender);

private:
    void initCharData();
    void initBoundNodes();
};