#include "Plant.h"

/**
 * Plant 类构造函数
 * 初始化成员变量为默认值
 */
Plant::Plant()
    : m_sunCost(0)
    , m_health(100)
    , m_maxHealth(100)
    , m_row(-1)  // -1 表示未种植状态
    , m_col(-1)
    , m_attack(0)
    , m_attackInterval(0.0f)
    , m_isReady(false)
    , m_isDead(false)
    , m_attackCounter(0.0f) {
}

/**
 * Plant 类虚析构函数
 * 用于清理资源，目前无需特殊操作
 */
Plant::~Plant() {
    // 如果未来有动态分配的资源，在此释放
}

/**
 * 初始化函数
 * 设置精灵纹理并启动更新调度器
 */
bool Plant::init() {
    // 1. 首先调用父类 Sprite 的 init 方法
    if (!Sprite::init()) {
        return false;
    }

    // 2. 设置一个默认的植物纹理（具体植物子类可能会覆盖此设置）
    // 例如：this->setTexture("plants/DefaultPlant.png");
    // 临时创建一个纯色矩形作为占位符，便于调试
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto drawer = DrawNode::create();
    // 绘制一个绿色的矩形代表植物
    Vec2 rectangle[4];
    rectangle[0] = Vec2(-20, -20);  // 左下角
    rectangle[1] = Vec2(20, -20);  // 右下角
    rectangle[2] = Vec2(20, 20);  // 右上角
    rectangle[3] = Vec2(-20, 20);  // 左上角
    drawer->drawPolygon(rectangle, 4, Color4F(0, 1.0f, 0, 0.5f), 1, Color4F(0, 0.5f, 0, 1.0f));
    this->addChild(drawer);

    // 3. 设置植物为未准备状态（不可攻击）
    m_isReady = false;

    // 4. 启动update函数调度，每帧自动调用
    this->scheduleUpdate();

    return true;
}

/**
 * 每帧更新函数
 * 负责处理攻击冷却等与时间相关的逻辑
 * @param delta 距离上一帧的时间间隔（秒）
 */
void Plant::update(float delta) {
    // 如果植物已经死亡，则不再进行任何更新
    if (m_isDead) {
        return;
    }

    // 更新攻击冷却计时器
    if (m_attackCounter > 0) {
        m_attackCounter -= delta;
        // 检查冷却是否结束
        if (m_attackCounter <= 0) {
            m_attackCounter = 0; // 避免负值
            m_isReady = true;    // 植物准备就绪，可以攻击
        }
    }
}

void Plant::attack() {
    // 提供一个默认实现，可以是空的
    // 或者有基本的攻击逻辑
    CCLOG("Plant is attacking!");
}

/**
 * 植物受到伤害的处理
 * @param damage 受到的伤害值
 */
void Plant::takeDamage(int damage) {
    // 扣除生命值
    m_health -= damage;

    // 检查植物是否死亡
    if (m_health <= 0) {
        m_health = 0;
        this->die(); // 调用死亡函数
    }

    // 可以在此处添加受伤特效或音效
    // 例如：播放一个闪烁动画提示受伤
    auto blink = Blink::create(0.3f, 1); // 快速闪烁一次
    this->runAction(blink);
}

/**
 * 植物死亡处理
 * 播放死亡动画，并安排从父节点中移除
 */
void Plant::die() {
    m_isDead = true;

    // 停止所有当前动作和更新调度
    this->stopAllActions();
    this->unscheduleUpdate();

    // 此处应播放死亡动画
    // 例如：使用序列动作，先播放死亡动画，然后移除自身
    auto fadeOut = FadeOut::create(0.5f); // 淡出效果
    auto remove = CallFunc::create([this]() {
        this->removeFromParent(); // 从父节点移除
        });
    auto sequence = Sequence::create(fadeOut, remove, nullptr);
    this->runAction(sequence);

    // 在实际项目中，还应通知游戏场景（GameScene）更新状态
    // 例如：m_delegate->onPlantDied(this);
}

void Plant::planted(int row, int col) {
    m_row = row;
    m_col = col;
    CCLOG("Plant planted at row %d, col %d", row, col);
}