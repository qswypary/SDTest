#include "WidgetOnLayer.h"
#include "XmlLayoutParser.h"

cocos2d::Layer* WidgetOnLayer::createWidgetByLayout(std::string layoutFilen)
{
	auto wol = WidgetOnLayer::create();
	wol->loadXmlLayout(layoutFilen);

	return wol;
}

void WidgetOnLayer::loadXmlLayout(std::string layoutFilen)
{
	// 从 XML 文件读取布局
	auto fileutils = cocos2d::FileUtils::getInstance();
	std::string xmldata = fileutils->getStringFromFile(layoutFilen);

	rapidxml::xml_document<> xmldoc;
	xmldoc.parse<0>(const_cast<char*>(xmldata.c_str()), xmldata.size());

	auto parser = XmlLayoutParser::getInstance();
	_boundNodes = parser->parseIntoScene(xmldoc, *this);
}

const Maps::StringMap<cocos2d::Node*>& WidgetOnLayer::getBoundNodes() const
{
	return _boundNodes;
}
