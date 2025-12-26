#pragma once
#include "Bullet.h"
#include "Zombie/Zombie.h"

/**
 * @class SnowPea
 * @brief 寒冰子弹类，继承自Bullet基类
 * @remark 核心特性：命中僵尸后造成伤害，并附加减速效果
 */
class SnowPea : public Bullet {
public:
    CREATE_FUNC(SnowPea);

    // 减速相关常量（可配置）
    static const float SLOW_DURATION;  // 减速持续时间
    static const float SLOW_FACTOR;    // 减速系数（0.5表示速度减半）

    virtual bool init() override;
    // 重写命中僵尸逻辑，添加减速
    virtual void onHitZombie(Zombie* zombie) override;

private:
    SnowPea() = default;
    ~SnowPea() override = default;
};