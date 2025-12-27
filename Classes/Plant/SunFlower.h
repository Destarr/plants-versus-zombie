#pragma once
#include "Plant.h"
#include "SunBeam/SunBeam.h"

class SunFlower : public Plant {
public:
    SunFlower();
    virtual ~SunFlower();
    CREATE_FUNC(SunFlower);

    // 重写父类方法
    virtual bool init() override;
    virtual void update(float delta) override;
    virtual void attack() override; // 这里attack逻辑改为生产阳光

private:
    // 向日葵专属配置
    const int SUN_FLOWER_COST = 50;    // 向日葵阳光消耗
    const int SUN_FLOWER_HEALTH = 300; // 向日葵生命值
    const float SUN_PRODUCE_INTERVAL = 5.0f; // 产阳光间隔（秒）
    const int SUN_PRODUCE_VALUE = 25;  // 每次产阳光的数值

    // 生成阳光的核心方法
    void produceSun();
};