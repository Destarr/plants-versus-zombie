#pragma once
//该文件为游戏场景的头文件 ——2451401 吕光晔
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Zombie/Zombie.h"
#include"Plant/Plant.h"
#include"Bullet/Bullet.h"
#include <vector>

class Game : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    virtual void update(float delta) override;

    // a selector callback
    void plantSelectedCallback(cocos2d::Ref* pSender);

    //创建僵尸的方法
    void spawnZombie(int row, float startX, float startY);

    // 僵尸到达房子的回调处理
    void onZombieReachHouse(Zombie* zombie);

    // 添加子弹到管理列表
    void addBullet(Bullet* bullet);

    //添加植物到管理列表
    void addPlant(Plant* plant);

    // implement the "static create()" method manually
    CREATE_FUNC(Game);

    // 维护活跃的僵尸和子弹列表
    std::vector<Zombie*> m_zombies;
    std::vector<Bullet*> m_bullets;
    std::vector<Plant*> m_plants;


    // 清理死亡的僵尸/子弹（避免内存泄漏）
    void cleanupDeadObjects();
};

#endif // __GAME_SCENE_H__
