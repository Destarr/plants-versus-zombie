#include "MenuSystem.h"
#include "GameScene.h"

#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

USING_NS_CC;

bool MenuSystem::init() {
    if (!Node::init()) {
        return false;
    }

    _isMenuVisible = false;

    // 创建菜单按钮
    createMenuButton();

    // 创建暂停菜单
    createPauseMenu();

    return true;
}

void MenuSystem::createMenuButton() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建菜单按钮（右上角）
    auto menuButton = MenuItemImage::create(
        "Menu/menuButton.png",
        "Menu/menuButtonSelected.png",
        CC_CALLBACK_1(MenuSystem::onMenuButtonClicked, this));

    auto menu = Menu::create(menuButton, nullptr);
    menu->setPosition(Vec2(visibleSize.width - 40, visibleSize.height - 40));
    this->addChild(menu);
}

void MenuSystem::createPauseMenu() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建半透明遮罩
    _menuOverlay = LayerColor::create(Color4B(0, 0, 0, 180));
    _menuOverlay->setVisible(false);
    this->addChild(_menuOverlay, 100);

    // 为遮罩层添加触摸监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        // 只有菜单可见时才吞噬触摸事件
        return _isMenuVisible;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _menuOverlay);

    // 创建菜单背景
    auto menuBg = Sprite::create("Menu/pause_menu.png");

    menuBg->setPosition(visibleSize / 2);
    _menuOverlay->addChild(menuBg);

    // 继续按钮
    auto continueItem = MenuItemImage::create(
        "Menu/continueButton.png",
        "Menu/continueButtonSelected.png",
        CC_CALLBACK_1(MenuSystem::onContinueClicked, this));


    // 重新开始按钮
    auto restartItem = MenuItemImage::create(
        "Menu/restartButton.png",
        "Menu/restartButtonSelected.png",
        CC_CALLBACK_1(MenuSystem::onRestartClicked, this));

    // 创建菜单
    auto menu = Menu::create(continueItem, restartItem, nullptr);
    menu->setPosition(Vec2(menuBg->getContentSize().width / 2,
        menuBg->getContentSize().height / 2));
    menu->alignItemsVerticallyWithPadding(30);
    menuBg->addChild(menu);
}

void MenuSystem::onMenuButtonClicked(Ref* sender) {
    if (_isMenuVisible) return;
    AudioEngine::play2d("music/butter.ogg", false, 1.0f);
    _isMenuVisible = true;
    _menuOverlay->setVisible(true);

    // pause background music.
    auto audio = SimpleAudioEngine::getInstance();
    audio->pauseBackgroundMusic();

    // 暂停游戏
    Director::getInstance()->pause();
}

void MenuSystem::onContinueClicked(Ref* sender) {
    if (!_isMenuVisible) return;
    AudioEngine::play2d("music/butter.ogg", false, 1.0f);
    _isMenuVisible = false;
    _menuOverlay->setVisible(false);

    // resume background music.
    auto audio = SimpleAudioEngine::getInstance();
    audio->resumeBackgroundMusic();

    // 恢复游戏
    Director::getInstance()->resume();
}

void MenuSystem::onRestartClicked(Ref* sender) {
    // 使用replaceScene重新开始
    AudioEngine::play2d("music/butter.ogg", false, 1.0f);
    auto scene = Game::createScene();
    Director::getInstance()->replaceScene(scene);
    Director::getInstance()->resume();
}