#include "SnowPeaShooter.h"

bool SnowPeaShooter::init() {
    // 先调用父类（豌豆射手）的初始化
    if (!Peashooter::init()) {
        return false;
    }

    // 覆盖豌豆射手的属性为寒冰射手专属
    this->setSunCost(SUN_COST);
    // 寒冰射手攻击间隔和普通豌豆一致（可按需修改）
    this->setAttackInterval(1.5f);
    // 替换纹理为寒冰射手
    this->setTexture("Plant/SnowPeaShooter.png");

    CCLOG("SnowPeaShooter initialized, sun cost: %d, attack interval: %.1f", SUN_COST, getAttackInterval());
    return true;
}