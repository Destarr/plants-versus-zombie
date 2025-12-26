#include "GameScene.h"
#include "GameSystem.h"
#include "MenuSystem.h"
#include "PlantPlacementCore.h"

USING_NS_CC;

Scene* Game::createScene() {
    return Game::create();
}

bool Game::init() {
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 创建游戏层（所有游戏元素放在这层）
    auto gameLayer = Layer::create();
    this->addChild(gameLayer, 1);

    // 2. 添加游戏系统（包含进度条和僵尸生成）
    auto gameSystem = GameSystem::create(gameLayer);
    this->addChild(gameSystem, 10);

    // 3. 添加菜单系统
    auto menuSystem = MenuSystem::create();
    this->addChild(menuSystem, 100);

    // 4. 添加地图背景
    auto spriteMap = Sprite::create("map.jpg");
    if (spriteMap) {
        spriteMap->setPosition(Vec2(visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y));
        this->addChild(spriteMap, 0);
    }

    // 5. 添加植物放置系统
    auto placementCore = PlantPlacementCore::create();
    gameLayer->addChild(placementCore, 10);

    // 6. 添加商店UI
    auto spriteShop = Sprite::create("Shop.png");
    if (spriteShop) {
        spriteShop->setAnchorPoint(Vec2(0, 1));
        spriteShop->setPosition(Vec2(0, visibleSize.height + origin.y));
        spriteMap->addChild(spriteShop, 0);
    }

    // 7. 延迟2秒后开始游戏
    this->runAction(Sequence::create(
        DelayTime::create(2.0f),
        CallFunc::create([gameSystem]() {
            gameSystem->startGame();
            }),
        nullptr
    ));

    return true;
}