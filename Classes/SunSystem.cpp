#include "SunSystem.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

bool SunSystem::init() {
    if (!Layer::init()) {
        return false;
    }

    // 初始化阳光数量
    _sunCount = 2000;
    _sunGenerateInterval = 5.0f;
    _timeSinceLastSun = 0.0f;
    _isCollectingSun = false;

    // 清空阳光列表
    _suns.clear();

    // 初始化UI
    initUI();

    // 设置触摸事件
    setupTouchHandlers();

    // 调度阳光生成
    scheduleUpdate();

    CCLOG("SunSystem initialized with %d suns", _sunCount);

    return true;
}

void SunSystem::onEnter() {
    Layer::onEnter();
    CCLOG("SunSystem::onEnter");
}

void SunSystem::onExit() {
    // 移除触摸监听器
    if (_touchListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
        _touchListener = nullptr;
    }

    // 停止所有调度
    unscheduleUpdate();

    // 清理所有阳光
    for (auto sun : _suns) {
        if (sun) {
            sun->stopAllActions();
            sun->removeFromParent();
        }
    }
    _suns.clear();

    Layer::onExit();
    CCLOG("SunSystem::onExit");
}

void SunSystem::setupTouchHandlers() {
    // 确保之前没有监听器
    if (_touchListener) {
        Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
    }

    _touchListener = EventListenerTouchOneByOne::create();
    _touchListener->setSwallowTouches(false);  // 允许触摸穿透

    _touchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        Vec2 touchPos = touch->getLocation();

        // 防止同时收集多个阳光
        if (_isCollectingSun) {
            return false;
        }

        // 遍历阳光列表，但创建副本以防修改原始列表
        auto sunsCopy = _suns;

        for (auto sun : sunsCopy) {
            // 检查阳光是否有效且包含触摸点
            if (sun && sun->getParent() && sun->getBoundingBox().containsPoint(touchPos)) {
                CCLOG("Touching sun at position: (%f, %f)", touchPos.x, touchPos.y);

                // 标记正在收集阳光
                _isCollectingSun = true;

                // 收集阳光
                onSunCollected(sun);

                // 立即返回，一次只处理一个阳光
                return true;
            }
        }

        return false;
        };

    // 确保事件分发器只有一个监听器
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

void SunSystem::initUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 阳光数量标签
    _sunLabel = Label::createWithTTF(std::to_string(_sunCount), "fonts/Marker Felt.ttf", 24);
    _sunLabel->setColor(Color3B::BLACK);
    _sunLabel->setAnchorPoint(Vec2(0, 0.5));
    _sunLabel->setPosition(Vec2(origin.x + 70, origin.y + visibleSize.height - 70));
    this->addChild(_sunLabel, 1);
}

void SunSystem::addSun(int amount) {
    _sunCount += amount;
    updateSunLabel();

    CCLOG("Sun added: %d, total: %d", amount, _sunCount);
}

bool SunSystem::consumeSun(int amount) {
    if (_sunCount >= amount) {
        _sunCount -= amount;
        updateSunLabel();
        CCLOG("Sun consumed: %d, remaining: %d", amount, _sunCount);
        return true;
    }
    CCLOG("Not enough suns to consume! Needed: %d, Have: %d", amount, _sunCount);
    return false;
}

void SunSystem::updateSunLabel() {
    if (_sunLabel) {
        _sunLabel->setString(std::to_string(_sunCount));

        // 如果阳光不足，改变颜色
        if (_sunCount < 50) {
            _sunLabel->setColor(Color3B::RED);
        }
        else {
            _sunLabel->setColor(Color3B::BLACK);
        }
    }
}

void SunSystem::update(float dt) {
    _timeSinceLastSun += dt;

    if (_timeSinceLastSun >= _sunGenerateInterval) {
        generateFallingSun();
        _timeSinceLastSun = 0.0f;

        // 随机生成间隔（5-10秒）
        _sunGenerateInterval = random(5.0f, 10.0f);
    }
}

void SunSystem::generateFallingSun() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 随机生成位置（在草地上方）
    float randomX = random(origin.x + 100, origin.x + visibleSize.width - 100);
    float startY = origin.y + visibleSize.height;

    auto sun = Sprite::create("sun.png");
    if (!sun) {
        CCLOG("ERROR: Failed to load sun image!");
        // 创建一个替代的黄色方块用于调试
        sun = Sprite::create();
        sun->setTextureRect(Rect(0, 0, 50, 50));
        sun->setColor(Color3B::YELLOW);
    }

    sun->setPosition(Vec2(randomX, startY));
    sun->setScale(0.8f);
    this->addChild(sun, 5);
    _suns.push_back(sun);

    CCLOG("Generated falling sun at position: (%f, %f)", randomX, startY);

    // 执行下落动画
    sunFallingAnimation(sun);
}

