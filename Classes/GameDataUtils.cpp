#include "GameDataUtils.h"
#include "JsonAttributeTableParser.h"

std::shared_ptr<GameDataUtils> GameDataUtils::instance;

GameDataUtils::GameDataUtils() {
	_TO_DATA_TYPE["Integer"] = GameAttribute::Integer;
	_TO_DATA_TYPE["Decimal2"] = GameAttribute::Decimal2;
	_TO_DATA_TYPE["Decimal4"] = GameAttribute::Decimal4;
	_TO_DATA_TYPE["Double"] = GameAttribute::Double;
	_TO_DATA_TYPE["Text"] = GameAttribute::Text;
}

std::shared_ptr<GameDataUtils> GameDataUtils::getInstance()
{
	if (!instance) {
		instance = std::make_shared<GameDataUtils>(GameDataUtils());
		instance->loadCharDataConfig();
	}
	return instance;
}

char GameDataUtils::getAttrIdSeparator() const
{
	return _ATTR_ID_SP;
}

std::string GameDataUtils::getAttrNameSeparator() const
{
	return _ATTR_NAME_SP;
}

void GameDataUtils::loadCharDataConfig()
{
	auto fileutils = cocos2d::FileUtils::getInstance();
	std::string data = fileutils->getStringFromFile(_CHAR_DATA_CFG_FILEN);
	rapidjson::Document document;
	document.Parse(data.c_str());

	assert(document.IsObject());
	auto parser = JsonAttributeTableParser::getInstance();

	// 解析 attrName 的 Object
	auto tempAttrName = parser->parseToAttrUnorderedMap(
		document.FindMember("attrName")->value, 
		GameAttribute::Text);
	_attrName = std::make_shared<Maps::StringMap<std::string>>(Maps::StringMap<std::string>());
	for (const auto &item : *tempAttrName) {
		(*_attrName)[item.first] = item.second->getText();
	}

	// 解析 charAttr 的 Object，创建原型
	const auto &attrVal = document.FindMember("charAttr")->value;
	assert(attrVal.IsObject());
	const auto& baseAttrVal = attrVal.FindMember("baseAttr")->value,
		&otherAttrVal = attrVal.FindMember("otherAttr")->value,
		&dynamicAttrVal = attrVal.FindMember("dynamicAttr")->value;
	_baseAttrProto = parseAttrTable(baseAttrVal);
	_otherAttrProto = parseAttrTable(otherAttrVal);
	_dynamicAttrProto = parseAttrTable(dynamicAttrVal);

	// 解析 expTable 的 Object
	// expTable 中如果有缺失的等级数据，默认用一次函数填充
	auto expPair = parseExpTable(document.FindMember("expTable")->value);
	_expTable = expPair.first;
	_allExpTable = expPair.second;
	_maxLv = _expTable->size();

	// 解析 growthItem 的 Object
	const auto &growthItemVal = document.FindMember("growthItem")->value;
	assert(growthItemVal.IsObject());
	_growthItem = std::make_shared<std::vector<std::string>>(std::vector<std::string>());
	_growthItemAssoAttr = std::make_shared<Maps::StringMultiMap<AssoAttr>>(Maps::StringMultiMap<AssoAttr>());
	for (auto it = growthItemVal.MemberBegin();
		it != growthItemVal.MemberEnd();
		++it) {
		assert(it->name.IsString());
		std::string gi = it->name.GetString();
		_growthItem->push_back(gi);

		assert(it->value.IsObject());
		for (auto subit = it->value.MemberBegin();
			subit != it->value.MemberEnd();
			++subit) {
			assert(subit->name.IsString());
			assert(subit->value.IsInt());
			AssoAttr aa = { subit->name.GetString(), subit->value.GetInt() };
			_growthItemAssoAttr->insert(std::make_pair(gi, aa));
		}
	}

	// 解析 dynamicAttrBinding 的 Object
	auto tempAttrBinding = parser->parseToAttrUnorderedMap(
		document.FindMember("dynamicAttrBinding")->value,
		GameAttribute::Text);
	_dynamicAttrBinding = std::make_shared<Maps::StringMap<std::string>>(Maps::StringMap<std::string>());
	for (const auto& item : *tempAttrBinding) {
		(*_dynamicAttrBinding)[item.first] = item.second->getText();
	}
}

