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
using namespace CocosDenshion;

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#define SPACE_GAME "SpaceGame.caf"
#define EXPLOSION_LARGE "explosion_large.caf"
#define LASER_SHIP "laser_ship.caf"
#else
#define SPACE_GAME "SpaceGame.wav"
#define EXPLOSION_LARGE "explosion_large.wav"
#define LASER_SHIP "laser_ship.wav"
#endif

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

	_batchNode = SpriteBatchNode::create("Sprites.pvr.ccz");
	this->addChild(_batchNode);
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Sprites.plist");

	_ship = Sprite::createWithSpriteFrameName("SpaceFlier_sm_1.png");
	_ship->setPosition(visibleSize.width * 0.1, visibleSize.height * 0.5);
	_batchNode->addChild(_ship, 1);

	// 1) Create the ParallaxNode
	_backgroundNode = ParallaxNodeExtras::create();
	this->addChild(_backgroundNode, -1);

	// 2) Create the sprites that will be added to the ParallaxNode
	_spaceDust1 = Sprite::create("bg_front_spacedust.png");
	_spaceDust2 = Sprite::create("bg_front_spacedust.png");
	_planetSunrise = Sprite::create("bg_planetsunrise.png");
	_galaxy = Sprite::create("bg_galaxy.png");
	_spatialAnomaly1 = Sprite::create("bg_spacialanomaly.png");
	_spatialAnomaly2 = Sprite::create("bg_spacialanomaly2.png");

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

	HelloWorld::addChild(ParticleSystemQuad::create("Stars1.plist"));
	HelloWorld::addChild(ParticleSystemQuad::create("Stars2.plist"));
	HelloWorld::addChild(ParticleSystemQuad::create("Stars3.plist"));

	// Accelerometer
	Device::setAccelerometerEnabled(true);
	auto accelerationListener = EventListenerAcceleration::create(CC_CALLBACK_2(HelloWorld::onAcceleration, this));
	_eventDispatcher->addEventListenerWithSceneGraphPriority(accelerationListener, this);

	// Touches
	auto touchListener = EventListenerTouchAllAtOnce::create();
	touchListener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

#define KNUMASTEROIDS 15
	_asteroids = new Vector<Sprite*>(KNUMASTEROIDS);
	for (int i = 0; i < KNUMASTEROIDS; ++i) {
		auto *asteroid = Sprite::createWithSpriteFrameName("asteroid.png");
		asteroid->setVisible(false);
		_batchNode->addChild(asteroid);
		_asteroids->pushBack(asteroid);
	}

#define KNUMLASERS 5
	_shipLasers = new Vector<Sprite*>(KNUMLASERS);
	for (int i = 0; i < KNUMLASERS; ++i) {
		auto shipLaser = Sprite::createWithSpriteFrameName("laserbeam_blue.png");
		shipLaser->setVisible(false);
		_batchNode->addChild(shipLaser);
		_shipLasers->pushBack(shipLaser);
	}

	_lives = 3;
	double curTime = getTimeTick();
	_gameOverTime = curTime + 30000;

	this->scheduleUpdate();

	SimpleAudioEngine::getInstance()->playBackgroundMusic(SPACE_GAME, true);
	SimpleAudioEngine::getInstance()->preloadEffect(EXPLOSION_LARGE);
	SimpleAudioEngine::getInstance()->preloadEffect(LASER_SHIP);

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

	// Parallax
	auto spaceDusts = new Vector<Sprite*>(2);
	spaceDusts->pushBack(_spaceDust1);
	spaceDusts->pushBack(_spaceDust2);
	for (Sprite* spaceDust : *spaceDusts) {
		float xPosition = _backgroundNode->convertToWorldSpace(spaceDust->getPosition()).x;
		float size = spaceDust->getContentSize().width;
		if (xPosition < -size / 2) {
			_backgroundNode->incrementOffset(Point(spaceDust->getContentSize().width * 2, 0), spaceDust);
		}
	}

	auto backGrounds = new Vector<Sprite*>(4);
	backGrounds->pushBack(_galaxy);
	backGrounds->pushBack(_planetSunrise);
	backGrounds->pushBack(_spatialAnomaly1);
	backGrounds->pushBack(_spatialAnomaly2);
	for (auto background : *backGrounds) {
		float xPosition = _backgroundNode->convertToWorldSpace(background->getPosition()).x;
		float size = background->getContentSize().width;
		if (xPosition < -size) {
			_backgroundNode->incrementOffset(Point(2000, 0), background);
		}
	}

	// Acceleration
	Size winSize = Director::getInstance()->getWinSize();
	float maxY = winSize.height - _ship->getContentSize().height / 2;
	float minY = _ship->getContentSize().height / 2;
	float diff = (_shipPointsPerSecY * dt);
	float newY = _ship->getPosition().y + diff;
	newY = MIN(MAX(newY, minY), maxY);
	_ship->setPosition(_ship->getPosition().x, newY);

	// Asteroids
	float curTimeMillis = getTimeTick();
	if (curTimeMillis > _nextAsteroidSpawn) {

		float randMillisecs = randomValueBetween(0.20F, 1.0F) * 1000;
		_nextAsteroidSpawn = randMillisecs + curTimeMillis;

		float randY = randomValueBetween(0.0F, winSize.height);
		float randDuration = randomValueBetween(2.0F, 10.0F);

		Sprite *asteroid = _asteroids->at(_nextAsteroid);
		_nextAsteroid++;

		if (_nextAsteroid >= _asteroids->size())
			_nextAsteroid = 0;

		asteroid->stopAllActions();
		asteroid->setPosition(winSize.width + asteroid->getContentSize().width / 2, randY);
		asteroid->setVisible(true);
		asteroid->runAction(
			Sequence::create(
				MoveBy::create(randDuration, Point(-winSize.width - asteroid->getContentSize().width, 0)),
				CallFuncN::create(CC_CALLBACK_1(HelloWorld::setInvisible, this)),
				NULL /* DO NOT FORGET TO TERMINATE WITH NULL (unexpected in C++)*/)
		);
	}

	// Collisions
	for (auto asteroid : *_asteroids) {
		if (!(asteroid->isVisible()))
			continue;
		for (auto shipLaser : *_shipLasers) {
			if (!(shipLaser->isVisible()))
				continue;
			if (shipLaser->getBoundingBox().intersectsRect(asteroid->getBoundingBox())) {
				SimpleAudioEngine::getInstance()->playEffect(EXPLOSION_LARGE);
				shipLaser->setVisible(false);
				asteroid->setVisible(false);
			}
		}
		if (_ship->getBoundingBox().intersectsRect(asteroid->getBoundingBox())) {
			asteroid->setVisible(false);
			_ship->runAction(Blink::create(1.0F, 9));
			_lives--;
		}
	}

	// Win / Lose conditions
	if (_lives <= 0) {
		_ship->stopAllActions();
		_ship->setVisible(false);
		this->endScene(KENDREASONLOSE);
	}
	else if (curTimeMillis >= _gameOverTime) {
		this->endScene(KENDREASONWIN);
	}
}

