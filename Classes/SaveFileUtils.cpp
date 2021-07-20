#include "SaveFileUtils.h"
#include <json/filewritestream.h>
#include <json/filereadstream.h>
#include <json/writer.h>
#include "JsonAttributeTableParser.h"

std::shared_ptr<SaveFileUtils> SaveFileUtils::instance;

SaveFileUtils::SaveFileUtils() = default;

std::shared_ptr<SaveFileUtils> SaveFileUtils::getInstance()
{
	if (!instance) {
		instance = std::make_shared<SaveFileUtils>(SaveFileUtils());
		instance->clear();
	}
	return instance;
}

void SaveFileUtils::clear(bool isObj)
{
	_cacheRoot = new rapidjson::Document();
	if (isObj) {
		_cacheRoot->SetObject();
	}
	else {
		_cacheRoot->SetArray();
	}
	resetCurrentNode();
}

void SaveFileUtils::save(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "wb");
	char buffer[65536];
	rapidjson::FileWriteStream os(fp, buffer, sizeof(buffer));

	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	_cacheRoot->Accept(writer);

	fclose(fp);
}

void SaveFileUtils::load(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "rb");
	char buffer[65536];
	rapidjson::FileReadStream is(fp, buffer, sizeof(buffer));

	_cacheRoot->ParseStream(is);
	resetCurrentNode();

	fclose(fp);
}

void SaveFileUtils::resetCurrentNode()
{
	_current = _cacheRoot;
}

bool SaveFileUtils::isCurrentNodeObject() const
{
	return _current->IsObject();
}

bool SaveFileUtils::isCurrentNodeArray() const
{
	return _current->IsArray();
}

SaveFileUtils::Status SaveFileUtils::getStatus() const
{
	Status st;
	st.root = _cacheRoot;
	st.node = _current;
	return st;
}

void SaveFileUtils::setStatus(Status st)
{
	if (st.root != _cacheRoot) {
		throw std::invalid_argument("the status is out of time");
	}
	_current = st.node;
}

void SaveFileUtils::setBoolInObject(std::string key, bool value)
{
	rapidjson::Value v;
	v.SetBool(value);
	setValueInObject(key, &v);
}

void SaveFileUtils::setIntegerInObject(std::string key, int value)
{
	setValueInObject(key, &rapidjson::Value(value));
}

void SaveFileUtils::setDoubleInObject(std::string key, double value)
{
	setValueInObject(key, &rapidjson::Value(value));
}

void SaveFileUtils::setStringInObject(std::string key, std::string value)
{
	setValueInObject(key, &rapidjson::Value(value.c_str(), value.size()));
}

void SaveFileUtils::setAttributeInObject(std::string key, const GameAttribute& value)
{
	switch (value.getDataType())
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		setIntegerInObject(key, value.getInteger());
		break;
	case GameAttribute::Double:
		setDoubleInObject(key, value.getDouble());
		break;
	case GameAttribute::Text:
		setStringInObject(key, value.getText());
		break;
	default:
		break;
	}
}

void SaveFileUtils::setArrayInObject(std::string key)
{
	setValueInObject(key, &rapidjson::Value(rapidjson::kArrayType));
}

void SaveFileUtils::setObjectInObject(std::string key)
{
	setValueInObject(key, &rapidjson::Value(rapidjson::kObjectType));
}

bool SaveFileUtils::getBoolInObject(std::string key) const
{
	auto data = getValueInObject(key);
	return data->GetBool();
}

int SaveFileUtils::getIntegerInObject(std::string key) const
{
	auto data = getValueInObject(key);
	return data->GetInt();
}

double SaveFileUtils::getDoubleInObject(std::string key) const
{
	auto data = getValueInObject(key);
	return data->GetDouble();
}

std::string SaveFileUtils::getStringInObject(std::string key) const
{
	auto data = getValueInObject(key);
	return data->GetString();
}

std::unique_ptr<GameAttribute> SaveFileUtils::getAttributeInObject(
	std::string key, GameAttribute::DataType dataType) const
{
	std::unique_ptr<GameAttribute> ga =
		std::make_unique<GameAttribute>(
			GameAttribute(key, dataType, true));
	switch (dataType)
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		ga->setInteger(getIntegerInObject(key));
		break;
	case GameAttribute::Double:
		ga->setDouble(getDoubleInObject(key));
		break;
	case GameAttribute::Text:
		ga->setText(getStringInObject(key));
		break;
	default:
		break;
	}
	return ga;
}

std::unique_ptr<Maps::AttrMap> SaveFileUtils::getAllAttributes(GameAttribute::DataType dataType, bool temp, std::string idprefix) const
{
	auto parser = JsonAttributeTableParser::getInstance();
	return parser->parseToAttrUnorderedMap(*_current, dataType, temp, idprefix);
}

int SaveFileUtils::loadIntoAttrs(Maps::AttrMap& attrs) const
{
	auto parser = JsonAttributeTableParser::getInstance();
	return parser->parseIntoAttrs(*_current, attrs);
}

int SaveFileUtils::loadIntoAttrs(Maps::AttrWithMaxiumMap& attrs) const
{
	auto parser = JsonAttributeTableParser::getInstance();
	return parser->parseIntoAttrs(*_current, attrs);
}

void SaveFileUtils::setCurrentNodeInObject(std::string key)
{
	assert(isCurrentNodeObject());
	auto target = _current->FindMember(key.c_str());
	if (target == _current->MemberEnd()) {
		throw std::invalid_argument("cannot find '" + key + "' in current node's children");
	}
	_current = &(target->value);
}

void SaveFileUtils::appendBoolInArray(bool value)
{
	rapidjson::Value v;
	v.SetBool(value);
	appendValueInArray(&v);
}

