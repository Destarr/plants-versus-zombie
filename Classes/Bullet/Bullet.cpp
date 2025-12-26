#include "Bullet.h"
// 引入Zombie头文件（实际碰撞检测类需包含）


bool Bullet::init() {
    if (!Sprite::init()) {
        return false;
    }

    m_damage = 1;
    m_speed = 300.0f;
    m_row = 0;
    // 明确注释：默认向右（豌豆射手攻击方向），可通过setDirection修改
    m_direction = Vec2(1, 0); 
    m_type = "pea";
    m_isActive = false;
    m_isDead = false;

    this->setVisible(false);
    this->scheduleUpdate();

    // 默认豌豆子弹纹理
    this->setTexture("Bullet/Pea.png");

    return true;
}

void Bullet::update(float delta) {
    if (m_isDead || !m_isActive) return;

    Vec2 moveDistance = m_direction * m_speed * delta;
    this->setPosition(this->getPosition() + moveDistance);

    Vec2 currentPos = this->getPosition();
    Size visibleSize = Director::getInstance()->getVisibleSize();

    if (currentPos.x < -this->getContentSize().width || currentPos.x > visibleSize.width + this->getContentSize().width) {
        this->die();
    }
}

void Bullet::activate(const Vec2& startPosition, int row) {
    if (m_isActive) return; // 避免重复激活
    m_isActive = true;
    m_isDead = false;
    m_row = row;

    this->setPosition(startPosition);
    this->setVisible(true);
}

void Bullet::die() {
    if (m_isDead) return; // 防止重复调用
    m_isDead = true;
    m_isActive = false;

    this->stopAllActions();
    this->setVisible(false);

    CCLOG("Bullet died and is ready for recycling.");
}

// 碰撞僵尸的逻辑
void Bullet::onHitZombie(Zombie* zombie) {
    if (m_isDead || !zombie || !zombie->isAlive()) return;
    // 僵尸受伤害
    zombie->takeDamage(m_damage);
    // 子弹命中后死亡
    this->die();
    CCLOG("Bullet hit zombie, damage: %d", m_damage);
}