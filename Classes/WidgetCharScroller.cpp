#include "WidgetCharScroller.h"
#include "XmlLayoutParser.h"

cocos2d::Layer* WidgetCharScroller::createWidget()
{
	return WidgetCharScroller::create();
}

bool WidgetCharScroller::init()
{
	if (!Layer::init()) {
		return false;
	}

	// 从 XML 文件读取布局
	auto fileutils = cocos2d::FileUtils::getInstance();
	std::string xmldata = fileutils->getStringFromFile("json/widgetCharScrollerLayout.xml");

	rapidxml::xml_document<> xmldoc;
	xmldoc.parse<0>(const_cast<char*>(xmldata.c_str()), xmldata.size());

	auto parser = XmlLayoutParser::getInstance();
	_boundNodes = parser->parseIntoScene(xmldoc, *this);

	return true;
}
