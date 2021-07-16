#include "GameCharacter.h"
#include <vector>
#include "GameDataUtils.h"
#include "JsonAttributeTableParser.h"

GameCharacter::GameCharacter(std::string id, boolean member, int level) :
	_id(id), _member(member), _level(level) { }

GameCharacter::GameCharacter(const rapidjson::Value &staticData,
	const rapidjson::Value& info)
{
	parseStaticData(staticData);
	parseInfo(info);
	// 还要读一个动态数据

	_nextLvExp = (*_EXP)[_level];
}

std::string GameCharacter::getId() const
{
	return _id;
}

std::string GameCharacter::getName() const
{
	return _name;
}

std::string GameCharacter::getTitle() const
{
	return _title;
}

bool GameCharacter::isMember() const
{
	return _member;
}

int GameCharacter::getLevel() const
{
	return _level;
}

int GameCharacter::getNowExp() const
{
	return _nowExp;
}

int GameCharacter::getNextLvExp() const
{
	return _nextLvExp;
}

int GameCharacter::getGrowPoint() const
{
	return _growPoint;
}

std::string GameCharacter::getAvatarFileName() const
{
	return _avatarFilen;
}

std::shared_ptr<Maps::AttrMap> GameCharacter::getBaseAttr() const
{
	return _BASE_ATTR;
}

std::shared_ptr<Maps::AttrMap> GameCharacter::getOtherAttr() const
{
	return _OTHER_ATTR;
}

std::shared_ptr<Maps::AttrMap> GameCharacter::getBaseAttrGrothRate() const
{
	return _BASE_ATTR_GROWTH_RATE;
}

std::unique_ptr<Maps::StringMap<double>> GameCharacter::getGrowth(const Maps::StringMap<double> growthItems) const
{
	auto util = GameDataUtils::getInstance();
	auto points = util->grow(*_BASE_ATTR, growthItems);
	for (auto &item : *points) {
		item.second *= _BASE_ATTR_GROWTH_RATE->at(item.first)->getInteger();
	}
	return points;
}

std::unique_ptr<Maps::StringMap<double>> GameCharacter::grow(const Maps::StringMap<double> growthItems)
{
	auto util = GameDataUtils::getInstance();
	auto points = util->grow(*_BASE_ATTR, growthItems);
	for (auto &item : *points) {
		item.second *= _BASE_ATTR_GROWTH_RATE->at(item.first)->getInteger();
		const auto &attr = _BASE_ATTR->at(item.first);
		attr->setInteger(attr->getInteger() + item.second);
	}
	return points;
}

std::vector<cocos2d::Label *> GameCharacter::getLabels() const
{
	float begx = 10.0, begy = 470.0;
	float stepx = 120.0, stepy = -20.0;
	std::vector<cocos2d::Label *> vec;
	vec.push_back(cocos2d::Label::createWithTTF(_id, "fonts/FZDBSJW.ttf", 12));
	vec.back()->setAnchorPoint(cocos2d::Vec2(0.0, 1.0));
	vec.back()->setPosition(begx, begy);
	float newx = begx, newy = begy;
	for (const auto& item : *_BASE_ATTR) {
		auto attr = *item.second;
		vec.push_back(cocos2d::Label::createWithTTF(
			attr.getName() + " " + std::to_string(attr.getInteger()), "fonts/FZDBSJW.ttf", 12));
		newy += stepy;
		vec.back()->setAnchorPoint(cocos2d::Vec2(0.0, 1.0));
		vec.back()->setPosition(newx, newy);
	}

	newx += stepx; newy = begy;
	for (const auto& item : *_OTHER_ATTR) {
		auto attr = *item.second;
		vec.push_back(cocos2d::Label::createWithTTF(
			attr.getName() + " " + std::to_string(attr.getInteger()) + " %", "fonts/FZDBSJW.ttf", 12));
		newy += stepy;
		vec.back()->setAnchorPoint(cocos2d::Vec2(0.0, 1.0));
		vec.back()->setPosition(newx, newy);
	}

	newx += stepx + 80.0; newy = begy;
	vec.push_back(cocos2d::Label::createWithTTF("growth:", "fonts/FZDBSJW.ttf", 12));
	vec.back()->setAnchorPoint(cocos2d::Vec2(0.0, 1.0));
	vec.back()->setPosition(newx, newy);
	for (const auto& item : *_BASE_ATTR_GROWTH_RATE) {
		auto attr = *item.second;
		vec.push_back(cocos2d::Label::createWithTTF(
			attr.getName() + " " + std::to_string(attr.getInteger()), "fonts/FZDBSJW.ttf", 12));
		newy += stepy;
		vec.back()->setAnchorPoint(cocos2d::Vec2(0.0, 1.0));
		vec.back()->setPosition(newx, newy);
	}

	return vec;
}

void GameCharacter::setGrowPoint(int growPoint)
{
	_growPoint = growPoint;
}

void GameCharacter::parseStaticData(const rapidjson::Value& staticData)
{
	assert(staticData.IsObject());
	auto util = GameDataUtils::getInstance();
	auto parser = JsonAttributeTableParser::getInstance();

	// 解析 id
	const rapidjson::Value* idv = &staticData.FindMember("id")->value;
	if (!_id.empty() && idv->GetString() != _id) {
		throw std::invalid_argument("the static data object does not belong to this character");
	}
	else if (_id.empty()) {
		_id = idv->GetString();
	}

	// 解析 expTable 的 Object
	const rapidjson::Value* etv = &staticData.FindMember("expTable")->value;
	assert(etv->IsObject());
	if (etv->ObjectEmpty()) {
		_EXP = util->getExpTable();
		_ALL_EXP = util->getAllExpTable();
	}
	else {
		auto pair = util->parseExpTable(*etv);
		_EXP = pair.first;
		_ALL_EXP = pair.second;
	}

	// 解析 baseAttrData 的 Object
	const rapidjson::Value* badv = &staticData.FindMember("baseAttrData")->value;
	_BASE_ATTR = util->getBaseAttrs(_id);
	parser->parseIntoAttrs(*badv, *_BASE_ATTR);

	// 解析 otherAttrData 的 Object
	const rapidjson::Value* oadv = &staticData.FindMember("otherAttrData")->value;
	_OTHER_ATTR = util->getOtherAttrs(_id);
	parser->parseIntoAttrs(*oadv, *_OTHER_ATTR);

	// 解析 baseAttrGrowth 的 Object
	const rapidjson::Value* bagv = &staticData.FindMember("baseAttrGrowth")->value;
	_BASE_ATTR_GROWTH_RATE = util->getBaseAttrs(_id + util->getAttrIdSeparator() + "growth");
	parser->parseIntoAttrs(*bagv, *_BASE_ATTR_GROWTH_RATE);
}

void GameCharacter::parseInfo(const rapidjson::Value& info)
{
	assert(info.IsObject());

	const rapidjson::Value* idv = &info.FindMember("id")->value;
	assert(idv->IsString());
	if (!_id.empty() && idv->GetString() != _id) {
		throw std::invalid_argument("the infomation object does not belong to this character");
	}
	else if (_id.empty()) {
		_id = idv->GetString();
	}

	const rapidjson::Value* namev = &info.FindMember("name")->value;
	assert(namev->IsString());
	_name = namev->GetString();

	const rapidjson::Value* titlev = &info.FindMember("title")->value;
	assert(titlev->IsString());
	_title = titlev->GetString();

	const rapidjson::Value* afnv = &info.FindMember("avatarFileName")->value;
	assert(afnv->IsString());
	_avatarFilen = afnv->GetString();
}
