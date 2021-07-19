#pragma once
#include <memory>
#include <json/document.h>
#include "GameAttribute.h"

class JsonSaveFileUtils {
private:
	static std::shared_ptr<JsonSaveFileUtils> instance;

	rapidjson::Document* _cacheRoot;
	rapidjson::Value* _current;

	JsonSaveFileUtils();

public:
	static std::shared_ptr<JsonSaveFileUtils> getInstance();

	// 缓存处理

	void clear();
	void save(std::string filename);

	// 可通过设置“当前节点”，对 JSON 树结构的不同部位进行操作

	void resetCurrentNode();
	bool isCurrentNodeObject() const;
	bool isCurrentNodeArray() const;

	// 当前节点为 Object 可用接口：

	void setBoolInObject(std::string key, bool value);
	void setIntegerInObject(std::string key, int value);
	void setDoubleInObject(std::string key, double value);
	void setStringInObject(std::string key, std::string value);
	void setAttributeInObject(std::string key, const GameAttribute& value);

	bool getBoolInObject(std::string key);
	int getIntegerInObject(std::string key);
	double getDoubleInObject(std::string key);
	std::string getStringInObject(std::string key);
	std::unique_ptr<GameAttribute> getAttributeInObject(
		std::string key, GameAttribute::DataType dataType);

	void setCurrentNodeInObject(std::string key);

	// 当前节点为 Array 可用接口

	void appendBoolInArray(bool value);
	void appendIntegerInArray(int value);
	void appendDoubleInArray(double value);
	void appendStringInArray(std::string value);
	void appendAttributeInArray(const GameAttribute& value);

	void setBoolInArray(int index, bool value);
	void setIntegerInArray(int index, int value);
	void setDoubleInArray(int index, double value);
	void setStringInArray(int index, std::string value);
	void setAttributeInArray(int index, const GameAttribute& value);

	bool getBoolInArray(int index);
	int getIntegerInArray(int index);
	double getDoubleInArray(int index);
	std::string getStringInArray(int index);
	std::unique_ptr<GameAttribute> getAttributeInArray(
		int index, GameAttribute::DataType dataType);

	void setCurrentNodeInArray(int index);

private:
	void setValueInObject(std::string key, rapidjson::Value* value);
	void setValueInArray(int index, rapidjson::Value* value);
	void appendValueInArray(rapidjson::Value* value);
	rapidjson::Value* getValueInObject(std::string key);
	rapidjson::Value* getValueInArray(int index);

	int checkIndex(int index) const;
};