#include "BucketheadZombie.h"

bool BucketheadZombie::init() {
    // 先初始化父类
    if (!Zombie::init()) {
        return false;
    }

    // 铁桶僵尸属性：血量更高，移动更慢
    m_maxHealth = 1000;   // 铁桶僵尸血量远高于普通和路障
    m_health = m_maxHealth;
    m_speed = m_originalSpeed;

    // 换皮：设置铁桶僵尸的专属纹理
    this->setTexture("Zombie/BucketheadZombie.png");
    this->setContentSize(this->getTexture()->getContentSize());
    this->setAnchorPoint(Vec2(0.5f, 0.5f));

    return true;
}