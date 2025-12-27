#include "GameDataCenter.h"
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

    GameDataCenter::getInstance()->reset();

    GameDataCenter* createdInstance;


    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 初始化阳光（初始50阳光）
    m_currentSun = 50;
    m_sunLabel = nullptr;
    m_sunIcon = nullptr;

    // 1. 创建游戏层（所有游戏元素放在这层）
    auto gameLayer = Layer::create();
    this->addChild(gameLayer, 1);

    // 2. 添加游戏系统（包含进度条和僵尸生成）
    auto gameSystem = GameSystem::create(gameLayer);
    gameLayer ->addChild(gameSystem, 10);

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

    // 8. 创建阳光显示UI
    createSunDisplay();

    // 9. 设置阳光变化回调
    m_sunChangedCallback = [this](int sunAmount) {
        this->updateSunDisplay();
        };

    // 7. 延迟2秒后开始游戏
    this->runAction(Sequence::create(
        DelayTime::create(2.0f),
        CallFunc::create([gameSystem]() {
            gameSystem->startGame();
            }),
        nullptr
    ));

    this->scheduleUpdate();
    return true;
}

void Game::createSunDisplay() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建阳光图标
    m_sunIcon = Sprite::create("SunBeam/SunBeam.png");
    if (!m_sunIcon) {
        // 如果找不到图片，创建一个黄色圆形占位符
        m_sunIcon = Sprite::create();
        auto drawNode = DrawNode::create();
        drawNode->drawSolidCircle(Vec2::ZERO, 25, 0, 16, Color4F::YELLOW);
        m_sunIcon->addChild(drawNode);
    }

    m_sunIcon->setPosition(Vec2(80, visibleSize.height - 50));
    m_sunIcon->setScale(0.6f);
    this->addChild(m_sunIcon, 200); // 高层级确保显示在最上面

    // 创建阳光数量标签
    m_sunLabel = Label::createWithTTF("50", "fonts/Marker Felt.ttf", 28);
    m_sunLabel->setPosition(Vec2(130, visibleSize.height - 50));
    m_sunLabel->setTextColor(Color4B::YELLOW);
    m_sunLabel->enableOutline(Color4B::BLACK, 2); // 黑色描边，更好看
    this->addChild(m_sunLabel, 200);

    CCLOG("Sun display created at position: (%.1f, %.1f)",
        130.0f, visibleSize.height - 50.0f);
}

// 更新阳光显示
void Game::updateSunDisplay() {
    if (m_sunLabel) {
        m_sunLabel->setString(std::to_string(m_currentSun));

        // 添加简单的动画效果
        auto scaleUp = ScaleTo::create(0.1f, 1.2f);
        auto scaleDown = ScaleTo::create(0.1f, 1.0f);
        m_sunLabel->runAction(Sequence::create(scaleUp, scaleDown, nullptr));
    }
}

// 增加阳光
void Game::addSun(int amount) {
    if (amount <= 0) return;

    m_currentSun += amount;

    // 上限检查（PVZ通常上限是9999）
    if (m_currentSun > 9999) {
        m_currentSun = 9999;
    }

    // 更新显示
    updateSunDisplay();

    // 触发回调
    if (m_sunChangedCallback) {
        m_sunChangedCallback(m_currentSun);
    }

    CCLOG("Sun added: %d, Total: %d", amount, m_currentSun);
}

// 消耗阳光（种植植物时调用）
bool Game::consumeSun(int amount) {
    if (amount <= 0) return true;

    if (m_currentSun >= amount) {
        m_currentSun -= amount;

        // 更新显示
        updateSunDisplay();

        // 触发回调
        if (m_sunChangedCallback) {
            m_sunChangedCallback(m_currentSun);
        }

        CCLOG("Sun consumed: %d, Remaining: %d", amount, m_currentSun);
        return true;
    }

    // 阳光不足，添加提示效果
    CCLOG("Not enough sun! Need: %d, Have: %d", amount, m_currentSun);

    if (m_sunLabel) {
        // 红色闪烁提示
        auto tintRed = TintTo::create(0.1f, Color3B::RED);
        auto tintYellow = TintTo::create(0.1f, Color3B::YELLOW);
        auto shake1 = MoveBy::create(0.05f, Vec2(5, 0));
        auto shake2 = MoveBy::create(0.05f, Vec2(-10, 0));
        auto shake3 = MoveBy::create(0.05f, Vec2(5, 0));

        m_sunLabel->runAction(Sequence::create(
            tintRed, tintYellow, nullptr
        ));

        m_sunIcon->runAction(Sequence::create(
            shake1, shake2, shake3, nullptr
        ));
    }

    return false;
}

// 获取当前阳光数
int Game::getCurrentSun() const {
    return m_currentSun;
}

void Game::update(float delta) {
    // 获取共享数据
    auto& bullets = GameDataCenter::getInstance()->getBullets();
    auto& zombies = GameDataCenter::getInstance()->getZombies();
    auto& plants = GameDataCenter::getInstance()->getPlants();

    // 使用队友的碰撞检测接口
    CollisionDetector::detectBulletZombieCollisions(bullets, zombies);
    CollisionDetector::detectZombiePlantCollisions(zombies, plants);

    // 清理死亡对象
    GameDataCenter::getInstance()->cleanupDeadObjects();
}