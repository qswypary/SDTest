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

    // �� XML �ļ���ȡ����
    auto fileutils = cocos2d::FileUtils::getInstance();
    std::string xmldata = fileutils->getStringFromFile("json/sceneCharGrowLayout.xml");

    rapidxml::xml_document<> xmldoc;
    xmldoc.parse<0>(const_cast<char *>(xmldata.c_str()), xmldata.size());

    auto parser = XmlLayoutParser::getInstance();
    _rootLayer = cocos2d::Layer::create();
    auto bound = parser->parseIntoScene(xmldoc, *_rootLayer);
    this->addChild(_rootLayer);

    // ��������Ӧ�ô�һ������ʵ������ȥ�ҵ������ɫ��ʵ����������
    // ������ʱд��ֱ�Ӵ��ļ�����
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

    auto savefileu = SaveFileUtils::getInstance();
    savefileu->load("charGrowthData.save");
    savefileu->setCurrentNodeInArray(0);
    auto st = savefileu->getStatus();

    auto chara = std::make_shared<GameCharacter>(GameCharacter(arr[0], arr2[0], st));

    // �����
    _widgetCharScroller = dynamic_cast<WidgetCharScroller*>(bound["charScroller"]);

    _widgetCharGrow = dynamic_cast<WidgetCharGrow*>(bound["charGrow"]);
    _widgetCharGrow->switchCharacter(chara);

	return true;
}
