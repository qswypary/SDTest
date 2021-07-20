#pragma once
#include <memory>
#include <json/document.h>
#include "GameAttribute.h"
#include "Maps.h"

class SaveFileUtils {
public:
	class Status {
	private:
		friend class SaveFileUtils;
		rapidjson::Document* root;
		rapidjson::Value* node;
	};
private:
	static std::shared_ptr<SaveFileUtils> instance;

	rapidjson::Document* _cacheRoot;
	rapidjson::Value* _current;

	SaveFileUtils();

public:
	static std::shared_ptr<SaveFileUtils> getInstance();

	// 缓存处理

	void clear(bool isObj = true);
	void save(std::string filename);
	void load(std::string filename);

	// 可通过设置“当前节点”，对 JSON 树结构的不同部位进行操作

	void resetCurrentNode();
	bool isCurrentNodeObject() const;
	bool isCurrentNodeArray() const;

	Status getStatus() const;
	void setStatus(Status st);

	// 当前节点为 Object 可用接口：

	void setBoolInObject(std::string key, bool value);
	void setIntegerInObject(std::string key, int value);
	void setDoubleInObject(std::string key, double value);
	void setStringInObject(std::string key, std::string value);
	void setAttributeInObject(std::string key, const GameAttribute& value);
	void setArrayInObject(std::string key);
	void setObjectInObject(std::string key);

	bool getBoolInObject(std::string key) const;
	int getIntegerInObject(std::string key) const;
	double getDoubleInObject(std::string key) const;
	std::string getStringInObject(std::string key) const;
	std::unique_ptr<GameAttribute> getAttributeInObject(
		std::string key, GameAttribute::DataType dataType) const;

	std::unique_ptr<Maps::AttrMap> getAllAttributes(
		GameAttribute::DataType dataType,
		bool temp = true, std::string idprefix = "") const;
	int loadIntoAttrs(Maps::AttrMap& attrs) const;
	int loadIntoAttrs(Maps::AttrWithMaxiumMap& attrs) const;

	void setCurrentNodeInObject(std::string key);

	// 当前节点为 Array 可用接口

	void appendBoolInArray(bool value);
	void appendIntegerInArray(int value);
	void appendDoubleInArray(double value);
	void appendStringInArray(std::string value);
	void appendAttributeInArray(const GameAttribute& value);
	void appendArrayInArray();
	void appendObjectInArray();

	void setBoolInArray(int index, bool value);
	void setIntegerInArray(int index, int value);
	void setDoubleInArray(int index, double value);
	void setStringInArray(int index, std::string value);
	void setAttributeInArray(int index, const GameAttribute& value);
	void setArrayInArray(int index);
	void setObjectInArray(int index);

	bool getBoolInArray(int index) const;
	int getIntegerInArray(int index) const;
	double getDoubleInArray(int index) const;
	std::string getStringInArray(int index) const;
	std::unique_ptr<GameAttribute> getAttributeInArray(
		int index, GameAttribute::DataType dataType) const;

	void setCurrentNodeInArray(int index);

private:
	void setValueInObject(std::string key, rapidjson::Value* value);
	void setValueInArray(int index, rapidjson::Value* value);
	void appendValueInArray(rapidjson::Value* value);
	rapidjson::Value* getValueInObject(std::string key) const;
	rapidjson::Value* getValueInArray(int index) const;

	int checkIndex(int index) const;
};