std::pair<std::shared_ptr<std::vector<int>>, std::shared_ptr<std::vector<int>>> GameDataUtils::parseExpTable(const rapidjson::Value& table) const
{
	auto parser = JsonAttributeTableParser::getInstance();
	auto tempExpTable = parser->parseToAttrUnorderedMap(table, GameAttribute::Integer);
	int sum = 0;
	auto expTable = std::make_shared<std::vector<int>>(std::vector<int>());
	auto allExpTable = std::make_shared<std::vector<int>>(std::vector<int>());
	for (int level = 0; level != _maxLv; ++level) {
		auto it = tempExpTable->find(std::to_string(level + 1));
		if (it != tempExpTable->end()) {
			expTable->push_back(it->second->getInteger());
			sum += expTable->back();
			allExpTable->push_back(sum);
		}
		else {
			int originlv = level;
			int begval = (level == 0) ? 0 : (*expTable)[level - 1];
			while (level != _maxLv && it == tempExpTable->end()) {
				++level;
				it = tempExpTable->find(std::to_string(level + 1));
			}
			if (level == _maxLv) {
				break;
			}
			int endval = it->second->getInteger();
			double step = (endval - begval) / double(level - originlv + 1);
			for (int againlv = originlv; againlv != level; ++againlv) {
				expTable->push_back(begval + step * (againlv - originlv + 1));
				sum += expTable->back();
				allExpTable->push_back(sum);
			}
			expTable->push_back(endval);
			sum += expTable->back();
			allExpTable->push_back(sum);
		}
	}
	return std::make_pair<std::shared_ptr<std::vector<int>>, std::shared_ptr<std::vector<int>>>
		(std::move(expTable), std::move(allExpTable));
}

std::unique_ptr<Maps::AttrMap> GameDataUtils::parseAttrTable(const rapidjson::Value& table) const
{
	std::unique_ptr<Maps::AttrMap> attrmap =
		std::make_unique<Maps::AttrMap>(Maps::AttrMap());
	assert(table.IsArray());
	for (const auto& item : table.GetArray()) {
		assert(item.IsObject());
		const auto& idVal = item.FindMember("id")->value;
		assert(idVal.IsString());
		const auto& dtVal = item.FindMember("dataType")->value;
		assert(dtVal.IsString());
		GameAttribute::DataType dt = _TO_DATA_TYPE.at(dtVal.GetString());

		std::unique_ptr<GameAttribute> ga =
			std::make_unique<GameAttribute>(GameAttribute(
				idVal.GetString(), dt, true
			));

		auto nit = item.FindMember("name");
		if (nit != item.MemberEnd()) {
			const auto& nVal = nit->value;
			assert(nVal.IsString());
			ga->setName(nVal.GetString());
		}
		else {
			editAttrName(*ga, true, true);
		}

		(*attrmap)[idVal.GetString()] = std::move(ga);
	}
	return attrmap;
}

std::shared_ptr<Maps::StringMap<std::string>> GameDataUtils::getAttrNames() const
{
	return _attrName;
}

std::string GameDataUtils::getShortId(std::string id) const
{
	auto lastsp = std::find(id.rbegin(), id.rend(), _ATTR_ID_SP);
	return std::string(lastsp.base(), id.end());
}

std::string GameDataUtils::getAttrName(std::string id) const
{
	auto it = _attrName->find(id);
	if (it == _attrName->end()) {
		throw std::invalid_argument("cannot find the name of this id");
	}
	return it->second;
}

std::string GameDataUtils::getComplexAttrName(std::string id, bool ignoreUnknown) const
{
	std::string res;
	bool first = true;
	for (auto it = id.begin(); it != id.end(); ) {
		auto nextsp = std::find(it, id.end(), _ATTR_ID_SP);
		std::string uniname;
		try {
			uniname = getAttrName(std::string(it, nextsp));
		}
		catch (std::invalid_argument e) {
			if (!ignoreUnknown) {
				uniname = std::string(it, nextsp);
			}
		}
		if (!uniname.empty()) {
			if (first) {
				first = false;
			}
			else {
				res += _ATTR_NAME_SP;
			}
			res += uniname;
		}
		if (nextsp == id.end()) {
			break;
		}
		it = nextsp + 1;
	}
	return res;
}

std::string GameDataUtils::getMinAttrName(std::string id) const
{
	return getAttrName(getShortId(id));
}

