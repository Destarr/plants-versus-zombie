// Zombie.h
#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "cocos2d.h"

USING_NS_CC;

// 前向声明GameScene，避免循环依赖
class GameScene;

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

    /**
     * @brief 设置场景引用，用于边界检测
     * @param scene 游戏场景指针
     */
    void setGameScene(GameScene* scene) { m_gameScene = scene; }

    // 使用CC_SYNTHESIZE宏定义属性（自动生成getter和setter）
    CC_SYNTHESIZE(int, m_health, Health);          ///< 当前生命值
    CC_SYNTHESIZE(int, m_maxHealth, MaxHealth);    ///< 最大生命值
    CC_SYNTHESIZE(float, m_speed, Speed);          ///< 移动速度（像素/秒）
    CC_SYNTHESIZE(int, m_damage, Damage);          ///< 攻击力
    CC_SYNTHESIZE(int, m_row, Row);                ///< 僵尸所在的行（0-4）
    CC_SYNTHESIZE(bool, m_isDead, IsDead);         ///< 是否已死亡
    CC_SYNTHESIZE(bool, m_hasReachedHouse, HasReachedHouse); ///< 是否已抵达房子

private:
    GameScene* m_gameScene;  ///< 指向游戏场景的指针，用于边界检测
};

#endif // ZOMBIE_H