void HelloWorld::onAcceleration(Acceleration* acc, Event* event) {
#define KFILTERINGFACTOR 0.1
#define KRESTACCELX -0.6
#define KSHIPMAXPOINTSPERSEC (winSize.height*0.5)
#define KMAXDIFFX 0.2

	double rollingX;

	// Cocos2DX inverts X and Y accelerometer depending on device orientation
	// in landscape mode right x=-y and y=x !!! (Strange and confusing choice)
	acc->x = acc->y;
	rollingX = (acc->x*KFILTERINGFACTOR) + (rollingX * (1.0 - KFILTERINGFACTOR));
	float accelX = acc->x - rollingX;
	Size winSize = Director::getInstance()->getWinSize();
	float accelDiff = accelX - KRESTACCELX;
	float accelFraction = accelDiff / KMAXDIFFX;
	_shipPointsPerSecY = KSHIPMAXPOINTSPERSEC * accelFraction;
}

float HelloWorld::randomValueBetween(float low, float high)
{
	return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

float HelloWorld::getTimeTick() {
	timeval time;
	gettimeofday(&time, NULL);
	unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (float)millisecs;
}

void HelloWorld::onTouchesBegan(const std::vector<Touch*>& touches, Event * event)
{
	SimpleAudioEngine::getInstance()->playEffect(LASER_SHIP);
	auto winSize = Director::getInstance()->getWinSize();
	auto shipLaser = _shipLasers->at(_nextShipLaser++);
	if (_nextShipLaser >= _shipLasers->size())
		_nextShipLaser = 0;
	shipLaser->setPosition(_ship->getPosition() + Point(shipLaser->getContentSize().width / 2, 0));
	shipLaser->setVisible(true);
	shipLaser->stopAllActions();
	shipLaser->runAction(
		Sequence::create(
			MoveBy::create(0.5, Point(winSize.width, 0)),
			CallFuncN::create(CC_CALLBACK_1(HelloWorld::setInvisible, this)),
			NULL));
}

void HelloWorld::setInvisible(Node * node) {
	node->setVisible(false);
}

void HelloWorld::restartTapped(Ref* pSender) {
	Director::getInstance()->replaceScene
	(TransitionZoomFlipX::create(0.5, this->createScene()));
	// reschedule
	this->scheduleUpdate();
}

void HelloWorld::endScene(EndReason endReason) {
	if (_gameOver)
		return;
	_gameOver = true;

	auto winSize = Director::getInstance()->getWinSize();
	char message[10] = "You Win";
	if (endReason == KENDREASONLOSE)
		strcpy(message, "You Lose");
	auto label = Label::createWithBMFont("Arial.fnt", message);
	label->setScale(0.1F);
	label->setPosition(winSize.width / 2, winSize.height*0.6F);
	this->addChild(label);

	strcpy(message, "Restart");
	auto restartLabel = Label::createWithBMFont("Arial.fnt", message);
	auto restartItem = MenuItemLabel::create(restartLabel, CC_CALLBACK_1(HelloWorld::restartTapped, this));
	restartItem->setScale(0.1F);
	restartItem->setPosition(winSize.width / 2, winSize.height*0.4);

	auto *menu = Menu::create(restartItem, NULL);
	menu->setPosition(Point::ZERO);
	this->addChild(menu);

	// clear label and menu
	restartItem->runAction(ScaleTo::create(0.5F, 1.0F));
	label->runAction(ScaleTo::create(0.5F, 1.0F));

	// Terminate update callback
	this->unscheduleUpdate();
}
