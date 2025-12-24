// CollisionDetector.h
#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "cocos2d.h"

// 前向声明，避免循环依赖
class Plant;
class Zombie;
class Bullet;

USING_NS_CC;

/**
 * @brief 碰撞检测器类
 *
 * 负责检测游戏对象之间的碰撞，使用模板方法实现通用碰撞检测
 */
class CollisionDetector {
public:

    /**
     * @brief 计算两个矩形的交集
     * @param rectA 第一个矩形
     * @param rectB 第二个矩形
     * @return 相交的矩形区域，如果没有相交则返回Rect::ZERO
     */
    static Rect getIntersectionRect(const Rect& rectA, const Rect& rectB);

    /**
     * @brief 检测两个精灵对象是否发生碰撞
     * @tparam T 第一个对象的类型
     * @tparam U 第二个对象的类型
     * @param objA 第一个对象
     * @param objB 第二个对象
     * @return 如果发生碰撞返回true，否则返回false
     */
    template<typename T, typename U>
    static bool checkCollision(T* objA, U* objB) {
        // 安全检查
        if (!objA || !objB) {
            return false;
        }

        // 获取对象的边界框
        Rect rectA = objA->getBoundingBox();
        Rect rectB = objB->getBoundingBox();

        // 使用cocos2d-x的矩形相交检测
        return rectA.intersectsRect(rectB);
    }

    /**
     * @brief 检测植物与僵尸的碰撞（专用方法）
     * @param plant 植物对象
     * @param zombie 僵尸对象
     * @return 如果发生碰撞返回true，否则返回false
     */
    static bool checkPlantZombieCollision(Plant* plant, Zombie* zombie);

    /**
     * @brief 检测子弹与僵尸的碰撞（专用方法）
     * @param bullet 子弹对象
     * @param zombie 僵尸对象
     * @return 如果发生碰撞返回true，否则返回false
     */
    static bool checkBulletZombieCollision(Bullet* bullet, Zombie* zombie);

    /**
     * @brief 检测点是否在矩形内
     * @param point 要检测的点
     * @param rect 矩形区域
     * @return 如果点在矩形内返回true，否则返回false
     */
    static bool isPointInRect(const Vec2& point, const Rect& rect) {
        return rect.containsPoint(point);
    }

    /**
     * @brief 检测两个矩形是否相交
     * @param rectA 第一个矩形
     * @param rectB 第二个矩形
     * @return 如果相交返回true，否则返回false
     */
    static bool isRectIntersect(const Rect& rectA, const Rect& rectB) {
        return rectA.intersectsRect(rectB);
    }


    /**
     * @brief 检测两个圆形是否相交
     * @param centerA 第一个圆的中心
     * @param radiusA 第一个圆的半径
     * @param centerB 第二个圆的中心
     * @param radiusB 第二个圆的半径
     * @return 如果相交返回true，否则返回false
     */
    static bool isCircleIntersect(const Vec2& centerA, float radiusA,
        const Vec2& centerB, float radiusB) {
        float distance = centerA.distance(centerB);
        return distance <= (radiusA + radiusB);
    }

private:
    /**
     * @brief 私有构造函数，防止实例化
     */
    CollisionDetector() = delete;

    /**
     * @brief 私有析构函数
     */
    ~CollisionDetector() = delete;

    /**
     * @brief 禁止复制和赋值
     */
    CollisionDetector(const CollisionDetector&) = delete;
    CollisionDetector& operator=(const CollisionDetector&) = delete;
};

#endif // COLLISIONDETECTOR_H