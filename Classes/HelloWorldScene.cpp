/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "HelloWorldScene.h"
#include "GameCharacter.h"
#include "GameDataUtils.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width - closeItem->getContentSize().width/2;
        float y = origin.y + closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    //auto wrapLabel = [this](std::string content, float x, float y) {
    //    auto label = Label::createWithTTF(content, "fonts/FZDBSJW.ttf", 12);
    //    label->setAnchorPoint(Vec2(0.0, 1.0));
    //    label->setPosition(x, y);
    //    this->addChild(label);
    //};

    //float y = 470.0;
    //auto utilp = GameDataUtils::getInstance();
    //wrapLabel("Test attribute names:", 10.0, y);
    //wrapLabel("Get 'atk' name: " + utilp->getAttrName("atk"), 10.0, y -= 20.0);
    //wrapLabel("Get 'ch0012:magicDmgValidity:ice' name: " + utilp->getComplexAttrName("ch0012:magicDmgValidity:ice"), 10.0, y -= 20.0);
    //wrapLabel("Get 'ch0012:magicDmgValidity:ice' short name: " + utilp->getMinAttrName("ch0012:magicDmgValidity:ice"), 10.0, y -= 20.0);
    //wrapLabel("Get 'atk' short name: " + utilp->getMinAttrName("atk"), 10.0, y -= 20.0);

    //GameAttribute ga("ch0021:def", GameAttribute::Integer, true);
    //wrapLabel("Test edit attribute name. Before edit: " + ga.getName(), 10.0, y -= 20.0);
    //utilp->editAttrName(ga);
    //wrapLabel("After edit (complex): " + ga.getName(), 10.0, y -= 20.0);
    //utilp->editAttrName(ga, false);
    //wrapLabel("After edit (short): " + ga.getName(), 10.0, y -= 20.0);

    //wrapLabel("Test level experience: ", 10.0, y -= 20.0);
    //wrapLabel("Get max level: " + std::to_string(utilp->getPossibleMaxLevel()), 10.0, y -= 20.0);
    //wrapLabel("Get level 77 - 78 experience: " + std::to_string(utilp->getExpToLevel(77)), 10.0, y -= 20.0);
    //wrapLabel("Get level 0 - 78 experience: " + std::to_string(utilp->getAllExpToLevel(77)), 10.0, y -= 20.0);

    //wrapLabel("Test growth: ", 10.0, y -= 20.0);
    //GameAttribute maxHp("ch0021:maxHp", GameAttribute::Integer);
    //maxHp.setInteger(1000);
    //wrapLabel("Max HP before growth: " + std::to_string(maxHp.getInteger()), 10.0, y -= 20.0);
    //Maps::StringMap<double> growth;
    //growth["pow"] = 100.0;
    //growth["str"] = 200.0;
    //utilp->grow(maxHp, growth);
    //wrapLabel("Max HP after growth: " + std::to_string(maxHp.getInteger()), 10.0, y -= 20.0);

    //wrapLabel("Test binding: ", 10.0, y -= 20.0);
    //GameAttribute tempHp("ch0021:hp", GameAttribute::Integer, true);
    //wrapLabel("ID of bound attribute: " + utilp->getBoundAttrId(tempHp.getId()), 10.0, y -= 20.0);
    //GameAttributeWithMaxium hp = utilp->generateAttrWithMax(tempHp);
    //hp.setInteger(800);
    //hp.setBindingMode(GameAttributeWithMaxium::Ratio);
    //utilp->grow(maxHp, growth);
    //wrapLabel("HP: " + std::to_string(hp.getInteger()), 10.0, y -= 20.0);

    auto fileutils = cocos2d::FileUtils::getInstance();
    std::string data = fileutils->getStringFromFile("json/charStaticData.json");

    rapidjson::Document document;
    document.Parse(data.c_str());
    assert(document.IsArray());
    auto arr = document.GetArray();
    assert(arr.Size() > 0);

    //GameCharacter character(arr[0]);
    //auto labels = character.getLabels();
    //for (auto label : labels) {
    //    this->addChild(label, 1);
    //}

    //auto label = Label::createWithTTF("测试 Test", "fonts/FZDBSJW.ttf", 24);
    //if (label == nullptr)
    //{
    //    problemLoading("'fonts/Marker Felt.ttf'");
    //}
    //else
    //{
    //    // position the label on the center of the screen
    //    label->setPosition(Vec2(origin.x + visibleSize.width/2,
    //                            origin.y + visibleSize.height - label->getContentSize().height));

    //    // add the label as a child to this layer
    //    this->addChild(label, 1);
    //}

    //// add "HelloWorld" splash screen"
    //auto sprite = Sprite::create("HelloWorld.png");
    //if (sprite == nullptr)
    //{
    //    problemLoading("'HelloWorld.png'");
    //}
    //else
    //{
    //    // position the sprite on the center of the screen
    //    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    //    // add the sprite as a child to this layer
    //    this->addChild(sprite, 0);
    //}
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}
