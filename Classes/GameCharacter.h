#pragma once
#include <unordered_map>
#include <json/document.h>
#include "GameAttributeWithMaxium.h"
#include "Maps.h"
#include "SaveFileUtils.h"

class GameCharacter
{
private:
	std::string _id;
	std::string _name;
	std::string _title;
	boolean _member = false;
	int _level = 1;
	int _nowExp = 0;
	int _nextLvExp = 10;
	int _growPoint = 0;
	std::string _avatarFilen;

	std::shared_ptr<Maps::AttrMap> _BASE_ATTR;
	std::shared_ptr<Maps::AttrMap> _OTHER_ATTR;
	std::shared_ptr<Maps::AttrMap> _BASE_ATTR_GROWTH_RATE;
	std::shared_ptr<std::vector<int>> _EXP;
	std::shared_ptr<std::vector<int>> _ALL_EXP;

	std::shared_ptr<Maps::AttrWithMaxiumMap> _dynamicAttr;
	std::shared_ptr<Maps::AttrMap> _finalBaseAttr;

	std::unordered_map<std::string, std::vector<std::unique_ptr<GameAttribute>>> _baseAttrGrowth;

public:
	GameCharacter(std::string id, boolean member = true, int level = 1);
	GameCharacter(const rapidjson::Value& staticData, const rapidjson::Value& info);
	GameCharacter(const rapidjson::Value& staticData, 
		const rapidjson::Value& info, SaveFileUtils::Status st);

	std::string getId() const;
	std::string getName() const;
	std::string getTitle() const;
	bool isMember() const;
	int getLevel() const;
	int getNowExp() const;
	int getNextLvExp() const;
	int getGrowPoint() const;
	std::string getAvatarFileName() const;
	std::shared_ptr<Maps::AttrMap> getBaseAttr() const;
	std::shared_ptr<Maps::AttrMap> getOtherAttr() const;
	std::shared_ptr<Maps::AttrMap> getBaseAttrGrothRate() const;
	std::shared_ptr<Maps::AttrWithMaxiumMap> getDynamicAttr() const;
	std::shared_ptr<Maps::AttrMap> getFinalBaseAttr() const;

	std::unique_ptr<Maps::StringMap<double>> getGrowth(const Maps::StringMap<double> growthItems) const;
	std::unique_ptr<Maps::StringMap<double>> grow(const Maps::StringMap<double> growthItems);
	
	void setGrowPoint(int growPoint);

	void saveDynamicData(SaveFileUtils::Status st);

private:
	void parseStaticData(const rapidjson::Value& staticData);
	void parseInfo(const rapidjson::Value& info);
	void initDynamicData();
	void loadDynamicData(SaveFileUtils::Status st);
};
