#pragma once
#include "Plant.h"
#include "Zombie/Zombie.h"
#include <vector>

// 前向声明
class Zombie;

class CherryBomb : public Plant {
public:
    // 静态创建方法
    CREATE_FUNC(CherryBomb);

    // 重写父类方法
    virtual bool init() override;
    virtual void update(float delta) override;
    virtual void attack() override; // 爆炸攻击逻辑
    virtual void die() override;    // 爆炸后销毁

    // 设置场景僵尸列表（用于全场景范围检测）
    void setZombiesList(const std::vector<Zombie*>* zombies) {
        m_zombiesList = zombies;
    }

private:
    // 樱桃炸弹专属属性
    int m_explosionDamage;     // 爆炸伤害（秒杀所有僵尸）
    float m_explosionRadius;   // 爆炸范围半径（像素，比土豆地雷大）
    const std::vector<Zombie*>* m_zombiesList; // 场景僵尸列表（弱引用）

    // 切换前摇/爆炸外观
    void updateAppearance();
};