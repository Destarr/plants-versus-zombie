//该文件为游戏场景的实现 ——2451401 吕光晔
#include "GameScene.h"
#include "SimpleAudioEngine.h"
//#include "CardShop.h";

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Game::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.


    //// create menu, it's an autorelease object
    //auto menu = Menu::create(adventureModeItem, NULL);
    //menu->setPosition(Vec2::ZERO);
    //this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    

    // add "HelloWorld" splash screen"
    auto spriteMap = Sprite::create("map.jpg");
    if (spriteMap == nullptr)
    {
        problemLoading("'map.jpg'");
    }
    else
    {

        // position the sprite on the center of the screen
        spriteMap->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(spriteMap, 0);
    }

    auto spriteShop = Sprite::create("Shop.png");
    if (spriteShop == nullptr)
    {
        problemLoading("'Shop.png'");
    }
    else
    {
        spriteShop->setAnchorPoint(Vec2(0, 1));
        // position the sprite on the center of the screen
        spriteShop->setPosition(Vec2(0, visibleSize.height + origin.y));

        // add the sprite as a child to this layer
        spriteMap->addChild(spriteShop, 0);
    }

    //auto adventureModeItem = MenuItemImage::create(
    //    "adventureMode.png",
    //    "adventureModeSelected.png",
    //    CC_CALLBACK_1(Game::plantSelectedCallback, this));

    //if (adventureModeItem == nullptr ||
    //    adventureModeItem->getContentSize().width <= 0 ||
    //    adventureModeItem->getContentSize().height <= 0)
    //{
    //    problemLoading("'adventureMode.png' and 'adventureModeSelected.png'");
    //}
    //else
    //{
    //    float x = origin.x + visibleSize.width / 9 * 6;
    //    float y = origin.y + visibleSize.height / 6 * 4;
    //    adventureModeItem->setPosition(Vec2(x, y));
    //}

    //// create menu, it's an autorelease object
    //auto menu = Menu::create(adventureModeItem, NULL);
    //menu->setPosition(Vec2::ZERO);
    //this->addChild(menu, 1);
    //auto cardshop = CardShop::create();
    //this->addChild(cardshop, 0);

    return true;
}


void Game::plantSelectedCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
