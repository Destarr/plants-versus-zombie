#include "CherryBomb.h"
#include "cocos2d.h"

USING_NS_CC;

bool CherryBomb::init() {
    if (!Plant::init()) {
        return false;
    }

    // ========== 复用父类属性配置 ==========
    m_sunCost = 150;               // 阳光消耗（原版150）
    m_attackInterval = 1.0f;       // 爆炸前摇时间（1秒）
    m_attackCounter = m_attackInterval; // 初始倒计时1秒
    m_isReady = false;             // 初始未就绪，不可爆炸
    m_isDead = false;

    // ========== 樱桃炸弹专属配置 ==========
    m_explosionDamage = 1800;      // 爆炸伤害（秒杀所有普通僵尸）
    m_explosionRadius = 150.0f;    // 爆炸范围150像素（全场景大范围）
    m_zombiesList = nullptr;       // 初始化僵尸列表引用

    // ========== 外观设置 ==========
    // 移除父类默认的绿色占位符
    this->removeAllChildren();
    updateAppearance(); // 初始显示前摇外观

    CCLOG("CherryBomb initialized: pre-explode time=%.1fs, explosion damage=%d, radius=%.1f",
        m_attackInterval, m_explosionDamage, m_explosionRadius);

    return true;
}

void CherryBomb::update(float delta) {
    if (m_isDead) return;

    // ========== 复用父类倒计时逻辑（爆炸前摇） ==========
    if (m_attackCounter > 0) {
        m_attackCounter -= delta;
        // 前摇结束，触发爆炸
        if (m_attackCounter <= 0) {
            m_attackCounter = 0;
            m_isReady = true;      // 标记可爆炸
            CCLOG("CherryBomb row %d col %d pre-explode finished! Trigger explosion", m_row, m_col);
            this->attack();        // 自动触发爆炸（无需碰撞）
        }
    }
}

void CherryBomb::attack() {
    if (m_isDead) {
        CCLOG("CherryBomb attack failed: dead=%d, zombiesList=%p", m_isDead, m_zombiesList);
        return;
    }

    if (!m_zombiesList) {
        this->die();
        return;
    }

    CCLOG("===== CherryBomb Explosion =====");
    Vec2 bombPos = this->getPosition();
    CCLOG("Bomb position: (%.1f, %.1f), explosion radius: %.1f, damage: %d",
        bombPos.x, bombPos.y, m_explosionRadius, m_explosionDamage);
    CCLOG("Total zombies in scene: %zu", m_zombiesList->size());

    // ========== 全场景范围伤害逻辑（不限行） ==========
    for (auto zombie : *m_zombiesList) {
        // 跳过死亡僵尸
        if (!zombie || zombie->getIsDead()) {
            continue;
        }

        Vec2 zombiePos = zombie->getPosition();
        float distance = zombiePos.distance(bombPos);
        CCLOG("Zombie row %d, pos (%.1f, %.1f), distance to bomb: %.1f",
            zombie->getRow(), zombiePos.x, zombiePos.y, distance);

        // 范围内僵尸受爆炸伤害
        if (distance <= m_explosionRadius) {
            int oldHealth = zombie->getHealth();
            zombie->takeDamage(m_explosionDamage);
            int newHealth = zombie->getHealth();
            CCLOG("Zombie hit by CherryBomb! Old health: %d, New health: %d, Damage: %d",
                oldHealth, newHealth, m_explosionDamage);

        }
    }

    // ========== 爆炸后自身销毁 ==========
    this->die();
}

void CherryBomb::die() {
    if (m_isDead) return;
    m_isDead = true;

    // 停止更新和所有动作
    this->stopAllActions();
    this->unscheduleUpdate();

    // 爆炸特效（红色大范围圆形，模拟樱桃炸弹爆炸）,位置不准确
    auto explosion = DrawNode::create();
    explosion->drawSolidCircle(Vec2::ZERO, m_explosionRadius, 0, 64, Color4F(1.0f, 0.2f, 0.2f, 0.9f));
    this->addChild(explosion);

    // 爆炸后快速淡出销毁
    auto fadeOut = FadeOut::create(0.5f);
    auto removeSelf = CallFunc::create([this]() {
        this->removeFromParent();
        });
    auto sequence = Sequence::create(fadeOut, removeSelf, nullptr);
    this->runAction(sequence);

    CCLOG("CherryBomb row %d col %d destroyed after explosion", m_row, m_col);
}

void CherryBomb::updateAppearance() {
    // 清空现有外观
    this->removeAllChildren();
    this->setTexture("Plant/CherryBomb.png");
}