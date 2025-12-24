#pragma once

#include "cocos2d.h"
USING_NS_CC;

/**
 * @class Plant
 * @brief 植物基类。
 * @remark 所有具体植物（如豌豆射手、向日葵）都应继承自这个类。
 *
 * 设计说明：
 * 1. 继承自cocos2d::Sprite，直接使用精灵的显示功能。
 * 2. 使用CREATE_FUNC宏提供标准的创建接口。
 * 3. 成员变量使用'm_'前缀，便于区分局部变量。
 * 4. 属性通过CC_SYNTHESIZE宏暴露，便于外部安全访问。
 */
class Plant : public cocos2d::Sprite {
public:
    Plant(); ///< 构造函数
    virtual ~Plant(); ///< 虚析构函数，确保派生类能正确释放资源

    // 使用cocos2d-x的CREATE_FUNC宏来简化create()函数的编写
    CREATE_FUNC(Plant);

    /**
     * @brief 初始化方法
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;

    /**
     * @brief 每帧更新函数，负责处理攻击冷却、状态更新等逻辑
     * @param delta 距离上一帧的时间间隔
     */
    virtual void update(float delta) override;

    // 使用CC_SYNTHESIZE宏定义属性（自动生成getter和setter）
    CC_SYNTHESIZE(int, m_sunCost, SunCost);       ///< 种植所需的阳光成本
    CC_SYNTHESIZE(int, m_health, Health);         ///< 当前生命值
    CC_SYNTHESIZE(int, m_maxHealth, MaxHealth);   ///< 最大生命值
    CC_SYNTHESIZE(int, m_row, Row);               ///< 植物所在的行（0-4）
    CC_SYNTHESIZE(int, m_col, Col);               ///< 植物所在的列（0-8）
    CC_SYNTHESIZE(int, m_attack, Attack);         ///< 攻击力
    CC_SYNTHESIZE(float, m_attackInterval, AttackInterval); ///< 攻击间隔（秒）
    CC_SYNTHESIZE(bool, m_isReady, IsReady);      ///< 是否已准备就绪（可进行攻击或生产）
    CC_SYNTHESIZE(bool, m_isDead, IsDead);        ///< 是否已死亡

    // 植物行为方法
    /**
     * @brief 植物受到伤害
     * @param damage 受到的伤害值
     */
    virtual void takeDamage(int damage);

    /**
     * @brief 植物进行攻击的抽象方法，需要子类根据具体类型实现
     */
    virtual void attack();

    /**
     * @brief 植物死亡处理
     */
    virtual void die();

    /**
     * @brief 植物种植到指定位置
     * @param row 行位置
     * @param col 列位置
     */
    virtual void planted(int row, int col);

protected:
    // 保护成员，子类可以访问和扩展这些计时器
    float m_attackCounter; ///< 攻击冷却计时器
};