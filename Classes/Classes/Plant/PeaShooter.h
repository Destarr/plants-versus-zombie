#pragma once
#include "Plant.h"
#include "Bullet/Bullet.h"

/**
 * @class Peashooter
 * @brief 豌豆射手类，继承自Plant基类
 * @remark 核心功能：每隔1秒发射一颗豌豆子弹，向左攻击僵尸
 */
class Peashooter : public Plant {
public:
    // 固定值：豌豆射手的阳光成本、攻击间隔等
    static const int SUN_COST = 100;
    static const int ATTACK_DAMAGE = 20;

    CREATE_FUNC(Peashooter);

    // 重写父类纯虚函数
    virtual bool init() override;
    virtual void update(float delta) override;
    virtual void attack() override;

    /**
     * @brief 设置子弹生成的回调（由GameScene/BulletManager实现）
     * @param callback 生成子弹的函数（返回创建的子弹）
     */
    void setCreateBulletCallback(const std::function<Bullet* (void)>& callback) {
        m_createBulletCallback = callback;
    }

    // 设置同行僵尸检测回调（核心）
    using ZombieCheckCallback = std::function<bool(int)>; // 参数：行号，返回：是否有存活僵尸
    void setZombieCheckCallback(const ZombieCheckCallback& callback) {
        m_zombieCheckCallback = callback;
    }

private:
    std::function<Bullet* (void)> m_createBulletCallback;
    ZombieCheckCallback m_zombieCheckCallback; // 僵尸检测回调
};