// CollisionDetector.cpp
#include "CollisionDetector.h"
#include "Plant/Plant.h"
#include "Zombie/Zombie.h"
#include "Bullet/Bullet.h"
#include <algorithm>  // 包含min/max函数

// 由于模板函数需要在头文件中定义，这里只实现非模板的专用方法

bool CollisionDetector::checkPlantZombieCollision(Plant* plant, Zombie* zombie) {
    // 安全检查
    if (!plant || !zombie || plant->getIsDead() || zombie->getIsDead()) {
        return false;
    }

    // 获取对象的边界框
    Rect plantRect = plant->getBoundingBox();
    Rect zombieRect = zombie->getBoundingBox();

    // 使用矩形相交检测
    bool isColliding = plantRect.intersectsRect(zombieRect);

    // 可选的：添加调试日志
    if (isColliding) {
        CCLOG("Collision detected between plant at row %d and zombie at row %d",
            plant->getRow(), zombie->getRow());
    }

    return isColliding;
}

bool CollisionDetector::checkBulletZombieCollision(Bullet* bullet, Zombie* zombie) {
    // 安全检查
    if (!bullet || !zombie || bullet->getIsDead() || zombie->getIsDead() || !bullet->getIsActive()) {
        return false;
    }

    // 获取对象的边界框
    Rect bulletRect = bullet->getBoundingBox();
    Rect zombieRect = zombie->getBoundingBox();

    // 使用矩形相交检测
    bool isColliding = bulletRect.intersectsRect(zombieRect);

    // 可选的：添加调试日志
    if (isColliding) {
        CCLOG("Bullet hit zombie! Zombie health: %d, Bullet damage: %d",
            zombie->getHealth(), bullet->getDamage());
    }

    return isColliding;
}

Rect CollisionDetector::getIntersectionRect(const Rect& rectA, const Rect& rectB) {
    // 计算两个矩形在x轴上的重叠部分
    float x1 = std::max(rectA.getMinX(), rectB.getMinX());
    float x2 = std::min(rectA.getMaxX(), rectB.getMaxX());

    // 计算两个矩形在y轴上的重叠部分
    float y1 = std::max(rectA.getMinY(), rectB.getMinY());
    float y2 = std::min(rectA.getMaxY(), rectB.getMaxY());

    // 如果没有重叠，返回空矩形
    if (x1 >= x2 || y1 >= y2) {
        return Rect::ZERO;
    }

    // 返回重叠的矩形
    return Rect(x1, y1, x2 - x1, y2 - y1);
}