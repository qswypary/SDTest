#pragma once
#include "GameAttribute.h"

class GameAttributeWithMaxium :
	public GameAttribute
{
public:
	enum BindingMode {
		Fixed,
		VacancyFixed,
		Ratio
	};

private:
	cocos2d::EventListenerCustom* _changelistener;
	std::string _boundAttrId;
	Data _boundAttrData;
	BindingMode _bindingmode = Fixed;
	bool _initializing = true;

public:
	GameAttributeWithMaxium(std::string id, DataType datatype, std::string boundAttrId);
	GameAttributeWithMaxium(std::string id, std::string name, DataType datatype, std::string boundAttrId);

	GameAttributeWithMaxium& operator=(const GameAttributeWithMaxium& that);
	GameAttributeWithMaxium(const GameAttributeWithMaxium& that);
	~GameAttributeWithMaxium();

	std::string getBoundAttributeId() const;

	void setDataType(DataType datatype);

	void setInteger(int data);
	void setDecimal2(double data);
	void setDecimal4(double data);
	void setDouble(double data);

	void setBindingMode(BindingMode bm);

	BindingMode getBindingMode() const;

private:
	void initialize();

	void onBoundAttributeChange(Data *);

	void setText(std::string data);
	void setText(const char data[], int len);

	std::string getText() const;
};

