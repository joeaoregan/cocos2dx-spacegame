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
#include "SimpleAudioEngine.h"

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

	_batchNode = SpriteBatchNode::create("spritesheets/Sprites.pvr.ccz");
	this->addChild(_batchNode);
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("spritesheets/Sprites.plist");

	_ship = Sprite::createWithSpriteFrameName("SpaceFlier_sm_1.png");
	_ship->setPosition(visibleSize.width * 0.1, visibleSize.height * 0.5);
	_batchNode->addChild(_ship, 1);

	// 1) Create the ParallaxNode
	_backgroundNode = ParallaxNode::create();
	this->addChild(_backgroundNode, -1);

	// 2) Create the sprites that will be added to the ParallaxNode
	_spaceDust1 = Sprite::create("backgrounds/bg_front_spacedust.png");
	_spaceDust2 = Sprite::create("backgrounds/bg_front_spacedust.png");
	_planetSunrise = Sprite::create("backgrounds/bg_planetsunrise.png");
	_galaxy = Sprite::create("backgrounds/bg_galaxy.png");
	_spatialAnomaly1 = Sprite::create("backgrounds/bg_spacialanomaly.png");
	_spatialAnomaly2 = Sprite::create("backgrounds/bg_spacialanomaly2.png");

	// 3) Determine relative movement speeds for space dust and background
	auto dustSpeed = Point(0.1F, 0.1F);
	auto bgSpeed = Point(0.05F, 0.05F);

	// 4) Add children to ParallaxNode
	_backgroundNode->addChild(_spaceDust1, 0, dustSpeed, Point(0, visibleSize.height) / 2);
	_backgroundNode->addChild(_spaceDust2, 0, dustSpeed, Point(_spaceDust1->getContentSize().width, visibleSize.height / 2));
	_backgroundNode->addChild(_galaxy, -1, bgSpeed, Point(0, visibleSize.height * 0.7));
	_backgroundNode->addChild(_planetSunrise, -1, bgSpeed, Point(600, visibleSize.height * 0));
	_backgroundNode->addChild(_spatialAnomaly1, -1, bgSpeed, Point(900, visibleSize.height * 0.3));
	_backgroundNode->addChild(_spatialAnomaly2, -1, bgSpeed, Point(1500, visibleSize.height * 0.9));

	this->scheduleUpdate();

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

void HelloWorld::update(float dt) {
	auto backgroundScrollVert = Point(-1000, 0);
	_backgroundNode->setPosition(_backgroundNode->getPosition() + (backgroundScrollVert * dt));
}
