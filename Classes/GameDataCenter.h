#ifndef __GAME_DATA_CENTER_H__
#define __GAME_DATA_CENTER_H__

#include <vector>
#include <algorithm>
#include "cocos2d.h"
#include "Zombie/Zombie.h"
#include "Plant/Plant.h"
#include "Bullet/Bullet.h"
#include "Manager/CollisionDetector.h"

USING_NS_CC;

class GameDataCenter {
public:
    // 单例模式
    static GameDataCenter* getInstance();

    // 销毁单例（重要！用于完全重置）
    static void destroyInstance();

    // 禁止拷贝和赋值
    GameDataCenter(const GameDataCenter&) = delete;
    GameDataCenter& operator=(const GameDataCenter&) = delete;

    // 获取列表
    std::vector<Zombie*>& getZombies();
    std::vector<Plant*>& getPlants();
    std::vector<Bullet*>& getBullets();

    // 添加元素
    void addZombie(Zombie* zombie);
    void addPlant(Plant* plant);
    void addBullet(Bullet* bullet);

    // 移除元素
    void removeZombie(Zombie* zombie);
    void removePlant(Plant* plant);
    void removeBullet(Bullet* bullet);

    // 清理死亡对象并释放内存
    void cleanupDeadObjects();

    // 完全重置游戏数据（释放所有内存）
    void reset();

    // 添加死亡回调
    using ZombieDeathCallback = std::function<void(Zombie*)>;
    void setZombieDeathCallback(const ZombieDeathCallback& callback) {
        m_zombieDeathCallback = callback;
    }
private:

    GameDataCenter() = default;
    ~GameDataCenter();

    // 内部清理函数
    void clearAll();

    static GameDataCenter* s_instance;
    std::vector<Zombie*> m_zombies;
    std::vector<Plant*> m_plants;
    std::vector<Bullet*> m_bullets;

    ZombieDeathCallback m_zombieDeathCallback; // 僵尸死亡回调
};

#endif // __GAME_DATA_CENTER_H__