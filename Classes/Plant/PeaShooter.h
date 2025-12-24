#pragma once

#include "Plant.h"
#include "Bullet/Bullet.h"  // 假设有子弹类

/**
 * @class Peashooter
 * @brief 豌豆射手植物
 * @remark 继承自Plant基类，具有发射豌豆攻击僵尸的功能
 */
class Peashooter : public Plant {
public:
    Peashooter();
    virtual ~Peashooter();

    // 使用cocos2d-x的CREATE_FUNC宏
    CREATE_FUNC(Peashooter);

    /**
     * @brief 初始化方法
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;

    /**
     * @brief 每帧更新函数
     * @param delta 距离上一帧的时间间隔
     */
    virtual void update(float delta) override;

    /**
     * @brief 豌豆射手特有的攻击方法
     */
    virtual void attack() override;

    /**
     * @brief 种植到指定位置
     * @param row 行
     * @param col 列
     */
    virtual void planted(int row, int col) override;

private:
    /**
     * @brief 创建一颗豌豆子弹
     * @return 创建的子弹对象
     */
    Bullet* createPeaBullet();

    /**
     * @brief 检查是否需要攻击
     */
    void checkForAttack();

    // 豌豆射手特有的属性
    int m_peaDamage;           ///< 豌豆子弹的伤害
    float m_peaSpeed;          ///< 豌豆子弹的飞行速度
    std::string m_bulletType;  ///< 子弹类型（可用于区分普通豌豆和冰冻豌豆等）
};