#pragma once
#include <string>
#include "cocos2d.h"

class GameAttribute
{
public:
	enum DataType {
		Integer,
		Decimal2,
		Decimal4,
		Double,
		Text
	};

protected:
	union Data
	{
		int intdata;
		double doubledata;
		char textdata[1024];
	};

private:
	const int _DECIMAL2RATE = 100;
	const int _DECIMAL4RATE = 10000;

	cocos2d::EventListenerCustom* _listener;
	std::string _id;
	std::string _name;
	DataType _datatype = Integer;
	Data _data;
	bool _temp = false;

protected:
	cocos2d::EventDispatcher* _dispatcher;

public:
	GameAttribute(std::string id, DataType datatype, bool temp = false);
	GameAttribute(std::string id, std::string name, DataType datatype, bool temp = false);

	GameAttribute& operator=(const GameAttribute& that);
	GameAttribute(const GameAttribute& that);
	~GameAttribute();

	void setName(std::string name);
	void setDataType(DataType datatype);

	void setInteger(int data);
	void setDecimal2(double data);
	void setDecimal4(double data);
	void setDouble(double data);
	void setText(std::string data);
	void setText(const char data[], int len);

	std::string getId() const;
	std::string getName() const;
	DataType getDataType() const;
	bool isTemp() const;

	int getInteger() const;
	double getDecimal2() const;
	double getDecimal4() const;
	double getDouble() const;
	std::string getText() const;

private:
	void initialize();

	void sendChangeEvent() const;
};