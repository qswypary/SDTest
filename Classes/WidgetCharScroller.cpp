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

	loadXmlLayout("json/widgetCharScrollerLayout.xml");

	return true;
}