void GameDataUtils::editAttrName(GameAttribute& attr, bool complex, bool ignoreUnknown) const
{
	attr.setName(complex ? 
		getComplexAttrName(attr.getId(), ignoreUnknown) 
		: getMinAttrName(attr.getId()));
}

std::unique_ptr<Maps::AttrMap> GameDataUtils::getBaseAttrs(std::string charid) const
{
	std::unique_ptr<Maps::AttrMap> am = 
		std::make_unique<Maps::AttrMap>(Maps::AttrMap());
	for (const auto &proto : *_baseAttrProto) {
		auto attr = std::make_unique<GameAttribute>(getBaseAttr(charid, proto.first));
		(*am)[proto.first] = std::move(attr);
	}
	return am;
}

std::unique_ptr<Maps::AttrMap> GameDataUtils::getOtherAttrs(std::string charid) const
{
	std::unique_ptr<Maps::AttrMap> am =
		std::make_unique<Maps::AttrMap>(Maps::AttrMap());
	for (const auto& proto : *_otherAttrProto) {
		auto attr = std::make_unique<GameAttribute>(getOtherAttr(charid, proto.first));
		(*am)[proto.first] = std::move(attr);
	}
	return am;
}

std::unique_ptr<Maps::AttrWithMaxiumMap> GameDataUtils::getDynamicAttrs(std::string charid) const
{
	std::unique_ptr<Maps::AttrWithMaxiumMap> am =
		std::make_unique<Maps::AttrWithMaxiumMap>(Maps::AttrWithMaxiumMap());
	for (const auto& proto : *_dynamicAttrProto) {
		auto attr = std::make_unique<GameAttributeWithMaxium>(getDynamicAttr(charid, proto.first));
		(*am)[proto.first] = std::move(attr);
	}
	return am;
}

GameAttribute GameDataUtils::getBaseAttr(std::string charid, std::string attrid) const
{
	return createAttrFromProto(charid, *(_baseAttrProto->at(attrid)));
}

GameAttribute GameDataUtils::getOtherAttr(std::string charid, std::string attrid) const
{
	return createAttrFromProto(charid, *(_otherAttrProto->at(attrid)));
}

GameAttributeWithMaxium GameDataUtils::getDynamicAttr(std::string charid, std::string attrid) const
{
	return createDynamicAttrFromProto(charid, *(_dynamicAttrProto->at(attrid)));
}

int GameDataUtils::getPossibleMaxLevel() const
{
	return _maxLv;
}

std::shared_ptr<std::vector<int>> GameDataUtils::getExpTable() const
{
	return _expTable;
}

std::shared_ptr<std::vector<int>> GameDataUtils::getAllExpTable() const
{
	return _allExpTable;
}

int GameDataUtils::getExpToLevel(int lv) const
{
	if (lv <= 0 || lv > _maxLv) {
		throw std::out_of_range("cannot reach such level");
	}
	return (*_expTable)[lv];
}

int GameDataUtils::getAllExpToLevel(int lv) const
{
	if (lv <= 0 || lv > _maxLv) {
		throw std::out_of_range("cannot reach such level");
	}
	return (*_allExpTable)[lv];
}

std::shared_ptr<std::vector<std::string>> GameDataUtils::getGrowthItems() const
{
	return _growthItem;
}

std::shared_ptr<std::vector<std::string>> GameDataUtils::getGrowthItemNames() const
{
	auto vec = std::make_shared<std::vector<std::string>>(std::vector<std::string>());
	for (const auto &item : *_growthItem) {
		vec->push_back(getAttrName(item));
	}
	return vec;
}

std::shared_ptr<std::unordered_multimap<std::string, GameDataUtils::AssoAttr>> GameDataUtils::getGrowthTable() const
{
	return _growthItemAssoAttr;
}

std::unique_ptr<std::vector<GameDataUtils::AssoAttr>> GameDataUtils::getAssoAttrs(std::string item) const
{
	auto itpair = _growthItemAssoAttr->equal_range(item);
	if (itpair.first == _growthItemAssoAttr->end()) {
		throw std::invalid_argument("cannot find the associated attributes of this item");
	}
	std::unique_ptr<std::vector<AssoAttr>> res(new std::vector<AssoAttr>);
	for (auto it = itpair.first; it != itpair.second; ++it) {
		res->push_back(it->second);
	}
	return res;
}

