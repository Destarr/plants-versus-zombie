#pragma once

#include "cocos2d.h"

class Zombie : public cocos2d::Sprite {
public:
    CREATE_FUNC(Zombie); ///< 使用cocos2d-x的CREATE_FUNC宏来简化create()函数的编写

    /**
     * @brief 初始化方法
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;

    /**
     * @brief 每帧更新函数，负责移动等逻辑
     * @param delta 距离上一帧的时间间隔
     */
    virtual void update(float delta) override;

    void generate(int row);
    
    /**
     * @brief 僵尸承受伤害
     * @param damage 受到的伤害值
     */
    void takeDamage(int damage);

    /**
     * @brief 执行僵尸的死亡流程
     */
    void die();


    // 使用CC_SYNTHESIZE宏定义属性（自动生成getter和setter）
    CC_SYNTHESIZE(int, m_health, Health);          ///< 当前生命值
    CC_SYNTHESIZE(int, m_maxHealth, MaxHealth);    ///< 最大生命值
    CC_SYNTHESIZE(float, m_speed, Speed);          ///< 移动速度（像素/秒）
    CC_SYNTHESIZE(int, m_damage, Damage);          ///< 攻击力
    CC_SYNTHESIZE(int, m_row, Row);                ///< 僵尸所在的行（0-4）
    CC_SYNTHESIZE(bool, m_isDead, IsDead);         ///< 是否已死亡（用于逻辑标记）

};

