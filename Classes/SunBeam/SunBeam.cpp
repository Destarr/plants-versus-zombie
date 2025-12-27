#include "SunBeam.h"

SunBeam::SunBeam()
    : m_value(25)
    , m_isCollected(false)
    , m_isDead(false){
}

SunBeam::~SunBeam() {
    unregisterTouchListener(); // 析构时确保注销监听
}

bool SunBeam::init() {
    // 第一步：确保Sprite基类初始化成功
    if (!Sprite::init()) {
        CCLOG("SunBeam init failed: Sprite base init failed!");
        return false;
    }

    // 显式重置生命周期（核心：覆盖任何脏数据）
    m_lifeTime = DEFAULT_LIFE_TIME;
    m_aliveTime = 0.0f; // 强制重置计时

    // 第二步：尝试加载纹理，失败则绘制黄色圆形占位符
    this->setTexture("SunBeam/SunBeam.png");


    // 第三步：统一设置尺寸和锚点
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 第四步：注册触摸监听（核心：启用点击收集）
    registerTouchListener();

    // 第五步：开启update调度（管理生命周期）
    this->scheduleUpdate();

    //CCLOG("SunBeam init success! LifeTime: %.1fs, Size: %.1fx%.1f",
        //m_lifeTime, this->getContentSize().width, this->getContentSize().height);
    return true;
}

void SunBeam::registerTouchListener() {
    if (m_touchListener) return;

    m_touchListener = EventListenerTouchOneByOne::create();
    m_touchListener->setSwallowTouches(true); 

    m_touchListener->onTouchBegan = CC_CALLBACK_2(SunBeam::onTouchBegan, this);
    m_touchListener->onTouchEnded = CC_CALLBACK_2(SunBeam::onTouchEnded, this);

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 关键1：优先级设为-200（比绝大多数UI/场景层更高）
    // 关键2：监听器绑定当前节点（第二个参数），避免全局混乱
    dispatcher->addEventListenerWithFixedPriority(m_touchListener, -200);

    CCLOG("===== 阳光触摸监听器注册成功（优先级-200） =====");
}

// 注销时要匹配优先级，避免内存泄漏
void SunBeam::unregisterTouchListener() {
    if (!m_touchListener) return;

    auto dispatcher = Director::getInstance()->getEventDispatcher();
    // 直接移除监听器（无需判断是否存在，cocos2d-x 内部会处理）
    dispatcher->removeEventListener(m_touchListener);
    m_touchListener = nullptr; // 置空避免野指针
}

// 触摸开始：判断是否点击到阳光
bool SunBeam::onTouchBegan(Touch* touch, Event* event) {
    if (m_isDead || m_isCollected) return false;

    // 世界坐标系判定（稳定）
    Vec2 touchWorldPos = touch->getLocation();
    Rect sunWorldBox = this->getBoundingBox();

    if (sunWorldBox.containsPoint(touchWorldPos)) {
        CCLOG("===== 阳光命中，拦截触摸事件 =====");
        return true; // 命中则吞噬事件，不再传递
    }
    else {
        CCLOG("===== 阳光未命中，放行触摸事件 =====");
        return false; // 未命中则放行，让其他层响应
    }
}

// 触摸结束：执行阳光收集逻辑
void SunBeam::onTouchEnded(Touch* touch, Event* event) {
    if (m_isDead || m_isCollected) return;

    // 统一用世界坐标判定（和onTouchBegan保持一致）
    Vec2 touchWorldPos = touch->getLocation();
    Rect sunWorldBox = this->getBoundingBox();

    // 二次校验：确保触摸结束时仍在阳光范围内
    if (sunWorldBox.containsPoint(touchWorldPos)) {
        CCLOG("===== 阳光触摸结束，触发收集 =====");
        this->collect(); // 触发收集逻辑
    }
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

}

// 新增：阳光被收集的核心逻辑
void SunBeam::collect() {
    if (m_isDead || m_isCollected) return;

    m_isCollected = true;
    CCLOG("===== SunBeam collected! Value: %d =====", m_value);

    // 执行收集回调
    if (onCollectCallback) {
        onCollectCallback(m_value);
    }

    // 立即停止悬浮动画（避免位置偏移导致判定异常）
    this->stopAllActionsByTag(1001); // 给悬浮动画加tag，精准停止

    // 恢复收集动画（缩小+淡出）
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
    //CCLOG("SunBeam die! Collected: %d, Value: %d", m_isCollected, m_value);

    // 注销触摸监听（核心：防止空指针）
    unregisterTouchListener();

    // 停止所有动画和更新
    this->stopAllActions();
    this->unscheduleUpdate();

    auto fadeOut = FadeOut::create(0.5f);
    auto remove = CallFunc::create([this]() {
        this->removeFromParent();
        });
    auto sequence = Sequence::create(fadeOut, remove, nullptr);
    this->runAction(sequence);
}

void SunBeam::showCollectableAnimation() {
    // 先停止已有动画
    this->stopAllActions();

    // 上升+悬浮动画
    auto moveUp = MoveBy::create(1.0f, Vec2(0, 20));
    auto float1 = MoveBy::create(0.5f, Vec2(0, 5));
    auto float2 = MoveBy::create(0.5f, Vec2(0, -5));
    auto floatLoop = RepeatForever::create(Sequence::create(float1, float2, nullptr));
    floatLoop->setTag(1001); // 给悬浮动画标记，方便精准停止

    this->runAction(Sequence::create(moveUp, floatLoop, nullptr));
}