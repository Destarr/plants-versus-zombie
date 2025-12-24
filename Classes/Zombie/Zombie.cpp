#include "Zombie.h"
#include "GameScene/GameScene.h" // 假设您的游戏主场景类，用于获取屏幕尺寸等信息

USING_NS_CC;

bool Zombie::init() {
    if (!Sprite::init()) {
        return false;
    }

    // 初始化核心属性
    m_maxHealth = 100;
    m_health = m_maxHealth;
    m_speed = 20.0f;
    m_damage = 1;
    m_isDead = false;
    m_hasReachedHouse = false;
    m_gameScene = nullptr;  // 初始时场景指针为空，需要在创建后设置

    // 可以在此处设置僵尸的初始纹理
    this->setTexture("Zombie/NormalZombie.png");

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

    // 检查是否抵达房子边界
    if (m_gameScene) {
        // 调用GameScene的边界检测方法
        if (m_gameScene->checkZombieReachHouse(this)) {
            m_hasReachedHouse = true;
            // 僵尸抵达房子，可以在此处触发相关效果
            CCLOG("Zombie has reached the house at row %d", m_row);
        }
    }
    else {
        // 备用方案：如果未设置场景引用，使用简单边界检测
        if (this->getPositionX() < 50) {
            m_hasReachedHouse = true;
            CCLOG("Zombie reached house (fallback detection) at row %d", m_row);
        }
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

    // 停止所有可能存在的动作，例如移动动画
    this->stopAllActions();
    // 停止update函数的调度，死亡后不再需要每帧更新
    this->unscheduleUpdate();

    // 简单的淡出效果后移除
    auto fadeOut = cocos2d::FadeOut::create(0.5f); // 半秒内淡出
    auto removeSelf = cocos2d::RemoveSelf::create(true); // 从父节点移除自己
    auto sequence = cocos2d::Sequence::create(fadeOut, removeSelf, nullptr);
    this->runAction(sequence);

    CCLOG("Zombie died.");
}