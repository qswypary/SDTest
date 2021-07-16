#include "GameDataUtils.h"
#include "JsonAttributeTableParser.h"

std::shared_ptr<JsonAttributeTableParser> JsonAttributeTableParser::instance = nullptr;

JsonAttributeTableParser::JsonAttributeTableParser() = default;

std::shared_ptr<JsonAttributeTableParser> JsonAttributeTableParser::getInstance()
{
	if (!instance) {
		instance = std::make_shared<JsonAttributeTableParser>(JsonAttributeTableParser());
	}
	return instance;
}

std::unique_ptr<std::vector<std::unique_ptr<GameAttribute>>> 
	JsonAttributeTableParser::parseToAttrVec(
		const rapidjson::Value& value, GameAttribute::DataType type, 
		bool temp, std::string idprefix) const
{
	assert(value.IsObject());
	std::unique_ptr<std::vector<std::unique_ptr<GameAttribute>>> attrs;
	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		attrs->push_back(wrapAttr(it->name, it->value, type, temp, idprefix));
	}
	return std::move(attrs);
}

std::unique_ptr<std::map<std::string, std::unique_ptr<GameAttribute>>>
	JsonAttributeTableParser::parseToAttrMap(
		const rapidjson::Value& value, GameAttribute::DataType type, 
		bool temp, std::string idprefix) const
{
	assert(value.IsObject());
	std::unique_ptr<std::map<std::string, std::unique_ptr<GameAttribute>>> attrs
		(new std::map<std::string, std::unique_ptr<GameAttribute>>());
	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		auto attr = wrapAttr(it->name, it->value, type, temp, idprefix);
		(*attrs)[it->name.GetString()] = std::move(attr);
	}
	return attrs;
}

std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<GameAttribute>>>
	JsonAttributeTableParser::parseToAttrUnorderedMap(
		const rapidjson::Value& value, GameAttribute::DataType type, 
		bool temp, std::string idprefix) const
{
	assert(value.IsObject());
	std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<GameAttribute>>> attrs
		(new std::unordered_map<std::string, std::unique_ptr<GameAttribute>>());
	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		auto attr = wrapAttr(it->name, it->value, type, temp, idprefix);
		(*attrs)[it->name.GetString()] = std::move(attr);
	}
	return attrs;
}

int JsonAttributeTableParser::parseIntoAttrs(const rapidjson::Value& value, Maps::AttrMap& attrs) const
{
	assert(value.IsObject());
	int count = 0;
	auto util = GameDataUtils::getInstance();
	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		assert(it->name.IsString());
		auto gait = attrs.find(it->name.GetString());
		if (gait == attrs.end()) {
			continue;
		}
		const auto &ga = gait->second;
		switch (ga->getDataType())
		{
		case GameAttribute::Integer:
		case GameAttribute::Decimal2:
		case GameAttribute::Decimal4:
			assert(it->value.IsInt());
			ga->setInteger(it->value.GetInt());
			break;
		case GameAttribute::Double:
			assert(it->value.IsDouble());
			ga->setDouble(it->value.GetDouble());
			break;
		case GameAttribute::Text:
			assert(it->value.IsString());
			ga->setText(it->value.GetString());
			break;
		default:
			break;
		}
		++count;
	}
	return count;
}

int JsonAttributeTableParser::parseIntoAttrs(const rapidjson::Value& value, Maps::AttrWithMaxiumMap& attrs) const
{
	assert(value.IsObject());
	int count = 0;
	auto util = GameDataUtils::getInstance();
	for (auto it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
		assert(it->name.IsString());
		std::string shortid = util->getShortId(it->name.GetString());
		auto gait = attrs.find(shortid);
		if (gait == attrs.end()) {
			continue;
		}
		const auto& ga = gait->second;
		switch (ga->getDataType())
		{
		case GameAttribute::Integer:
		case GameAttribute::Decimal2:
		case GameAttribute::Decimal4:
			assert(it->value.IsInt());
			ga->setInteger(it->value.GetInt());
			break;
		case GameAttribute::Double:
			assert(it->value.IsDouble());
			ga->setDouble(it->value.GetDouble());
			break;
		case GameAttribute::Text:
		default:
			break;
		}
		++count;
	}
	return count;
}

std::unique_ptr<GameAttribute> JsonAttributeTableParser::wrapAttr(
	const rapidjson::Value& key, const rapidjson::Value& value, 
	GameAttribute::DataType type, bool temp, std::string idprefix) const
{
	assert(key.IsString());
	std::string skey = key.GetString();
	std::unique_ptr<GameAttribute> ga(new GameAttribute(idprefix + skey, type, temp));
	switch (type)
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		assert(value.IsInt());
		ga->setInteger(value.GetInt());
		break;
	case GameAttribute::Double:
		assert(value.IsDouble());
		ga->setDouble(value.GetDouble());
		break;
	case GameAttribute::Text:
		assert(value.IsString());
		ga->setText(value.GetString());
		break;
	default:
		break;
	}
	return ga;
}
