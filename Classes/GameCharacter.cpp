#include "GameCharacter.h"
#include <vector>
#include "GameDataUtils.h"
#include "JsonAttributeTableParser.h"
#include "SaveFileUtils.h"

GameCharacter::GameCharacter(std::string id, boolean member, int level) :
	_id(id), _member(member), _level(level) { }

GameCharacter::GameCharacter(const rapidjson::Value& staticData,
	const rapidjson::Value& info)
{
	parseStaticData(staticData);
	parseInfo(info);
	initDynamicData();

	_nextLvExp = (*_EXP)[_level];
}

GameCharacter::GameCharacter(const rapidjson::Value &staticData,
	const rapidjson::Value& info, SaveFileUtils::Status st)
{
	parseStaticData(staticData);
	parseInfo(info);
	loadDynamicData(st);

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

std::shared_ptr<Maps::AttrWithMaxiumMap> GameCharacter::getDynamicAttr() const
{
	return _dynamicAttr;
}

std::shared_ptr<Maps::AttrMap> GameCharacter::getFinalBaseAttr() const
{
	return _finalBaseAttr;
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
		// 更改成长值矩阵
		auto& growth = _baseAttrGrowth.at(item.first)[0];
		growth->setInteger(growth->getInteger() + item.second);
		// 更改基础属性最终值
		auto &attr = _finalBaseAttr->at(item.first);
		attr->setInteger(attr->getInteger() + item.second);
	}
	return points;
}

void GameCharacter::setGrowPoint(int growPoint)
{
	_growPoint = growPoint;
}

void GameCharacter::saveDynamicData(SaveFileUtils::Status st)
{
	auto util = SaveFileUtils::getInstance();
	SaveFileUtils::Status prevst = util->getStatus();
	util->setStatus(st);

	util->setStringInObject("id", _id);
	util->setBoolInObject("member", _member);
	util->setIntegerInObject("level", _level);
	util->setIntegerInObject("exp", _nowExp);
	util->setIntegerInObject("growPoint", _growPoint);

	util->setObjectInObject("dynamicAttrData");
	util->setCurrentNodeInObject("dynamicAttrData");
	for (const auto &item : *_dynamicAttr) {
		util->setAttributeInObject(item.first, *item.second);
	}
	util->setStatus(st);

	util->setObjectInObject("baseAttrGrowth");
	util->setCurrentNodeInObject("baseAttrGrowth");
	SaveFileUtils::Status growthRoot = util->getStatus();
	for (const auto& item : _baseAttrGrowth) {
		util->setStatus(growthRoot);
		util->setArrayInObject(item.first);
		util->setCurrentNodeInObject(item.first);
		for (const auto& growth : item.second) {
			util->appendAttributeInArray(*growth);
		}
	}
	util->setStatus(st);

	util->setStatus(prevst);
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
	_BASE_ATTR = util->getBaseAttrs(_id + ":raw");
	_finalBaseAttr = util->getBaseAttrs(_id);
	parser->parseIntoAttrs(*badv, *_BASE_ATTR);
	parser->parseIntoAttrs(*badv, *_finalBaseAttr);

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

void GameCharacter::initDynamicData()
{
	_member = false;
	_level = 1;
	_nowExp = 0;
	_growPoint = 0;

	auto util = GameDataUtils::getInstance();
	_dynamicAttr = util->getDynamicAttrs(_id);

	auto baseattrs = util->getBaseAttrs(_id);
	for (const auto& attr : *baseattrs) {
		_baseAttrGrowth[attr.first] = 
			std::vector<std::unique_ptr<GameAttribute>>();
		for (int i = 0; i != 3; ++i) {
			_baseAttrGrowth[attr.first].push_back(
				std::make_unique<GameAttribute>(
					GameAttribute(std::to_string(i), GameAttribute::Integer, true)));
			_baseAttrGrowth[attr.first].back()->setInteger(0);
		}
	}
}

void GameCharacter::loadDynamicData(SaveFileUtils::Status st)
{
	auto saveutils = SaveFileUtils::getInstance();
	SaveFileUtils::Status prevst = saveutils->getStatus();
	saveutils->setStatus(st);
	std::string id = saveutils->getStringInObject("id");
	if (!_id.empty() && id != _id) {
		throw std::invalid_argument("the dynamic object does not belong to this character");
	}
	else if (_id.empty()) {
		_id = id;
	}
	_member = saveutils->getBoolInObject("member");
	_level = saveutils->getIntegerInObject("level");
	_nowExp = saveutils->getIntegerInObject("exp");
	_growPoint = saveutils->getIntegerInObject("growPoint");

	auto util = GameDataUtils::getInstance();
	_dynamicAttr = util->getDynamicAttrs(_id);
	saveutils->setCurrentNodeInObject("dynamicAttrData");
	saveutils->loadIntoAttrs(*_dynamicAttr);
	saveutils->setStatus(st);

	auto baseattrs = util->getBaseAttrs(_id);
	saveutils->setCurrentNodeInObject("baseAttrGrowth");
	SaveFileUtils::Status attrdict = saveutils->getStatus();
	for (const auto &attr : *baseattrs) {
		saveutils->setStatus(attrdict);
		saveutils->setCurrentNodeInObject(attr.first);
		auto& growthList = _baseAttrGrowth[attr.first]
			= std::vector<std::unique_ptr<GameAttribute>>();
		for (int i = 0; i != 3; ++i) {
			growthList.push_back(
				saveutils->getAttributeInArray(i, GameAttribute::Integer));
			auto &finalbase = (*_finalBaseAttr)[attr.first];
			finalbase->setInteger(finalbase->getInteger() + growthList.back()->getInteger());
		}
	}
	saveutils->setStatus(st);

	saveutils->setStatus(prevst);
}
