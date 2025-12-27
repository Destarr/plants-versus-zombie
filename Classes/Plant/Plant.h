#pragma once
#ifndef __PLANT_H__
#define __PLANT_H__

#include "cocos2d.h"
USING_NS_CC;

class Plant : public cocos2d::Sprite {
public:
    Plant();
    virtual ~Plant();
    CREATE_FUNC(Plant);

    virtual bool init() override;
    virtual void update(float delta) override;

    CC_SYNTHESIZE(int, m_sunCost, SunCost);
    CC_SYNTHESIZE(int, m_health, Health);
    CC_SYNTHESIZE(int, m_maxHealth, MaxHealth);
    CC_SYNTHESIZE(int, m_row, Row);
    CC_SYNTHESIZE(int, m_col, Col);
    CC_SYNTHESIZE(int, m_damage, Damage); // 建议重命名为m_damage，与子弹统一
    CC_SYNTHESIZE(float, m_attackInterval, AttackInterval);
    CC_SYNTHESIZE(bool, m_isReady, IsReady);
    CC_SYNTHESIZE(bool, m_isDead, IsDead);
    CC_SYNTHESIZE(cocos2d::Vec2, m_gridOrigin, GridOrigin);// 格子基准坐标（由GameScene传入）

    virtual void takeDamage(int damage);

    virtual void attack();
    virtual void die();
    // 传入格子大小，同步设置屏幕坐标
    virtual void planted(int row, int col, float gridWidth, float gridHeight);


    float m_attackCounter;
};

#endif