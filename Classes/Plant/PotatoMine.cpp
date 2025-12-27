#include "PotatoMine.h"
#include "cocos2d.h"

USING_NS_CC;

bool PotatoMine::init() {
    if (!Plant::init()) {
        return false;
    }

    // ========== 复用父类属性配置 ==========
    m_sunCost = 25;                // 阳光消耗（原版25）
    m_attackInterval = 15.0f;       // 出土冷却时间（15秒）
    m_attackCounter = m_attackInterval; // 初始倒计时15秒
    m_isReady = false;             // 初始未出土，不可爆炸
    m_isDead = false;

    // ========== 土豆地雷专属配置 ==========
    m_isEmerged = false;           // 初始未出土
    m_explosionDamage = 1800;      // 爆炸伤害（秒杀普通僵尸）
    m_explosionRadius = 100.0f;     // 爆炸范围100像素
    m_zombiesList = nullptr;       // 初始化僵尸列表引用

    // ========== 外观设置 ==========
    // 移除父类默认的绿色占位符
    this->removeAllChildren();
    updateAppearance(); // 初始显示未出土外观

    CCLOG("PotatoMine initialized: emerge time=%.1fs, explosion damage=%d, radius=%.1f",
        m_attackInterval, m_explosionDamage, m_explosionRadius);

    return true;
}

void PotatoMine::update(float delta) {
    if (m_isDead) return;

    // ========== 复用父类倒计时逻辑（出土冷却） ==========
    if (m_attackCounter > 0) {
        m_attackCounter -= delta;
        // 出土冷却结束
        if (m_attackCounter <= 0) {
            m_attackCounter = 0;
            m_isReady = true;      // 标记可爆炸
            m_isEmerged = true;    // 标记已出土
            updateAppearance();    // 切换出土外观
            CCLOG("PotatoMine row %d col %d emerged! Ready to explode", m_row, m_col);
        }
    }
}

void PotatoMine::attack() {
    if (m_isDead || !m_isReady || !m_zombiesList) return;

    CCLOG("PotatoMine row %d col %d exploded! Radius=%.1f, damage=%d",
        m_row, m_col, m_explosionRadius, m_explosionDamage);

    // ========== 爆炸范围伤害逻辑 ==========
    Vec2 minePos = this->getPosition();

    for (auto zombie : *m_zombiesList) {
        if (zombie && zombie->isAlive() && !zombie->getIsDead() && zombie->getRow() == m_row) {
            // 计算僵尸与地雷的距离
            float distance = zombie->getPosition().distance(minePos);
            if (distance <= m_explosionRadius) {
                // 范围内僵尸受爆炸伤害
                zombie->takeDamage(m_explosionDamage);
                CCLOG("Zombie in explosion range! Distance=%.1f, damage=%d",
                    distance, m_explosionDamage);
            }
        }
    }

    // ========== 爆炸后自身销毁 ==========
    this->die();
}

void PotatoMine::die() {
    if (m_isDead) return;
    m_isDead = true;

    // 停止更新和所有动作
    this->stopAllActions();
    this->unscheduleUpdate();

    // 爆炸特效（可选，替换为纹理更美观）,位置不准确
    auto explosion = DrawNode::create();
    explosion->drawSolidCircle(Vec2::ZERO, m_explosionRadius, 0, 32, Color4F(1.0f, 0.5f, 0, 0.8f));
    this->addChild(explosion);

    // 爆炸后淡出销毁
    auto fadeOut = FadeOut::create(0.3f);
    auto removeSelf = CallFunc::create([this]() {
        this->removeFromParent();
        });
    auto sequence = Sequence::create(fadeOut, removeSelf, nullptr);
    this->runAction(sequence);

    CCLOG("PotatoMine row %d col %d destroyed after explosion", m_row, m_col);
}

void PotatoMine::onZombieCollision(Zombie* zombie) {
    // 仅出土后触碰才触发爆炸
    if (m_isReady && !m_isDead && zombie && zombie->isAlive()) {
        CCLOG("Zombie touched PotatoMine! Trigger explosion");
        this->attack(); // 触发爆炸攻击
    }
}

void PotatoMine::updateAppearance() {
    // 清空现有外观
    this->removeAllChildren();

    if (m_isEmerged) {
        // 出土外观：（地雷激活状态）
        this->setTexture("Plant/PotatoMine1.png");
    }
    else {
        // 未出土外观：（冷却中）
        this->setTexture("Plant/PotatoMine0.png");
    }

    this->setAnchorPoint(Vec2(0.5f, 0.5f)); // 锚点居中
}