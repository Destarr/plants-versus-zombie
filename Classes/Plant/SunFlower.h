#ifndef __SUN_FLOWER_H__
#define __SUN_FLOWER_H__

#include "Plant.h"

class SunFlower : public Plant {
public:
    SunFlower();
    virtual ~SunFlower();
    CREATE_FUNC(SunFlower);

    // 重写父类方法
    virtual bool init() override;
    virtual void update(float delta) override;
    virtual void attack() override; // 这里attack逻辑改为生产阳光

    // 设置阳光生产间隔
    void setSunProduceInterval(float interval) { _sunProduceInterval = interval; }
    float getSunProduceInterval() const { return _sunProduceInterval; }

    void initVisual();
    void produceSun();
    void playProduceAnimation();

private:
    // 向日葵专属配置
    const int SUN_FLOWER_COST = 50;    // 向日葵阳光消耗
    const int SUN_FLOWER_HEALTH = 300; // 向日葵生命值
    const int SUN_PRODUCE_VALUE = 25;  // 每次产阳光的数值
    const int SUN_PRODUCE_INTERVAL = 5;

    // 阳光生产相关
    float _sunProduceInterval;         // 产阳光间隔（秒）
    float _sunProduceTimer;           // 产阳光计时器
    bool _canProduceSun;              // 是否可以生产阳光
};

#endif // __SUN_FLOWER_H__