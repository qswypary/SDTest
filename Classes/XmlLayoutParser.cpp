#include "XmlLayoutParser.h"
#include "WidgetCharGrow.h"
#include "WidgetCharScroller.h"
#include <cctype>

std::shared_ptr<XmlLayoutParser> XmlLayoutParser::instance;
const char XmlLayoutParser::_BIND_NAME_SP = '.';
Maps::StringMap<XmlLayoutParser::CreateFunc> XmlLayoutParser::_WIDGET_MAP;

XmlLayoutParser::XmlLayoutParser() = default;

std::shared_ptr<XmlLayoutParser> XmlLayoutParser::getInstance()
{
	if (!instance) {
		instance = std::make_unique<XmlLayoutParser>(XmlLayoutParser());
		_WIDGET_MAP["charGrow"] = CreateFunc(WidgetCharGrow::create);
		_WIDGET_MAP["charScroller"] = CreateFunc(WidgetCharScroller::create);
	}
	return instance;
}

Maps::StringMap<cocos2d::Node*> XmlLayoutParser::parseIntoScene(const rapidxml::xml_node<>& root, cocos2d::Layer& scene)
{
	_rootScene = &scene;

	auto layoutnode = root.first_node("layout");
	assert(layoutnode);

	// 解析全局设置节点
	auto settingsnode = layoutnode->first_node("settings");
	assert(settingsnode);
	rapidxml::xml_node<>* colorsnode = nullptr,
		* fontnode = nullptr,
		* areanode = nullptr,
		* anchornode = nullptr;
	for (auto it = settingsnode->first_node(); it != nullptr; it = it->next_sibling()) {
		std::string name = it->name();
		if (name == "colors") {
			colorsnode = it;
		}
		else if (name == "font") {
			fontnode = it;
		}
		else if (name == "area") {
			areanode = it;
		}
		else if (name == "anchor") {
			anchornode = it;
		}
	}
	// 解析色彩设置
	if (colorsnode) {
		for (auto it = colorsnode->first_node(); it != nullptr; it = it->next_sibling()) {
			std::string name = it->name();
			if (name == "color") {
				_defaultColors[it->first_attribute("name")->value()] =
					strToColor(it->first_attribute("value")->value());
			}
		}
	}
	// 解析字体设置
	if (fontnode) {
		for (auto it = fontnode->first_attribute(); it != nullptr; it = it->next_attribute()) {
			std::string name = it->name();
			if (name == "size") {
				_defaultFontSize = std::stoi(it->value());
			}
			else if (name == "filePath") {
				_defaultFontFilen = it->value();
			}
			else if (name == "color") {
				_defaultFontColor = strToColor(it->value());
			}
		}
	}
	// 解析显示区域设置
	scene.setIgnoreAnchorPointForPosition(false);
	scene.setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
	auto visibleSize = cocos2d::Director::getInstance()->getWinSize();
	scene.setPosition(visibleSize / 2.0);
	if (areanode) {
		float width = visibleSize.width, height = visibleSize.height;
		for (auto it = areanode->first_attribute(); it != nullptr; it = it->next_attribute()) {
			std::string name = it->name();
			float value = std::stof(it->value());
			if (name == "x") {
				scene.setPositionX(value);
			}
			else if (name == "y") {
				scene.setPositionY(value);
			}
			else if (name == "width") {
				width = value;
			}
			else if (name == "height") {
				height = value;
			}
		}
		scene.setContentSize(cocos2d::Size(width, height));
	}
	// 解析锚点设置
	if (anchornode) {
		float anchorX = _defaultAnchor.x, anchorY = _defaultAnchor.y;
		for (auto na = anchornode->first_attribute(); na != nullptr; na = na->next_attribute()) {
			std::string name = na->name();
			float floatv = std::stof(na->value());
			if (name == "x") {
				anchorX = floatv;
			}
			else if (name == "y") {
				anchorY = floatv;
			}
		}
		_defaultAnchor = cocos2d::Vec2(anchorX, anchorY);
	}

	// 解析主体节点树
	auto bodynode = layoutnode->first_node("body");
	parseNodeRecursive(*bodynode, "");

	return _boundNodes;
}

void XmlLayoutParser::renderLabel(cocos2d::Label& label, std::string name, std::string data)
{
	std::string raw = label.getName();
	std::string res;

	auto last = raw.end();
	auto edited = raw.begin();
	for (auto it = raw.begin(); it != raw.end(); ++it) {
		if (*it == '{') {
			last = it;
		}
		else if (*it == '}' && last != raw.end()) {
			std::string arg(last + 1, it);
			if (name == arg) {
				res += std::string(edited, last) + data;
				edited = it + 1;
			}
			last = raw.end();
		}
	}
	res += std::string(edited, raw.end());
	label.setString(res);
}