void SaveFileUtils::appendIntegerInArray(int value)
{
	appendValueInArray(&rapidjson::Value(value));
}

void SaveFileUtils::appendDoubleInArray(double value)
{
	appendValueInArray(&rapidjson::Value(value));
}

void SaveFileUtils::appendStringInArray(std::string value)
{
	appendValueInArray(&rapidjson::Value(value.c_str(), value.size()));
}

void SaveFileUtils::appendAttributeInArray(const GameAttribute& value)
{
	switch (value.getDataType())
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		appendIntegerInArray(value.getInteger());
		break;
	case GameAttribute::Double:
		appendDoubleInArray(value.getDouble());
		break;
	case GameAttribute::Text:
		appendStringInArray(value.getText());
		break;
	default:
		break;
	}
}

void SaveFileUtils::appendArrayInArray()
{
	appendValueInArray(&rapidjson::Value(rapidjson::kArrayType));
}

void SaveFileUtils::appendObjectInArray()
{
	appendValueInArray(&rapidjson::Value(rapidjson::kObjectType));
}

void SaveFileUtils::setBoolInArray(int index, bool value)
{
	rapidjson::Value v;
	v.SetBool(value);
	setValueInArray(index, &v);
}

void SaveFileUtils::setIntegerInArray(int index, int value)
{
	setValueInArray(index, &rapidjson::Value(value));
}

void SaveFileUtils::setDoubleInArray(int index, double value)
{
	setValueInArray(index, &rapidjson::Value(value));
}

void SaveFileUtils::setStringInArray(int index, std::string value)
{
	setValueInArray(index, &rapidjson::Value(value.c_str(), value.size()));
}

void SaveFileUtils::setAttributeInArray(int index, const GameAttribute& value)
{
	switch (value.getDataType())
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		setIntegerInArray(index, value.getInteger());
		break;
	case GameAttribute::Double:
		setDoubleInArray(index, value.getDouble());
		break;
	case GameAttribute::Text:
		setStringInArray(index, value.getText());
		break;
	default:
		break;
	}
}

void SaveFileUtils::setArrayInArray(int index)
{
	setValueInArray(index, &rapidjson::Value(rapidjson::kArrayType));
}

void SaveFileUtils::setObjectInArray(int index)
{
	setValueInArray(index, &rapidjson::Value(rapidjson::kObjectType));
}

bool SaveFileUtils::getBoolInArray(int index) const
{
	auto data = getValueInArray(index);
	return data->GetBool();
}

int SaveFileUtils::getIntegerInArray(int index) const
{
	auto data = getValueInArray(index);
	return data->GetInt();
}

double SaveFileUtils::getDoubleInArray(int index) const
{
	auto data = getValueInArray(index);
	return data->GetDouble();
}

std::string SaveFileUtils::getStringInArray(int index) const
{
	auto data = getValueInArray(index);
	return data->GetString();
}

std::unique_ptr<GameAttribute> SaveFileUtils::getAttributeInArray(
	int index, GameAttribute::DataType dataType) const
{
	std::unique_ptr<GameAttribute> ga =
		std::make_unique<GameAttribute>(
			GameAttribute(std::to_string(index), dataType, true));
	switch (dataType)
	{
	case GameAttribute::Integer:
	case GameAttribute::Decimal2:
	case GameAttribute::Decimal4:
		ga->setInteger(getIntegerInArray(index));
		break;
	case GameAttribute::Double:
		ga->setDouble(getDoubleInArray(index));
		break;
	case GameAttribute::Text:
		ga->setText(getStringInArray(index));
		break;
	default:
		break;
	}
	return ga;
}

void SaveFileUtils::setCurrentNodeInArray(int index)
{
	assert(isCurrentNodeArray());
	index = checkIndex(index);
	_current = &(*_current)[index];
}

void SaveFileUtils::setValueInObject(std::string key, rapidjson::Value* value)
{
	assert(isCurrentNodeObject());
	rapidjson::Value name;
	name.SetString(key.c_str(), _cacheRoot->GetAllocator());
	if (value->IsString()) {
		value = &rapidjson::Value().SetString(value->GetString(), _cacheRoot->GetAllocator());
	}
	_current->AddMember(name, *value, _cacheRoot->GetAllocator());
}

void SaveFileUtils::setValueInArray(int index, rapidjson::Value* value)
{
	assert(isCurrentNodeArray());
	index = checkIndex(index);
	auto now = _current->GetArray().Begin() + index;
	now->CopyFrom(*value, _cacheRoot->GetAllocator());
}

void SaveFileUtils::appendValueInArray(rapidjson::Value* value)
{
	assert(isCurrentNodeArray());
	if (value->IsString()) {
		value = &rapidjson::Value().SetString(value->GetString(), _cacheRoot->GetAllocator());
	}
	_current->GetArray().PushBack(*value, _cacheRoot->GetAllocator());
}

rapidjson::Value* SaveFileUtils::getValueInObject(std::string key) const
{
	assert(isCurrentNodeObject());
	auto target = _current->FindMember(key.c_str());
	if (target == _current->MemberEnd()) {
		throw std::invalid_argument("cannot find '" + key + "' in current node's children");
	}
	return &(target->value);
}

rapidjson::Value* SaveFileUtils::getValueInArray(int index) const
{
	assert(isCurrentNodeArray());
	index = checkIndex(index);
	auto now = _current->GetArray().Begin() + index;
	return now;
}

int SaveFileUtils::checkIndex(int index) const
{
	int size = _current->Size();
	if (index < 0) {
		index += size;
	}
	if (index < 0 || index >= size) {
		throw std::out_of_range("the index of node is out of range");
	}
	return index;
}
