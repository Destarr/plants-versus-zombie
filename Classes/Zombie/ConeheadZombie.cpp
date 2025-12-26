#include "ConeheadZombie.h"

bool ConeheadZombie::init() {
    // 先调用父类Zombie的初始化，保证基础逻辑生效
    if (!Zombie::init()) {
        return false;
    }

    // 覆盖父类的属性，设置路障僵尸的专属值
    m_maxHealth = 500;    // 路障僵尸血量更高（普通300）
    m_health = m_maxHealth; // 初始血量等于最大血量
    m_speed = m_originalSpeed;

    // 换皮：设置路障僵尸的专属纹理
    this->setTexture("Zombie/ConeheadZombie.png");
    this->setContentSize(this->getTexture()->getContentSize()); // 刷新尺寸
    this->setAnchorPoint(Vec2(0.5f, 0.5f)); // 保持锚点一致

    return true;
}