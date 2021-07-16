#include "SceneCharGrow.h"
#include "rapidxml/rapidxml_utils.hpp"
#include "XmlLayoutParser.h"

cocos2d::Scene* SceneCharGrow::createScene()
{
	return SceneCharGrow::create();
}

bool SceneCharGrow::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
    cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();

    // 从 XML 文件读取布局
    auto fileutils = cocos2d::FileUtils::getInstance();
    std::string xmldata = fileutils->getStringFromFile("json/sceneCharGrowLayout.xml");

    rapidxml::xml_document<> xmldoc;
    xmldoc.parse<0>(const_cast<char *>(xmldata.c_str()), xmldata.size());

    auto parser = XmlLayoutParser::getInstance();
    _rootLayer = cocos2d::Layer::create();
    auto bound = parser->parseIntoScene(xmldoc, *_rootLayer);
    this->addChild(_rootLayer);

    // 正常而言应该从一个公共实例里面去找到这个角色的实例就完事了
    // 这里暂时写成直接从文件加载
    std::string data = fileutils->getStringFromFile("json/charStaticData.json");

    rapidjson::Document document;
    document.Parse(data.c_str());
    assert(document.IsArray());
    auto arr = document.GetArray();
    assert(arr.Size() > 0);

    data = fileutils->getStringFromFile("json/charInfo.json");

    rapidjson::Document document2;
    document2.Parse(data.c_str());
    assert(document2.IsArray());
    auto arr2 = document2.GetArray();
    assert(arr2.Size() > 0);

    auto chara = std::make_shared<GameCharacter>(GameCharacter(arr[0], arr2[0]));

    // 绑定组件
    auto father = bound["charScroller"]->getParent();
    father->removeChild(bound["charScroller"]);
    bound["charScroller"] = WidgetCharScroller::createWidget();
    father->addChild(bound["charScroller"]);
    _widgetCharScroller = dynamic_cast<WidgetCharScroller*>(bound["charScroller"]);

    auto father2 = bound["charGrow"]->getParent();
    father2->removeChild(bound["charGrow"]);
    bound["charGrow"] = WidgetCharGrow::createWidget(chara);
    father2->addChild(bound["charGrow"]);
    _widgetCharGrow = dynamic_cast<WidgetCharGrow*>(bound["charGrow"]);

    //// 创建各标签和菜单
    //auto namel = cocos2d::Label::createWithTTF(_currChar->getName(), "fonts/FZDBSJW.ttf", 32);
    //namel->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    //namel->setPosition(origin.x + 30.0, 
    //    origin.y + visibleSize.height - 60.0);
    //auto levell = cocos2d::Label::createWithTTF(
    //    "Level " + std::to_string(_currChar->getLevel()), "fonts/FZDBSJW.ttf", 24);
    //levell->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    //levell->setPosition(origin.x + 160.0,
    //    origin.y + visibleSize.height - 57.0);
    //_infoLabels.push_back(namel);
    //_infoLabels.push_back(levell);
    //this->addChild(namel);
    //this->addChild(levell);

    //float y = origin.y + visibleSize.height - 60.0;
    //for (const auto& item : *(_currChar->getBaseAttr())) {
    //    std::vector<cocos2d::Label*> vec;
    //    auto attrnl = cocos2d::Label::createWithTTF(item.second->getName(), "fonts/FZDBSJW.ttf", 18);
    //    attrnl->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    //    attrnl->setPosition(origin.x + 30.0, y -= 40.0);
    //    vec.push_back(attrnl);
    //    auto beforel = cocos2d::Label::createWithTTF(std::to_string(item.second->getInteger()), "fonts/FZDBSJW.ttf", 18);
    //    beforel->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    //    beforel->setPosition(origin.x + 160.0, y);
    //    vec.push_back(beforel);
    //    auto arrowl = cocos2d::Label::createWithTTF("->", "fonts/FZDBSJW.ttf", 18);
    //    arrowl->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    //    arrowl->setPosition(origin.x + 200.0, y);
    //    vec.push_back(arrowl);
    //    auto afterl = cocos2d::Label::createWithTTF(std::to_string(item.second->getInteger()), "fonts/FZDBSJW.ttf", 18);
    //    afterl->setAnchorPoint(cocos2d::Vec2(0.0, 0.0));
    //    afterl->setPosition(origin.x + 240.0, y);
    //    vec.push_back(afterl);
    //    _attrLabels.push_back(vec);
    //    for (auto p : vec) { this->addChild(p); }
    //}

	return true;
}
