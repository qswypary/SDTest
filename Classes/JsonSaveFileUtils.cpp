#include "JsonSaveFileUtils.h"
#include <json/filewritestream.h>
#include <json/writer.h>

std::shared_ptr<JsonSaveFileUtils> JsonSaveFileUtils::instance;

JsonSaveFileUtils::JsonSaveFileUtils() = default;

std::shared_ptr<JsonSaveFileUtils> JsonSaveFileUtils::getInstance()
{
	if (!instance) {
		instance = std::make_shared<JsonSaveFileUtils>(JsonSaveFileUtils());
		instance->clear();
	}
	return instance;
}

void JsonSaveFileUtils::clear()
{
	_cacheRoot = new rapidjson::Document();
	_cacheRoot->SetObject();
	resetCurrentNode();
}

void JsonSaveFileUtils::save(std::string filename)
{
	FILE* fp = fopen(filename.c_str(), "wb");
	char buffer[65536];
	rapidjson::FileWriteStream os(fp, buffer, sizeof(buffer));

	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	_cacheRoot->Accept(writer);
}

void JsonSaveFileUtils::resetCurrentNode()
{
	_current = _cacheRoot;
}

bool JsonSaveFileUtils::isCurrentNodeObject() const
{
	return _current->IsObject();
}

bool JsonSaveFileUtils::isCurrentNodeArray() const
{
	return _current->IsArray();
}

void JsonSaveFileUtils::setBoolInObject(std::string key, bool value)
{
	rapidjson::Value v;
	v.SetBool(value);
	setValueInObject(key, &v);
}

void JsonSaveFileUtils::setIntegerInObject(std::string key, int value)
{
	setValueInObject(key, &rapidjson::Value(value));
}

void JsonSaveFileUtils::setDoubleInObject(std::string key, double value)
{
	setValueInObject(key, &rapidjson::Value(value));
}

void JsonSaveFileUtils::setStringInObject(std::string key, std::string value)
{
	setValueInObject(key, &rapidjson::Value(value.c_str(), value.size()));
}

void JsonSaveFileUtils::setAttributeInObject(std::string key, const GameAttribute& value)
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

bool JsonSaveFileUtils::getBoolInObject(std::string key)
{
	auto data = getValueInObject(key);
	return data->GetBool();
}

int JsonSaveFileUtils::getIntegerInObject(std::string key)
{
	auto data = getValueInObject(key);
	return data->GetInt();
}

double JsonSaveFileUtils::getDoubleInObject(std::string key)
{
	auto data = getValueInObject(key);
	return data->GetDouble();
}

std::string JsonSaveFileUtils::getStringInObject(std::string key)
{
	auto data = getValueInObject(key);
	return data->GetString();
}

std::unique_ptr<GameAttribute> JsonSaveFileUtils::getAttributeInObject(
	std::string key, GameAttribute::DataType dataType)
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

void JsonSaveFileUtils::setCurrentNodeInObject(std::string key)
{
	assert(isCurrentNodeObject());
	auto target = _current->FindMember(key.c_str());
	if (target == _current->MemberEnd()) {
		throw std::invalid_argument("cannot find '" + key + "' in current node's children");
	}
	_current = &(target->value);
}

void JsonSaveFileUtils::appendBoolInArray(bool value)
{
	rapidjson::Value v;
	v.SetBool(value);
	appendValueInArray(&v);
}

void JsonSaveFileUtils::appendIntegerInArray(int value)
{
	appendValueInArray(&rapidjson::Value(value));
}

void JsonSaveFileUtils::appendDoubleInArray(double value)
{
	appendValueInArray(&rapidjson::Value(value));
}

void JsonSaveFileUtils::appendStringInArray(std::string value)
{
	appendValueInArray(&rapidjson::Value(value.c_str(), value.size()));
}

void JsonSaveFileUtils::appendAttributeInArray(const GameAttribute& value)
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

void JsonSaveFileUtils::setBoolInArray(int index, bool value)
{
	rapidjson::Value v;
	v.SetBool(value);
	setValueInArray(index, &v);
}

void JsonSaveFileUtils::setIntegerInArray(int index, int value)
{
	setValueInArray(index, &rapidjson::Value(value));
}

void JsonSaveFileUtils::setDoubleInArray(int index, double value)
{
	setValueInArray(index, &rapidjson::Value(value));
}

void JsonSaveFileUtils::setStringInArray(int index, std::string value)
{
	setValueInArray(index, &rapidjson::Value(value.c_str(), value.size()));
}

void JsonSaveFileUtils::setAttributeInArray(int index, const GameAttribute& value)
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

bool JsonSaveFileUtils::getBoolInArray(int index)
{
	auto data = getValueInArray(index);
	return data->GetBool();
}

int JsonSaveFileUtils::getIntegerInArray(int index)
{
	auto data = getValueInArray(index);
	return data->GetInt();
}

double JsonSaveFileUtils::getDoubleInArray(int index)
{
	auto data = getValueInArray(index);
	return data->GetDouble();
}

std::string JsonSaveFileUtils::getStringInArray(int index)
{
	auto data = getValueInArray(index);
	return data->GetString();
}

std::unique_ptr<GameAttribute> JsonSaveFileUtils::getAttributeInArray(
	int index, GameAttribute::DataType dataType)
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

void JsonSaveFileUtils::setCurrentNodeInArray(int index)
{
	assert(isCurrentNodeArray());
	index = checkIndex(index);
	_current = &(*_current)[index];
}

void JsonSaveFileUtils::setValueInObject(std::string key, rapidjson::Value* value)
{
	assert(isCurrentNodeObject());
	rapidjson::Value name(key.c_str(), key.size());
	_current->AddMember(name, *value, _cacheRoot->GetAllocator());
}

void JsonSaveFileUtils::setValueInArray(int index, rapidjson::Value* value)
{
	assert(isCurrentNodeArray());
	index = checkIndex(index);
	auto now = _current->GetArray().Begin() + index;
	now->Set(value, _cacheRoot->GetAllocator());
}

void JsonSaveFileUtils::appendValueInArray(rapidjson::Value* value)
{
	assert(isCurrentNodeArray());
	_current->GetArray().PushBack(*value, _cacheRoot->GetAllocator());
}

rapidjson::Value* JsonSaveFileUtils::getValueInObject(std::string key)
{
	assert(isCurrentNodeObject());
	auto target = _current->FindMember(key.c_str());
	if (target == _current->MemberEnd()) {
		throw std::invalid_argument("cannot find '" + key + "' in current node's children");
	}
	return &(target->value);
}

rapidjson::Value* JsonSaveFileUtils::getValueInArray(int index)
{
	assert(isCurrentNodeArray());
	index = checkIndex(index);
	auto now = _current->GetArray().Begin() + index;
	return now;
}

int JsonSaveFileUtils::checkIndex(int index) const
{
	int size = _current->Size();
	if (index < 0) {
		index += size;
	}
	if (index <= 0 || index >= size) {
		throw std::out_of_range("the index of node is out of range");
	}
	return index;
}
