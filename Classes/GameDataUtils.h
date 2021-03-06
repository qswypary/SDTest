#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <json/document.h>
#include "GameAttribute.h"
#include "GameAttributeWithMaxium.h"
#include "Maps.h"

class GameDataUtils
{
public:
	class AssoAttr {
	public:
		std::string id;
		int assoPerc;
	};

private:
	static std::shared_ptr<GameDataUtils> instance;

	const std::string _CHAR_DATA_CFG_FILEN = "json/charDataConfig.json";
	const char _ATTR_ID_SP = ':';
	const std::string _ATTR_NAME_SP = " - ";
	Maps::StringMap<GameAttribute::DataType> _TO_DATA_TYPE;
	int _maxLv = 160;
	std::shared_ptr<Maps::StringMap<std::string>> _attrName;
	std::unique_ptr<Maps::AttrMap> _baseAttrProto;
	std::unique_ptr<Maps::AttrMap> _otherAttrProto;
	std::unique_ptr<Maps::AttrMap> _dynamicAttrProto;
	std::shared_ptr<std::vector<int>> _expTable;
	std::shared_ptr<std::vector<int>> _allExpTable;
	std::shared_ptr<std::vector<std::string>> _growthItem;
	std::shared_ptr<Maps::StringMultiMap<AssoAttr>> _growthItemAssoAttr;
	std::shared_ptr<Maps::StringMap<std::string>> _dynamicAttrBinding;

	GameDataUtils();

public:
	static std::shared_ptr<GameDataUtils> getInstance();

	char getAttrIdSeparator() const;
	std::string getAttrNameSeparator() const;

	void loadCharDataConfig();
	std::pair<std::shared_ptr<std::vector<int>>, std::shared_ptr<std::vector<int>>>
		parseExpTable(const rapidjson::Value& table) const;
	std::unique_ptr<Maps::AttrMap> parseAttrTable(const rapidjson::Value& table) const;

	std::shared_ptr<Maps::StringMap<std::string>> getAttrNames() const;
	std::string getShortId(std::string id) const;
	std::string getAttrName(std::string id) const;
	std::string getComplexAttrName(std::string id, bool ignoreUnknown = false) const;
	std::string getMinAttrName(std::string id) const;
	void editAttrName(GameAttribute& attr, bool complex = true, bool ignoreUnknown = false) const;

	std::unique_ptr<Maps::AttrMap> getBaseAttrs(std::string charid) const;
	std::unique_ptr<Maps::AttrMap> getOtherAttrs(std::string charid) const;
	std::unique_ptr<Maps::AttrWithMaxiumMap> getDynamicAttrs(std::string charid) const;
	GameAttribute getBaseAttr(std::string charid, std::string attrid) const;
	GameAttribute getOtherAttr(std::string charid, std::string attrid) const;
	GameAttributeWithMaxium getDynamicAttr(std::string charid, std::string attrid) const;

	int getPossibleMaxLevel() const;
	std::shared_ptr<std::vector<int>> getExpTable() const;
	std::shared_ptr<std::vector<int>> getAllExpTable() const;
	int getExpToLevel(int lv) const;
	int getAllExpToLevel(int lv) const;

	std::shared_ptr<std::vector<std::string>> getGrowthItems() const;
	std::shared_ptr<std::vector<std::string>> getGrowthItemNames() const;
	std::shared_ptr<Maps::StringMultiMap<AssoAttr>> getGrowthTable() const;
	std::unique_ptr<std::vector<AssoAttr>> getAssoAttrs(std::string item) const;

	// attr : ?????????????????????
	// growItem : ?????????????????????????????????????????? map
	double grow(const GameAttribute& attr, 
		const Maps::StringMap<double>& growItem) const;

	// attrs : ?????????????????????
	// growItem : ?????????????????????????????????????????? map
	std::unique_ptr<Maps::StringMap<double>> grow(const Maps::StringMap<std::unique_ptr<GameAttribute>>& attrs,
		const Maps::StringMap<double>& growItem) const;

	std::shared_ptr<Maps::StringMap<std::string>> getDynamicAttrBindings() const;
	std::string getBoundAttrId(std::string id) const;
	GameAttributeWithMaxium generateAttrWithMax(const GameAttribute& rawAttr) const;

	// ???????????? ID ????????????????????????
	// ??? "atk" ??? "ch0001:atk" ???????????????
	// "magicDmgValidity:light" ??? "ch0012:magicDmgValidity:light" ???????????????
	// rawid : ??????????????? ID
	// trueid : ???????????????????????????????????? ID
	bool isIdsAgree(std::string rawid, std::string trueid) const;

private:
	std::pair<std::string, std::string> getBinding(std::string id) const;

	GameAttribute createAttrFromProto(std::string charid, const GameAttribute& proto) const;
	GameAttributeWithMaxium createDynamicAttrFromProto(std::string charid, const GameAttribute& proto) const;
};

