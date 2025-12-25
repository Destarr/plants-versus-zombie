#include "Peashooter.h"

bool Peashooter::init() {
    // 先调用父类初始化
    if (!Plant::init()) {
        return false;
    }

    // 设置豌豆射手专属属性
    this->setSunCost(SUN_COST);
    this->setDamage(ATTACK_DAMAGE);
    this->setMaxHealth(300);
    this->setAttackInterval(1.5f);
    this->setHealth(this->getMaxHealth());

    // 替换植物占位符为豌豆射手样式
    this->removeAllChildren(); // 移除父类的默认占位符
    this->setTexture("Plant/Peashooter.png");

    CCLOG("Peashooter initialized, sun cost: %d, attack interval: %.1f", SUN_COST, getAttackInterval());
    return true;
}

void Peashooter::attack() {
    // 1. 基础判定：是否死亡/是否就绪/是否有子弹创建回调
    if (!m_isReady || m_isDead || !m_createBulletCallback) {
        return;
    }

    // 2. 核心新增：判定当前行是否有存活僵尸
    bool hasZombieInRow = false;
    if (m_zombieCheckCallback) {
        hasZombieInRow = m_zombieCheckCallback(this->getRow()); // 传入当前行号
    }

    // 3. 无僵尸则直接返回，不发射子弹
    if (!hasZombieInRow) {
        CCLOG("Peashooter row %d: No zombie in row, skip attack", this->getRow());
        return;
    }

    // 4. 有僵尸则正常发射子弹
    m_isReady = false;
    m_attackCounter = m_attackInterval;

    // 子弹发射位置：植物右侧（避免碰撞自身）
    Vec2 bulletStartPos = this->getPosition() + Vec2(this->getContentSize().width / 2 + 10, 0);

    Bullet* bullet = m_createBulletCallback();
    if (bullet) {
        bullet->setDamage(this->getDamage());
        bullet->activate(bulletStartPos, this->getRow());
        this->getParent()->addChild(bullet, 10);

        CCLOG("Peashooter row %d attack! Bullet created at (%.1f, %.1f)",
            this->getRow(), bulletStartPos.x, bulletStartPos.y);
    }
}

// 重写update（可选，若需自定义攻击逻辑）
void Peashooter::update(float delta) {
    // 先执行父类的冷却逻辑
    Plant::update(delta);

    // 若可攻击，则立即攻击（实际项目中需先检测同行是否有僵尸）
    if (m_isReady && !m_isDead) {
        this->attack();
    }
}