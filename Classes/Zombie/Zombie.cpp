// Zombie.cpp
#include "Zombie.h"

bool Zombie::init() {
    if (!Sprite::init()) {
        return false;
    }

    // 初始化核心属性
    m_maxHealth = 300;
    m_health = m_maxHealth;
    m_originalSpeed = 20.0f;
    m_speed = m_originalSpeed;
    m_damage = 100;
    m_isDead = false;
    m_hasReachedHouse = false;
    m_houseBoundary = 0.0f;  // 默认房子边界在x=0的位置
    m_attackInterval = 1.0f; // 攻击间隔1秒（可外部修改）
    m_attackCounter = 0.0f;  // 初始冷却完成，可立即攻击

    // 可以在此处设置僵尸的初始纹理
    this->setTexture("Zombie/NormalZombie.png");
    // 强制刷新contentSize
    this->setContentSize(this->getTexture()->getContentSize());
    // 锚点居中
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    // 开启每帧更新
    this->scheduleUpdate();

    return true;
}

void Zombie::update(float delta) {
    // 如果僵尸已经死亡，就不再进行任何逻辑更新
    if (m_isDead) {
        return;
    }

    // 如果僵尸不存活（生命值<=0）但尚未开始死亡流程，则触发死亡
    if (!isAlive() && !m_isDead) {
        this->die();
        return;
    }

    // 如果僵尸已抵达房子，不再移动
    if (m_hasReachedHouse) {
        return;
    }

    // 存活的僵尸向左移动（向房子前进）
    this->setPositionX(this->getPositionX() - m_speed * delta);

    // 更新攻击冷却计时器（每帧减少）
    if (m_attackCounter > 0) {
        m_attackCounter -= delta;
        // 防止负数
        if (m_attackCounter < 0) {
            m_attackCounter = 0;
        }
    }

    // 检查是否抵达房子边界
    if (this->getPositionX() <= m_houseBoundary) {
        m_hasReachedHouse = true;

        // 触发到达房子的回调
        if (m_reachHouseCallback) {
            m_reachHouseCallback(this);
        }

        // 僵尸抵达房子，可以在此处触发相关效果
        CCLOG("Zombie has reached the house at row %d, position x=%.1f", m_row, this->getPositionX());

        // 停止移动
        this->stopAllActions();
    }
}

void Zombie::takeDamage(int damage) {
    if (m_isDead) return; // 已死亡的僵尸不再受伤害

    m_health -= damage;
    if (m_health <= 0) {
        m_health = 0;
        // 注意：不在takeDamage中直接调用die()，而是在update中判断，避免在不确定的时机执行移除操作
    }
}

void Zombie::die() {
    // 标记僵尸为已死亡状态，防止重复处理
    m_isDead = true;

    // 停止所有可能存在的动作
    this->stopAllActions();
    // 停止update函数的调度
    this->unscheduleUpdate();

    // 简单的淡出效果后移除
    auto fadeOut = cocos2d::FadeOut::create(0.5f);
    auto removeSelf = cocos2d::RemoveSelf::create(true);
    auto sequence = cocos2d::Sequence::create(fadeOut, removeSelf, nullptr);
    this->runAction(sequence);

    CCLOG("Zombie died.");
}