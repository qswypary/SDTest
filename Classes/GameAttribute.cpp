#include "GameAttribute.h"
#include <cstring>

GameAttribute::GameAttribute(std::string id, DataType datatype, bool temp) :
	GameAttribute(id, id, datatype, temp) { }

GameAttribute::GameAttribute(std::string id, std::string name, DataType datatype, bool temp) :
	_dispatcher(cocos2d::Director::getInstance()->getEventDispatcher()),
	_id(id), _name(name), _datatype(datatype), _temp(temp)
{
	initialize();
}

GameAttribute& GameAttribute::operator=(const GameAttribute& that)
{
	_id = that._id; _name = that._name;
	_datatype = that._datatype; _temp = that._temp;
	_data = that._data;
	initialize();
	return *this;
}

GameAttribute::GameAttribute(const GameAttribute& that) :
	_dispatcher(cocos2d::Director::getInstance()->getEventDispatcher()),
	_id(that._id), _name(that._name), 
	_datatype(that._datatype), _temp(that._temp),
	_data(that._data)
{
	initialize();
}

GameAttribute::~GameAttribute()
{
	if (!_temp) {
		_dispatcher->removeEventListener(_listener);
	}
}

void GameAttribute::setName(std::string name)
{
	_name = name;
}

void GameAttribute::setDataType(DataType datatype)
{
	_datatype = datatype;
}

void GameAttribute::setInteger(int data)
{
	_data.intdata = data;
	if (!_temp) {
		sendChangeEvent();
	}
}

void GameAttribute::setDecimal2(double data)
{
	_data.intdata = data * _DECIMAL2RATE;
	if (!_temp) {
		sendChangeEvent();
	}
}

void GameAttribute::setDecimal4(double data)
{
	_data.intdata = data * _DECIMAL4RATE;
	if (!_temp) {
		sendChangeEvent();
	}
}

void GameAttribute::setDouble(double data)
{
	_data.doubledata = data;
	if (!_temp) {
		sendChangeEvent();
	}
}

void GameAttribute::setText(std::string data)
{
	strcpy_s(_data.textdata, data.size() + 1, data.c_str());
}

void GameAttribute::setText(const char data[], int len)
{
	strcpy_s(_data.textdata, len + 1, data);
}

std::string GameAttribute::getId() const
{
	return _id;
}

std::string GameAttribute::getName() const
{
	return _name;
}

GameAttribute::DataType GameAttribute::getDataType() const
{
	return _datatype;
}

bool GameAttribute::isTemp() const
{
	return _temp;
}

int GameAttribute::getInteger() const
{
	return _data.intdata;
}

double GameAttribute::getDecimal2() const
{
	return _data.intdata / double(_DECIMAL2RATE);
}

double GameAttribute::getDecimal4() const
{
	return _data.intdata / double(_DECIMAL4RATE);
}

double GameAttribute::getDouble() const
{
	return _data.doubledata;
}

std::string GameAttribute::getText() const
{
	return std::string(_data.textdata);
}

void GameAttribute::initialize()
{
	// 订阅对该对象发出的数据请求
	if (!_temp && getDataType() != Text) {
		std::function<void(cocos2d::EventCustom*)> func = [this](cocos2d::EventCustom* event) {
			sendChangeEvent();
		};
		_listener = _dispatcher->addCustomEventListener(_id + ":DataNeeded", func);
	}
}

void GameAttribute::sendChangeEvent() const
{
	// 发送包含改动后数据的消息
	cocos2d::EventCustom event = cocos2d::EventCustom(_id + ":DataChanged");
	event.setUserData((void *)(&_data));
	_dispatcher->dispatchEvent(&event);
}
