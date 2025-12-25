#include "SunBeam.h"

SunBeam::SunBeam()
    : m_value(25)
    , m_isCollected(false)
    , m_isDead(false)
    , m_lifeTime(DEFAULT_LIFE_TIME)
    , m_aliveTime(0.0f) {
}

SunBeam::~SunBeam() {}

bool SunBeam::init() {
    // 第一步：确保Sprite基类初始化成功
    if (!Sprite::init()) {
        CCLOG("SunBeam init failed: Sprite base init failed!");
        return false;
    }

    // 第二步：尝试加载纹理，失败则绘制黄色圆形占位符
    this->setTexture("SunBeam/SunBeam.png");


    // 第三步：统一设置尺寸和锚点
    this->setContentSize(Size(SUN_DEFAULT_SIZE, SUN_DEFAULT_SIZE));
    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->setScale(SUN_DEFAULT_SIZE / this->getContentSize().width);

    // 第四步：开启触摸监听（实现点击收集）
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true); // 吞噬触摸事件，避免穿透
    // 触摸开始时检测是否点击到阳光
    listener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        if (m_isDead || m_isCollected) return false;

        // 转换触摸坐标到本地坐标系
        Vec2 touchPos = this->convertTouchToNodeSpace(touch);
        // 检测点击是否在阳光范围内
        if (this->getBoundingBox().containsPoint(touchPos)) {
            this->collect(); // 执行收集逻辑
            return true;
        }
        return false;
        };
    // 添加监听到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // 第五步：开启update调度（管理生命周期）
    this->scheduleUpdate();

    CCLOG("SunBeam init success! LifeTime: %.1fs, Size: %.1fx%.1f",
        m_lifeTime, this->getContentSize().width, this->getContentSize().height);
    return true;
}

// 新增：帧更新逻辑（管理存活时间、检测是否死亡）
void SunBeam::update(float delta) {
    if (m_isDead || m_isCollected) return;

    // 累计已存活时间
    m_aliveTime += delta;

    // 超时未收集则自动销毁
    if (m_aliveTime >= m_lifeTime) {
        CCLOG("SunBeam timeout! Alive time: %.1fs, Life time: %.1fs", m_aliveTime, m_lifeTime);
        this->die();
        return;
    }

    // 可选：存活最后3秒闪烁提示（增强交互）
    if (m_aliveTime >= m_lifeTime - 3.0f && !this->getActionByTag(100)) {
        auto blink = Blink::create(1.0f, 1);
        blink->setTag(100); // 标记动画，避免重复添加
        this->runAction(RepeatForever::create(blink));
    }
}

// 新增：阳光被收集的核心逻辑
void SunBeam::collect() {
    if (m_isDead || m_isCollected) return;

    m_isCollected = true;
    CCLOG("SunBeam collected! Value: %d", m_value);

    // 执行收集回调（通知场景增加阳光数值）
    if (onCollectCallback) {
        onCollectCallback(m_value);
    }

    // 收集动画：缩小+淡出
    auto scaleDown = ScaleTo::create(0.3f, 0.0f);
    auto fadeOut = FadeOut::create(0.3f);
    auto spawn = Spawn::create(scaleDown, fadeOut, nullptr);
    auto dieCall = CallFunc::create([this]() {
        this->die();
        });
    this->runAction(Sequence::create(spawn, dieCall, nullptr));
}

// 新增：阳光销毁逻辑（统一销毁入口）
void SunBeam::die() {
    if (m_isDead) return;

    m_isDead = true;
    CCLOG("SunBeam die! Collected: %d, Value: %d", m_isCollected, m_value);

    // 停止所有动画和更新
    this->stopAllActions();
    this->unscheduleUpdate();

    // 移除触摸监听（避免内存泄漏）
    _eventDispatcher->removeEventListenersForTarget(this);

    // 延迟移除节点（确保动画执行完）
    this->runAction(Sequence::create(
        DelayTime::create(0.1f),
        CallFunc::create([this]() {
            this->removeFromParent();
            }),
        nullptr
    ));
}

void SunBeam::showCollectableAnimation() {
    // 停止原有动画，避免重复执行
    this->stopAllActions();

    // 上升+悬浮动画
    auto moveUp = MoveBy::create(1.0f, Vec2(0, 20));
    auto float1 = MoveBy::create(0.5f, Vec2(0, 5));
    auto float2 = MoveBy::create(0.5f, Vec2(0, -5));
    auto floatLoop = RepeatForever::create(Sequence::create(float1, float2, nullptr));

    this->runAction(Sequence::create(moveUp, floatLoop, nullptr));
    CCLOG("SunBeam animation started!");
}