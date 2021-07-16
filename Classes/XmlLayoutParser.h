#pragma once
#include <memory>
#include "cocos2d.h"
#include "rapidxml/rapidxml.hpp"
#include "Maps.h"
#include "WidgetOnLayer.h"

class XmlLayoutParser {
private:
	typedef WidgetOnLayer* (*CreateFunc)();

	static std::shared_ptr<XmlLayoutParser> instance;

	static const char _BIND_NAME_SP;
	static Maps::StringMap<CreateFunc> _WIDGET_MAP;

	cocos2d::Layer* _rootScene;
	Maps::StringMap<cocos2d::Color3B> _defaultColors;
	std::string _defaultFontFilen = "fonts/msyh.ttc";
	int _defaultFontSize = 10;
	cocos2d::Color3B _defaultFontColor = cocos2d::Color3B::BLACK;
	cocos2d::Vec2 _defaultAnchor = cocos2d::Vec2(0.0, 0.0);
	cocos2d::Vec2 _defaultAreaAnchor = cocos2d::Vec2(0.5, 0.5);
	Maps::StringMap<cocos2d::Node*> _boundNodes;

	XmlLayoutParser();

public:
	static std::shared_ptr<XmlLayoutParser> getInstance();

	Maps::StringMap<cocos2d::Node*> parseIntoScene(
		const rapidxml::xml_node<>& root, cocos2d::Layer &scene);

	static void renderLabel(cocos2d::Label& label, std::string name, std::string data);
	static void renderLabel(cocos2d::Label& label, Maps::StringMap<std::string> args);

private:
	cocos2d::Node* parseNodeRecursive(
		const rapidxml::xml_node<>& node, std::string fatherBind);

	void parseNodeInfo(const rapidxml::xml_node<>& node, cocos2d::Node &ccnode) const;

	cocos2d::Label* parseToLabel(const rapidxml::xml_node<>& node) const;
	cocos2d::Sprite* parseToSprite(const rapidxml::xml_node<>& node) const;
	cocos2d::Menu* parseToMenu(const rapidxml::xml_node<>& node) const;
	cocos2d::MenuItemLabel* parseToMenuItemLabel(const rapidxml::xml_node<>& node) const;
	cocos2d::MenuItemImage* parseToMenuItemImage(const rapidxml::xml_node<>& node) const;
	cocos2d::Layer* parseToLayer(const rapidxml::xml_node<>& node) const;
	WidgetOnLayer* parseToWidget(const rapidxml::xml_node<>& node) const;

	cocos2d::Color3B strToColor(std::string rgb) const;
};