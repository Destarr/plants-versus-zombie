//该文件作为游戏主界面菜单的实现
#include "GameScene.h"
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

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto adventureModeItem = MenuItemImage::create(
                                           "adventureMode.png",
                                           "adventureModeSelected.png",
                                           CC_CALLBACK_1(HelloWorld::gameBeginCallback, this));

    if (adventureModeItem == nullptr ||
        adventureModeItem->getContentSize().width <= 0 ||
        adventureModeItem->getContentSize().height <= 0)
    {
        problemLoading("'adventureMode.png' and 'adventureModeSelected.png'");
    }
    else
    {
        float x = origin.x + visibleSize.width / 9 * 6;
        float y = origin.y + visibleSize.height / 6 * 4;
        adventureModeItem->setPosition(Vec2(x,y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(adventureModeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label

    auto label = Label::createWithSystemFont("Plants VS Zombies", "", 16);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("Surface.png");
    if (sprite == nullptr)
    {
        problemLoading("'Surface.png'");
    }
    else
    {
        // position the sprite on the center of the screen
        sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(sprite, 0);
    }
    return true;
}


void HelloWorld::gameBeginCallback(Ref* pSender)
{
    // create a scene. it's an autorelease object
    auto scene = Game::createScene();
    // run
    auto director = Director::getInstance();
    director->replaceScene(scene);

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}