void XmlLayoutParser::renderLabel(cocos2d::Label& label, Maps::StringMap<std::string> args)
{
	std::string raw = label.getName();
	std::string res;

	auto last = raw.end();
	auto edited = raw.begin();
	for (auto it = raw.begin(); it != raw.end(); ++it) {
		if (*it == '{') {
			last = it;
		}
		else if (*it == '}' && last != raw.end()) {
			std::string arg(last + 1, it);
			if (args.find(arg) != args.end()) {
				res += std::string(edited, last) + args[arg];
				edited = it + 1;
			}
			last = raw.end();
		}
	}
	res += std::string(edited, raw.end());
	label.setString(res);
}

cocos2d::Node* XmlLayoutParser::parseNodeRecursive(
	const rapidxml::xml_node<>& node, std::string fatherBind)
{
	// 创建 cocos2d 节点
	cocos2d::Node* ccnode;
	std::string name = node.name();
	if (name == "body") {
		ccnode = _rootScene;
	}
	else if (name == "label") {
		ccnode = parseToLabel(node);
	}
	else if (name == "static" || name == "sprite") {
		ccnode = parseToSprite(node);
	}
	else if (name == "menu") {
		ccnode = parseToMenu(node);
	}
	else if (name == "menuItemLabel") {
		ccnode = parseToMenuItemLabel(node);
	}
	else if (name == "menuItemImage") {
		ccnode = parseToMenuItemImage(node);
	}
	else if (name == "set") {
		ccnode = parseToLayer(node);
	}
	else if (name == "widget") {
		ccnode = parseToWidget(node);
	}

	// 解析绑定
	std::string bindName = fatherBind;
	auto bind = node.first_attribute("bind");
	if (bind) {
		//ccnode->setVisible(false);
		if (!bindName.empty()) {
			bindName += _BIND_NAME_SP;
		}
		bindName += bind->value();
		_boundNodes[bindName] = ccnode;
	}

	// 解析子节点
	for (auto it = node.first_node(); it != nullptr; it = it->next_sibling()) {
		if (it->type() == rapidxml::node_element) {
			ccnode->addChild(parseNodeRecursive(*it, bindName));
		}
	}

	return ccnode;
}

void XmlLayoutParser::parseNodeInfo(const rapidxml::xml_node<>& node, cocos2d::Node& ccnode) const
{
	float x = ccnode.getPositionX(), y = ccnode.getPositionY();
	auto anchor = ccnode.getAnchorPoint();
	float anchorX = anchor.x, anchorY = anchor.y;
	// 遍历 XML 节点属性
	for (auto na = node.first_attribute(); na != nullptr; na = na->next_attribute()) {
		std::string name = na->name();
		if (name == "x") {
			x = std::stof(na->value());
		}
		else if (name == "y") {
			y = std::stof(na->value());
		}
		else if (name == "anchorX") {
			anchorX = std::stof(na->value());
		}
		else if (name == "anchorY") {
			anchorY = std::stof(na->value());
		}
	}
	ccnode.setPosition(x, y);
	ccnode.setAnchorPoint(cocos2d::Vec2(anchorX, anchorY));
}

cocos2d::Label* XmlLayoutParser::parseToLabel(const rapidxml::xml_node<>& node) const
{
	std::string filen = _defaultFontFilen;
	int size = _defaultFontSize;
	cocos2d::Color3B color = _defaultFontColor;
	// 遍历 XML 节点属性
	for (auto na = node.first_attribute(); na != nullptr; na = na->next_attribute()) {
		std::string name = na->name();
		if (name == "fontSize") {
			size = std::stoi(na->value());
		}
		else if (name == "fontFilePath") {
			filen = na->value();
		}
		else if (name == "fontColor") {
			color = strToColor(na->value());
		}
	}
	// 创建标签对象
	auto label = cocos2d::Label::createWithTTF(node.value(), filen, size);
	label->setName(node.value());
	label->setTextColor(cocos2d::Color4B(color));
	label->setAnchorPoint(_defaultAnchor);
	parseNodeInfo(node, *label);
	return label;
}

