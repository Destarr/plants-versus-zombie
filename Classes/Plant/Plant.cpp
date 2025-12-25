#include "Plant.h"

Plant::Plant()
    : m_sunCost(0)
    , m_health(300)
    , m_maxHealth(300)
    , m_row(-1)
    , m_col(-1)
    , m_damage(0)
    , m_attackInterval(0.0f)
    , m_isReady(false)
    , m_isDead(false)
    , m_attackCounter(0.0f)
    , m_gridOrigin(Vec2::ZERO) {
}

Plant::~Plant() {}

bool Plant::init() {
    if (!Sprite::init()) {
        return false;
    }

    // 初始化攻击默认值（子类可覆盖）
    m_attackInterval = 1.0f; // 默认1秒攻击间隔
    m_damage = 1;            // 默认攻击力1
    m_isReady = true;        // 初始可攻击

    // 绿色占位符（保留）
    auto drawer = DrawNode::create();
    Vec2 rectangle[4] = { Vec2(-20, -20), Vec2(20, -20), Vec2(20, 20), Vec2(-20, 20) };
    drawer->drawPolygon(rectangle, 4, Color4F(0, 1.0f, 0, 0.5f), 1, Color4F(0, 0.5f, 0, 1.0f));
    this->addChild(drawer);

    this->scheduleUpdate();
    return true;
}

void Plant::update(float delta) {
    if (m_isDead) return;

    if (m_attackCounter > 0) {
        m_attackCounter -= delta;
        if (m_attackCounter <= 0) {
            m_attackCounter = 0;
            m_isReady = true;
        }
    }
}

void Plant::takeDamage(int damage) {
    if (m_isDead) return; // 死亡后不再受伤害

    m_health -= damage;
    if (m_health <= 0) {
        m_health = 0;
        this->die();
    }

}

void Plant::die() {
    if (m_isDead) return; // 防止重复调用
    m_isDead = true;

    this->stopAllActions();
    this->unscheduleUpdate();

    auto fadeOut = FadeOut::create(0.5f);
    auto remove = CallFunc::create([this]() {
        this->removeFromParent();
        });
    auto sequence = Sequence::create(fadeOut, remove, nullptr);
    this->runAction(sequence);

    CCLOG("Plant died at row %d, col %d", m_row, m_col);
}

// 修改：同步设置屏幕坐标
void Plant::planted(int row, int col, float gridWidth, float gridHeight) {
    m_row = row;
    m_col = col;
    // 计算植物屏幕坐标（格子中心）
    float x = m_gridOrigin.x + col * gridWidth + gridWidth / 2;
    float y = m_gridOrigin.y + row * gridHeight + gridHeight / 2;
    this->setPosition(Vec2(x, y));
    CCLOG("Plant planted at row %d, col %d, position (%.1f, %.1f)", row, col, x, y);
}

void Plant::attack() {
    // 提供一个默认实现，可以是空的
    // 或者有基本的攻击逻辑
    CCLOG("Plant is attacking!");
}

