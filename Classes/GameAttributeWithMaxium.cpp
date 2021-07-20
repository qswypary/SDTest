#include "GameAttributeWithMaxium.h"

GameAttributeWithMaxium::GameAttributeWithMaxium(std::string id, DataType datatype, std::string boundAttrId):
	GameAttribute(id, id, datatype), _boundAttrId(boundAttrId)
{
	initialize();
}

GameAttributeWithMaxium::GameAttributeWithMaxium(std::string id, std::string name, DataType datatype, std::string boundAttrId):
	GameAttribute(id, name, datatype), _boundAttrId(boundAttrId)
{
	initialize();
}

GameAttributeWithMaxium& GameAttributeWithMaxium::operator=(const GameAttributeWithMaxium& that)
{
	GameAttribute::operator=(that);
	_boundAttrId = that._boundAttrId;
	BindingMode _bindingmode = that._bindingmode;
	_initializing = true;
	initialize();
	return *this;
}

GameAttributeWithMaxium::GameAttributeWithMaxium(const GameAttributeWithMaxium& that) : 
	GameAttribute(that)
{
	_boundAttrId = that._boundAttrId;
	BindingMode _bindingmode = that._bindingmode;
	initialize();
}

GameAttributeWithMaxium::~GameAttributeWithMaxium()
{
	_dispatcher->removeEventListener(_changelistener);
}

std::string GameAttributeWithMaxium::getBoundAttributeId() const
{
	return _boundAttrId;
}

void GameAttributeWithMaxium::setDataType(DataType datatype)
{
	if (datatype == Text) {
		throw std::invalid_argument("an attribute with maxium cannot be a text");
	}
}

void GameAttributeWithMaxium::setInteger(int data)
{
	int max = _boundAttrData.intdata;
	if (data > max) {
		data = max;
	}
	GameAttribute::setInteger(data);
}

void GameAttributeWithMaxium::setDecimal2(double data)
{
	double max = _boundAttrData.intdata;
	if (data > max) {
		data = max;
	}
	GameAttribute::setInteger(data);
}

void GameAttributeWithMaxium::setDecimal4(double data)
{
	double max = _boundAttrData.intdata;
	if (data > max) {
		data = max;
	}
	GameAttribute::setInteger(data);
}

void GameAttributeWithMaxium::setDouble(double data)
{
	double max = _boundAttrData.doubledata;
	if (data > max) {
		data = max;
	}
	GameAttribute::setInteger(data);
}

void GameAttributeWithMaxium::setBindingMode(BindingMode bm)
{
	_bindingmode = bm;
}

GameAttributeWithMaxium::BindingMode GameAttributeWithMaxium::getBindingMode() const
{
	return _bindingmode;
}

void GameAttributeWithMaxium::initialize()
{
	if (getDataType() == Text) {
		throw std::invalid_argument("an attribute with maxium cannot be a text");
	}

	// 订阅最大值属性
	std::function<void(cocos2d::EventCustom*)> func = [this](cocos2d::EventCustom* event) {
		onBoundAttributeChange((Data *)event->getUserData());
	};
	_changelistener = _dispatcher->addCustomEventListener(_boundAttrId + ":DataChanged", func);
	// 发出消息请求获取最大值属性的值
	cocos2d::EventCustom event = cocos2d::EventCustom(_boundAttrId + ":DataNeeded");
	_dispatcher->dispatchEvent(&event);
}

void GameAttributeWithMaxium::onBoundAttributeChange(Data * data)
{
	Data olddata = _boundAttrData;
	_boundAttrData = *data;
	if (_initializing) {
		_initializing = false;
		return;
	}
	auto changethis = [](auto thisdata, auto thatdataprev, auto thatdata, BindingMode bm) -> double {
		auto vacancy = thatdataprev - thisdata;
		switch (bm)
		{
		case Fixed:
			return (thisdata <= thatdata) ? thisdata : thatdata;
			break;
		case VacancyFixed:
			return (vacancy <= thatdata) ? thatdata - vacancy : 0;
			break;
		case Ratio:
			return thatdata * (double(thisdata) / double(thatdataprev));
			break;
		default:
			break;
		}
	};
	if (getDataType() == Double) {
		double max = data->doubledata;
		setDouble(changethis(getDouble(), olddata.doubledata, max, _bindingmode));
	}
	else {
		int max = data->intdata;
		setInteger((double)changethis(getInteger(), olddata.intdata, max, _bindingmode));
	}
}

void GameAttributeWithMaxium::setText(std::string data)
{
	throw std::runtime_error("an attribute with maxium has no operation about text content");
}

void GameAttributeWithMaxium::setText(const char data[], int len)
{
	throw std::runtime_error("an attribute with maxium has no operation about text content");
}

std::string GameAttributeWithMaxium::getText() const
{
	throw std::runtime_error("an attribute with maxium has no operation about text content");
}