cocos2d::Sprite* XmlLayoutParser::parseToSprite(const rapidxml::xml_node<>& node) const
{
	std::string src;
	// 遍历 XML 节点属性
	for (auto na = node.first_attribute(); na != nullptr; na = na->next_attribute()) {
		std::string name = na->name();
		if (name == "srcPath") {
			src = na->value();
		}
	}
	// 创建精灵对象
	auto sprite = cocos2d::Sprite::create();
	if (!src.empty()) {
		sprite = cocos2d::Sprite::create(src);
	}
	sprite->setAnchorPoint(_defaultAnchor);
	parseNodeInfo(node, *sprite);
	return sprite;
}

cocos2d::Menu* XmlLayoutParser::parseToMenu(const rapidxml::xml_node<>& node) const
{
	float width = _rootScene->getContentSize().width,
		height = _rootScene->getContentSize().height;
	// 遍历 XML 节点属性
	for (auto na = node.first_attribute(); na != nullptr; na = na->next_attribute()) {
		std::string name = na->name();
		if (name == "width") {
			width = std::stof(na->value());
		}
		else if (name == "height") {
			height = std::stof(na->value());
		}
	}
	// 创建菜单对象
	auto menu = cocos2d::Menu::create();
	menu->setContentSize(cocos2d::Size(width, height));
	menu->setIgnoreAnchorPointForPosition(false);
	menu->setAnchorPoint(_defaultAreaAnchor);
	menu->setPosition(cocos2d::Size(width, height) / 2.0);
	parseNodeInfo(node, *menu);
	return menu;
}

cocos2d::MenuItemLabel* XmlLayoutParser::parseToMenuItemLabel(const rapidxml::xml_node<>& node) const
{
	auto menuItem = cocos2d::MenuItemLabel::create(parseToLabel(node));
	return menuItem;
}

cocos2d::MenuItemImage* XmlLayoutParser::parseToMenuItemImage(const rapidxml::xml_node<>& node) const
{
	std::string src, srcSelected;
	// 遍历 XML 节点属性
	for (auto na = node.first_attribute(); na != nullptr; na = na->next_attribute()) {
		std::string name = na->name();
		if (name == "srcPath") {
			src = na->value();
		}
		else if (name == "srcPathSelected") {
			srcSelected = na->value();
		}
	}
	// 创建菜单项目对象
	auto menuItem = cocos2d::MenuItemImage::create(src, srcSelected);
	menuItem->setAnchorPoint(_defaultAnchor);
	parseNodeInfo(node, *menuItem);
	return menuItem;
}

cocos2d::Layer* XmlLayoutParser::parseToLayer(const rapidxml::xml_node<>& node) const
{
	float width = _rootScene->getContentSize().width,
		height = _rootScene->getContentSize().height;
	// 遍历 XML 节点属性
	for (auto na = node.first_attribute(); na != nullptr; na = na->next_attribute()) {
		std::string name = na->name();
		if (name == "width") {
			width = std::stof(na->value());
		}
		else if (name == "height") {
			height = std::stof(na->value());
		}
	}
	// 创建图层对象
	auto layer = cocos2d::Layer::create();
	layer->setContentSize(cocos2d::Size(width, height));
	layer->setIgnoreAnchorPointForPosition(false);
	layer->setAnchorPoint(_defaultAreaAnchor);
	layer->setPosition(cocos2d::Size(width, height) / 2.0);
	parseNodeInfo(node, *layer);
	return layer;
}

WidgetOnLayer* XmlLayoutParser::parseToWidget(const rapidxml::xml_node<>& node) const
{
	float width = _rootScene->getContentSize().width,
		height = _rootScene->getContentSize().height;

	std::string bind = node.first_attribute("bind")->value();

	auto widget = _WIDGET_MAP[bind]();
	widget->setIgnoreAnchorPointForPosition(false);
	widget->setAnchorPoint(_defaultAreaAnchor);
	widget->setPosition(cocos2d::Size(width, height) / 2.0);
	parseNodeInfo(node, *widget);
	return widget;
}

cocos2d::Color3B XmlLayoutParser::strToColor(std::string rgb) const
{
	auto color = _defaultColors.find(rgb);
	if (color != _defaultColors.end()) {
		return color->second;
	}

	std::string hex = "0123456789ABCDEF";
	auto tohex = [&hex](char ch) {
		return hex.find(std::toupper(ch));
	};
	unsigned int r = tohex(rgb[0]) * 16 + tohex(rgb[1]);
	unsigned int g = tohex(rgb[2]) * 16 + tohex(rgb[3]);
	unsigned int b = tohex(rgb[4]) * 16 + tohex(rgb[5]);
	return cocos2d::Color3B(r, g, b);
}