void SunSystem::generateFallingSunAtPosition(const Vec2& position, int sunValue) {
    auto sun = Sprite::create("sun.png");
    if (!sun) {
        CCLOG("ERROR: Failed to load sun image!");
        // 创建一个替代的黄色方块
        sun = Sprite::create();
        sun->setTextureRect(Rect(0, 0, 50, 50));
        sun->setColor(Color3B::YELLOW);
    }

    sun->setPosition(position);
    sun->setScale(0.8f);

    // 使用用户数据存储阳光值
    sun->setUserData(new int(sunValue));

    this->addChild(sun, 5);
    _suns.push_back(sun);

    CCLOG("Generated static sun at position: (%f, %f) with value: %d",
        position.x, position.y, sunValue);

    // 在原地停留，不执行下落动画
    sun->setScale(0);
    sun->runAction(Sequence::create(
        ScaleTo::create(0.3f, 0.8f),
        DelayTime::create(5.0f),  // 停留5秒
        FadeOut::create(0.5f),
        CallFunc::create([sun, this]() {
            safeRemoveSun(sun);
            }),
        nullptr
    ));
}

void SunSystem::sunFallingAnimation(Sprite* sun) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 随机下落终点Y坐标
    float endY = random(origin.y + 100, origin.y + visibleSize.height - 200);

    // 抛物线运动
    auto jump = JumpTo::create(1.5f, Vec2(sun->getPositionX(), endY), 150, 1);

    // 下落完成后的停留动画
    sun->runAction(Sequence::create(
        jump,
        DelayTime::create(5.0f),  // 停留5秒
        FadeOut::create(0.5f),
        CallFunc::create([sun, this]() {
            safeRemoveSun(sun);
            }),
        nullptr
    ));
}

void SunSystem::onSunCollected(Sprite* sun) {
    if (!sun || !sun->getParent()) {
        CCLOG("ERROR: sun is null or has no parent in onSunCollected");
        _isCollectingSun = false;
        return;
    }

    CCLOG("Sun collected!");

    // 获取阳光值
    int sunValue = DEFAULT_SUN_VALUE;
    if (sun->getUserData()) {
        sunValue = *((int*)sun->getUserData());
    }

    // 停止阳光所有动画
    sun->stopAllActions();

    // 从列表中移除阳光
    auto it = std::find(_suns.begin(), _suns.end(), sun);
    if (it != _suns.end()) {
        _suns.erase(it);
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 目标位置：阳光显示位置
    Vec2 targetPos = Vec2(origin.x + 45, origin.y + visibleSize.height - 45);

    // 抛物线运动到阳光计数器
    auto moveTo = MoveTo::create(0.5f, targetPos);
    auto easeOut = EaseBackIn::create(moveTo);

    // 缩小的动画
    auto scaleDown = ScaleTo::create(0.5f, 0.2f);

    // 淡出
    auto fadeOut = FadeOut::create(0.5f);

    // 执行动画序列
    sun->runAction(Sequence::create(
        Spawn::create(easeOut, scaleDown, fadeOut, nullptr),
        CallFunc::create([sun, this, sunValue]() {
            // 增加阳光
            addSun(sunValue);

            // 清理用户数据
            if (sun->getUserData()) {
                delete (int*)sun->getUserData();
                sun->setUserData(nullptr);
            }

            // 移除阳光
            if (sun->getParent()) {
                sun->removeFromParent();
            }

            // 重置收集标志
            _isCollectingSun = false;

            CCLOG("Sun collection completed! Total suns: %d", _sunCount);
            }),
        nullptr
    ));
}

void SunSystem::safeRemoveSun(Sprite* sun) {
    if (!sun) return;

    // 从列表中移除阳光
    auto it = std::find(_suns.begin(), _suns.end(), sun);
    if (it != _suns.end()) {
        _suns.erase(it);
    }

    // 清理用户数据
    if (sun->getUserData()) {
        delete (int*)sun->getUserData();
        sun->setUserData(nullptr);
    }

    // 移除阳光
    if (sun->getParent()) {
        sun->removeFromParent();
    }
}