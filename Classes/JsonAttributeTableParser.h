#pragma once
#include <memory>
#include <json/document.h>
#include "GameAttribute.h"
#include "Maps.h"

class JsonAttributeTableParser
{
private:
	static std::shared_ptr<JsonAttributeTableParser> instance;

	JsonAttributeTableParser();

public:
	static std::shared_ptr<JsonAttributeTableParser> getInstance();

	std::unique_ptr<std::vector<std::unique_ptr<GameAttribute>>> parseToAttrVec(
		const rapidjson::Value& value, GameAttribute::DataType type, 
		bool temp = true, std::string idprefix = "") const;
	std::unique_ptr<std::map<std::string, std::unique_ptr<GameAttribute>>> parseToAttrMap(
		const rapidjson::Value& value, GameAttribute::DataType type, 
		bool temp = true, std::string idprefix = "") const;
	std::unique_ptr<std::unordered_map<std::string, std::unique_ptr<GameAttribute>>> parseToAttrUnorderedMap(
		const rapidjson::Value& value, GameAttribute::DataType type, 
		bool temp = true, std::string idprefix = "") const;

	// 根据给定的属性 map，解析 JSON value，并装配回属性 map 中
	// 返回装配成功的属性个数
	int parseIntoAttrs(const rapidjson::Value& value, Maps::AttrMap& attrs) const;
	int parseIntoAttrs(const rapidjson::Value& value, Maps::AttrWithMaxiumMap& attrs) const;

private:
	std::unique_ptr<GameAttribute> wrapAttr(
		const rapidjson::Value& key, const rapidjson::Value& value, 
		GameAttribute::DataType type, bool temp = true, std::string idprefix = "") const;
};

