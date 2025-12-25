// CollisionDetector.h
#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "cocos2d.h"
#include "Bullet/Bullet.h"
#include "Zombie/Zombie.h"
#include "Plant/Plant.h"

class Plant;
class Zombie;
class Bullet;

class CollisionDetector {
public:
    // 简单碰撞检测：使用cocos2d-x自带的intersectsRect
    template<typename T, typename U>
    static bool checkCollision(T* objA, U* objB) {
        if (!objA || !objB) return false;
        return objA->getBoundingBox().intersectsRect(objB->getBoundingBox());
    }
    /**
 * @brief 批量检测子弹与僵尸的碰撞（按行优化）
 * @param bullets 待检测的子弹列表
 * @param zombies 待检测的僵尸列表
 * @remark 仅检测同行的子弹和僵尸，提升性能
 */
    static void detectBulletZombieCollisions(
        const std::vector<Bullet*>& bullets,
        const std::vector<Zombie*>& zombies
    );
    /**
 * @brief 批量检测僵尸与植物的碰撞（每帧扣血，无攻击间隔）
 * @param zombies 待检测的僵尸列表
 * @param plants 待检测的植物列表
 */
    static void detectZombiePlantCollisions(
        const std::vector<Zombie*>& zombies,
        const std::vector<Plant*>& plants
    );

    //检测单个子弹与所有同行僵尸的碰撞
    static void checkSingleBulletCollisions(Bullet* bullet, const std::vector<Zombie*>& zombies);
    static void checkSingleZombieCollisions(Zombie* zombie, const std::vector<Plant*>& plants);
};

#endif // COLLISIONDETECTOR_H