double GameDataUtils::grow(const GameAttribute& attr, const Maps::StringMap<double>& growItem) const
{
	std::string attrid = attr.getId();
	double growsum = 0.0;
	for (auto item : growItem) {
		// 找出该项目所影响的属性
		auto itpair = _growthItemAssoAttr->equal_range(item.first);
		for (auto it = itpair.first; it != itpair.second; ++it) {
			// 检查是否影响 attr
			std::string assoid = it->second.id;
			if (isIdsAgree(assoid, attrid)) {
				// 影响到 attr，计算成长数值
				growsum += it->second.assoPerc * item.second / 100.0;
			}
		}
	}
	return growsum;
}

std::unique_ptr<Maps::StringMap<double>> GameDataUtils::grow(
	const Maps::StringMap<std::unique_ptr<GameAttribute>>& attrs, const Maps::StringMap<double>& growItem) const
{
	std::unique_ptr<Maps::StringMap<double>> growsummap(new Maps::StringMap<double>());
	for (auto item : growItem) {
		// 找出该项目所影响的属性
		auto itpair = _growthItemAssoAttr->equal_range(item.first);
		for (auto it = itpair.first; it != itpair.second; ++it) {
			// 检查是否影响 attrs 中的属性
			std::string assoid = it->second.id;
			auto findit = attrs.cbegin();
			for (; findit != attrs.cend(); ++findit) {
				if (isIdsAgree(assoid, findit->first)) {
					break;
				}
			}
			if (findit != attrs.end()) {
				// 影响到 attrs 中的属性，计算成长数值
				std::string attrid = findit->first;
				if (growsummap->find(attrid) == growsummap->end()) {
					(*growsummap)[attrid] = 0.0;
				}
				(*growsummap)[attrid] += it->second.assoPerc * item.second / 100.0;
			}
		}
	}
	
	return growsummap;
}

std::shared_ptr<Maps::StringMap<std::string>> GameDataUtils::getDynamicAttrBindings() const
{
	return _dynamicAttrBinding;
}

std::string GameDataUtils::getBoundAttrId(std::string id) const
{
	auto pr = getBinding(id);
	return id.substr(0, id.size() - pr.first.size()) + pr.second;
}

GameAttributeWithMaxium GameDataUtils::generateAttrWithMax(const GameAttribute& rawAttr) const
{
	std::string id = rawAttr.getId();
	std::string bound = getBoundAttrId(id);

	GameAttributeWithMaxium gawm(rawAttr.getId(), rawAttr.getName(), rawAttr.getDataType(), bound);

	switch (rawAttr.getDataType())
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		gawm.setInteger(rawAttr.getInteger());
		break;
	case GameAttribute::Double:
		gawm.setDouble(rawAttr.getDouble());
		break;
	case GameAttribute::Text:
		throw std::invalid_argument("cannot generate text attribute");
		break;
	}

	return gawm;
}

bool GameDataUtils::isIdsAgree(std::string rawid, std::string trueid) const
{
	if (rawid.size() > trueid.size()) {
		//throw std::invalid_argument("too short true id");
		return false;
	}
	if (rawid == trueid) {
		return true;
	}
	auto rawit = rawid.rbegin();
	auto trueit = trueid.rbegin();
	while (rawit != rawid.rend()) {
		if (*rawit != *trueit) {
			break;
		}
		++rawit; ++trueit;
	}
	return rawit == rawid.rend() && (trueit == trueid.rend() || *trueit == _ATTR_ID_SP);
}

std::pair<std::string, std::string> GameDataUtils::getBinding(std::string id) const
{
	auto it = _dynamicAttrBinding->cbegin();
	for (; it != _dynamicAttrBinding->cend(); ++it) {
		if (isIdsAgree(it->first, id)) {
			break;
		}
	}
	if (it == _dynamicAttrBinding->end()) {
		throw std::invalid_argument("cannot find the binding of this attribute id");
	}
	return *it;
}

GameAttribute GameDataUtils::createAttrFromProto(std::string charid, const GameAttribute& proto) const
{
	GameAttribute ga(charid + _ATTR_ID_SP + proto.getId(), proto.getName(), proto.getDataType());
	return ga;
}

GameAttributeWithMaxium GameDataUtils::createDynamicAttrFromProto(std::string charid, const GameAttribute& proto) const
{
	GameAttribute ga = createAttrFromProto(charid, proto);
	return generateAttrWithMax(ga);
}
