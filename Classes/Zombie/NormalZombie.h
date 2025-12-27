// Zombie.h
#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "cocos2d.h"

USING_NS_CC;

class Zombie : public Sprite {
public:
    CREATE_FUNC(Zombie);

    /**
     * @brief 初始化方法
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;

    /**
     * @brief 每帧更新函数，负责移动、攻击等逻辑
     * @param delta 距离上一帧的时间间隔
     */
    virtual void update(float delta) override;

    /**
     * @brief 僵尸承受伤害
     * @param damage 受到的伤害值
     */
    void takeDamage(int damage);

    /**
     * @brief 执行僵尸的死亡流程
     */
    void die();

    /**
     * @brief 判断僵尸是否存活
     * @return 存活返回true，否则返回false
     */
    bool isAlive() const { return m_health > 0; }

    // 使用CC_SYNTHESIZE宏定义属性（自动生成getter和setter）
    CC_SYNTHESIZE(int, m_health, Health);          ///< 当前生命值
    CC_SYNTHESIZE(int, m_maxHealth, MaxHealth);    ///< 最大生命值
    CC_SYNTHESIZE(float, m_speed, Speed);          ///< 移动速度（像素/秒）
    CC_SYNTHESIZE(float, m_originalSpeed, OriginalSpeed);
    CC_SYNTHESIZE(int, m_damage, Damage);          ///< 攻击力
    CC_SYNTHESIZE(int, m_row, Row);                ///< 僵尸所在的行（0-4）
    CC_SYNTHESIZE(bool, m_isDead, IsDead);         ///< 是否已死亡
    CC_SYNTHESIZE(float, m_attackInterval, AttackInterval); 
    CC_SYNTHESIZE(bool, m_hasReachedHouse, HasReachedHouse); ///< 是否已抵达房子

    /**
     * @brief 设置房子边界位置
     * @param houseBoundary 房子的x坐标边界
     */
    void setHouseBoundary(float houseBoundary) { m_houseBoundary = houseBoundary; }

    /**
     * @brief 设置到达房子的回调函数
     * @param callback 回调函数
     */
    void setReachHouseCallback(const std::function<void(Zombie*)>& callback) {
        m_reachHouseCallback = callback;
    }


    void setSpeedFactor(float factor, float duration);

    float m_attackCounter;

private:
    float m_houseBoundary;  ///< 房子的边界位置（默认50.0f）
    std::function<void(Zombie*)> m_reachHouseCallback; ///< 到达房子的回调函数

    // 减速相关属性
    bool m_isSlowed;        ///< 减速状态标签（是否正在减速）
    float m_speedFactor;    ///< 减速系数（0~1）
    float m_slowCounter;    ///< 减速计时计数器（剩余减速时间）
};

#endif // ZOMBIE_H

