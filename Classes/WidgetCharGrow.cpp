#include "WidgetCharGrow.h"
#include "GameDataUtils.h"
#include "XmlLayoutParser.h"

cocos2d::Layer* WidgetCharGrow::createWidget(std::shared_ptr<GameCharacter> chara)
{
	auto widget = WidgetCharGrow::create();
	widget->switchCharacter(chara);

	return widget;
}

bool WidgetCharGrow::init()
{
	if (!Layer::init()) {
		return false;
	}

	loadXmlLayout("json/widgetCharGrowLayout.xml");

	return true;
}

void WidgetCharGrow::initCharData()
{
	auto utils = GameDataUtils::getInstance();

	_growItems = utils->getGrowthItems();
	for (auto item : *(utils->getGrowthItems())) {
		_addPoints[item] = 0;
	}
	_leftPoints = _char->getGrowPoint();

	auto attrs = _char->getFinalBaseAttr();
	for (const auto &attr : *attrs) {
		_rawAttrs[attr.first] = _grownAttrs[attr.first] = attr.second->getInteger();
	}
}

void WidgetCharGrow::initBoundNodes()
{
	// 渲染布局
	// 角色基础信息
	const auto& bound = getBoundNodes();
	dynamic_cast<cocos2d::Sprite*>(bound.at("info.avatar"))->setTexture(
		_char->getAvatarFileName());
	XmlLayoutParser::renderLabel(
		*dynamic_cast<cocos2d::Label*>(bound.at("info.name")), "value", _char->getName());
	XmlLayoutParser::renderLabel(
		*dynamic_cast<cocos2d::Label*>(bound.at("info.title")), "value", _char->getTitle());
	XmlLayoutParser::renderLabel(
		*dynamic_cast<cocos2d::Label*>(bound.at("info.level")), "value",
		std::to_string(_char->getLevel()));
	Maps::StringMap<std::string> map;
	map["now"] = std::to_string(_char->getNowExp());
	map["needed"] = std::to_string(_char->getNextLvExp());
	XmlLayoutParser::renderLabel(
		*dynamic_cast<cocos2d::Label*>(bound.at("info.exp")), map);
	// 属性名称
	auto util = GameDataUtils::getInstance();
	for (auto attr : _rawAttrs) {
		std::string id = attr.first;
		std::string name = util->getAttrName(id);
		XmlLayoutParser::renderLabel(
			*dynamic_cast<cocos2d::Label*>(bound.at("attrs." + id + ".name")),
			"value", name);
	}
	// 加点项目名称
	for (auto item : *_growItems) {
		std::string name = util->getAttrName(item);
		XmlLayoutParser::renderLabel(
			*dynamic_cast<cocos2d::Label*>(bound.at("items." + item + ".name")),
			"value", name);
	}

	// 菜单项设置回调
	// 加点箭头
	for (auto name : *_growItems) {
		dynamic_cast<cocos2d::MenuItem*>(bound.at("items." + name + ".adjust.left"))
			->setCallback(CC_CALLBACK_1(
				WidgetCharGrow::changeGrowthPoint,
				this, name, -1
			));
		dynamic_cast<cocos2d::MenuItem*>(bound.at("items." + name + ".adjust.right"))
			->setCallback(CC_CALLBACK_1(
				WidgetCharGrow::changeGrowthPoint,
				this, name, 1
			));
	}
	// 选项
	dynamic_cast<cocos2d::MenuItem*>(bound.at("options.ok"))
		->setCallback(CC_CALLBACK_1(
			WidgetCharGrow::okButtonCallback,
			this
		));
	dynamic_cast<cocos2d::MenuItem*>(bound.at("options.clear"))
		->setCallback(CC_CALLBACK_1(
			WidgetCharGrow::clearButtonCallback,
			this
		));
	dynamic_cast<cocos2d::MenuItem*>(bound.at("options.auto"))
		->setCallback(CC_CALLBACK_1(
			WidgetCharGrow::autoButtonCallback,
			this
		));
	dynamic_cast<cocos2d::MenuItem*>(bound.at("options.exit"))
		->setCallback(CC_CALLBACK_1(
			WidgetCharGrow::exitButtonCallback,
			this
		));
}

void WidgetCharGrow::switchCharacter(std::shared_ptr<GameCharacter> chara)
{
	_char = chara;
	initCharData();
	initBoundNodes();
	refresh();
}

void WidgetCharGrow::refresh()
{
	// 渲染布局
	// 属性值
	const auto& bound = getBoundNodes();
	for (auto attr : _rawAttrs) {
		std::string id = attr.first;
		auto rawl = dynamic_cast<cocos2d::Label*>(bound.at("attrs." + id + ".prev"));
		auto grownl = dynamic_cast<cocos2d::Label*>(bound.at("attrs." + id + ".grown"));
		XmlLayoutParser::renderLabel(
			*rawl,
			"value", std::to_string(attr.second));
		XmlLayoutParser::renderLabel(
			*grownl,
			"value", std::to_string(_grownAttrs.at(id)));
		if (attr.second != _grownAttrs.at(id)) {
			grownl->setTextColor(cocos2d::Color4B::RED);
		}
		else {
			grownl->setTextColor(rawl->getTextColor());
		}
	}
	// 加点
	for (auto item : *_growItems) {
		XmlLayoutParser::renderLabel(
			*dynamic_cast<cocos2d::Label*>(bound.at("items." + item + ".addv")),
			"value", std::to_string(int(_addPoints.at(item))));
	}
	// 剩余点数
	XmlLayoutParser::renderLabel(
		*dynamic_cast<cocos2d::Label*>(bound.at("point")),
		"value", std::to_string(_leftPoints));
}

void WidgetCharGrow::changeGrowthPoint(Ref* pSender, std::string item, int quant)
{
	if (_addPoints.at(item) + quant >= 0 && _leftPoints - quant >= 0) {
		_addPoints.at(item) += quant;
		_leftPoints -= quant;
		auto growth = _char->getGrowth(_addPoints);
		for (auto &attrv : _grownAttrs) {
			attrv.second = (*growth)[attrv.first] + _rawAttrs.at(attrv.first);
		}
		refresh();
	}
}

void WidgetCharGrow::okButtonCallback(Ref* pSender)
{
	// 提交到角色信息
	_char->grow(_addPoints);
	_char->setGrowPoint(_leftPoints);
	// 更新显示数据
	for (auto &item : _rawAttrs) {
		item.second = _grownAttrs.at(item.first);
	}
	for (auto& item : _addPoints) {
		item.second = 0.0;
	}
	refresh();

	auto savefileu = SaveFileUtils::getInstance();
	savefileu->clear(false);
	savefileu->appendObjectInArray();
	savefileu->setCurrentNodeInArray(0);
	auto st = savefileu->getStatus();
	_char->saveDynamicData(st);
	savefileu->save("charGrowthData.save");
}

void WidgetCharGrow::clearButtonCallback(Ref* pSender)
{
	for (auto& item : _grownAttrs) {
		item.second = _rawAttrs.at(item.first);
	}
	double sum = 0.0;
	for (auto& item : _addPoints) {
		sum += item.second;
		item.second = 0.0;
	}
	_leftPoints += sum;
	refresh();
}

void WidgetCharGrow::autoButtonCallback(Ref* pSender)
{
}

void WidgetCharGrow::exitButtonCallback(Ref* pSender)
{
}
