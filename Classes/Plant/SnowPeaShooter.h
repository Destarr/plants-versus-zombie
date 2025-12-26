#pragma once
#include "Peashooter.h"
#include "Bullet/SnowPea.h"

/**
 * @class SnowPeaShooter
 * @brief 寒冰射手类，继承自Peashooter基类
 * @remark 核心功能：每隔1.5秒发射一颗寒冰豌豆子弹，命中僵尸后造成伤害并减速
 */
class SnowPeaShooter : public Peashooter {
public:
    // 寒冰射手专属常量（阳光消耗比普通豌豆高）
    static const int SUN_COST = 175;

    CREATE_FUNC(SnowPeaShooter);

    virtual bool init() override;

private:
    SnowPeaShooter() = default;
    ~SnowPeaShooter() override = default;
};