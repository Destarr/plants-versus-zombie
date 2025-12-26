#include "SnowPea.h"

// 初始化减速常量
const float SnowPea::SLOW_DURATION = 3.0f;  // 减速持续3秒
const float SnowPea::SLOW_FACTOR = 0.5f;    // 僵尸速度变为原来的50%

bool SnowPea::init() {
    // 先调用父类初始化
    if (!Bullet::init()) {
        return false;
    }

    // 替换子弹纹理为寒冰豌豆
    this->setTexture("Bullet/SnowPea.png");
    // 设置子弹类型标识
    this->setType("snow_pea");


    CCLOG("SnowPea initialized, damage: %d, slow factor: %.1f", this->getDamage(), SLOW_FACTOR);
    return true;
}

void SnowPea::onHitZombie(Zombie* zombie) {
    // 先执行父类的基础命中逻辑（造成伤害+子弹死亡）
    Bullet::onHitZombie(zombie);

    // 新增：给僵尸添加减速效果
    if (zombie && zombie->isAlive()) {
        zombie->setSpeedFactor(SLOW_FACTOR, SLOW_DURATION);
        CCLOG("Zombie slowed! Speed factor: %.1f, duration: %.1f", SLOW_FACTOR, SLOW_DURATION);
    }
}