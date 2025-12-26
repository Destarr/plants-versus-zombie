#pragma once
#include "Plant.h"
#include "Zombie/Zombie.h"
#include <vector>

// 前向声明
class Zombie;

class PotatoMine : public Plant {
public:
    // 静态创建方法
    CREATE_FUNC(PotatoMine);

    // 重写父类方法
    virtual bool init() override;
    virtual void update(float delta) override;
    virtual void attack() override; // 爆炸攻击逻辑
    virtual void die() override;    // 爆炸后销毁

    // 新增：设置僵尸列表（用于爆炸范围检测）
    void setZombiesList(const std::vector<Zombie*>* zombies) {
        m_zombiesList = zombies;
    }

    // 新增：僵尸碰撞触发爆炸
    void onZombieCollision(Zombie* zombie);

private:
    // 土豆地雷专属属性
    bool m_isEmerged;          // 是否已出土（true=出土，可爆炸；false=冷却中）
    int m_explosionDamage;     // 爆炸伤害（默认秒杀普通僵尸）
    float m_explosionRadius;   // 爆炸范围半径（像素）
    const std::vector<Zombie*>* m_zombiesList; // 场景僵尸列表（弱引用）

    // 切换出土/未出土外观
    void updateAppearance();
};