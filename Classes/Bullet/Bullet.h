#pragma once
#ifndef __BULLET_H__
#define __BULLET_H__


#include "cocos2d.h"
#include "Zombie/Zombie.h"
USING_NS_CC;

// 前向声明，避免循环引用
class Zombie;

class Bullet : public Sprite {
public:
    CREATE_FUNC(Bullet);

    virtual bool init() override;
    virtual void update(float delta) override;

    CC_SYNTHESIZE(int, m_damage, Damage);
    CC_SYNTHESIZE(float, m_speed, Speed);
    CC_SYNTHESIZE(int, m_row, Row);
    CC_SYNTHESIZE(Vec2, m_direction, Direction);
    CC_SYNTHESIZE(std::string, m_type, Type);
    CC_SYNTHESIZE(bool, m_isActive, IsActive);
    CC_SYNTHESIZE(bool, m_isDead, IsDead);

    virtual void activate(const Vec2& startPosition, int row);
    virtual void die();
    // 新增：碰撞僵尸的回调接口
    virtual void onHitZombie(Zombie* zombie);
};

#endif