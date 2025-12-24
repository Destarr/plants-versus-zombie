#include "Bullet.h"

// 初始化方法
bool Bullet::init() {
    // 调用父类Sprite的init方法
    if (!Sprite::init()) {
        return false;
    }

    // 初始化子弹的默认属性
    m_damage = 1;           // 默认伤害值
    m_speed = 300.0f;       // 默认移动速度（像素/秒）
    m_row = 0;              // 默认所在行
    m_direction = Vec2(-1, 0); // 默认方向向左（用于攻击僵尸）
    m_type = "pea";         // 默认子弹类型为豌豆
    m_isActive = false;     // 初始状态为非激活
    m_isDead = false;       // 初始状态为未死亡

    // 可以在此处设置子弹的初始纹理，例如：
    // this->setTexture("pea.png");

    // 初始时子弹不可见，直到被激活
    this->setVisible(false);

    // 开启每帧更新（即使未激活也更新，但会在update中检查状态）
    this->scheduleUpdate();

    return true;
}

// 每帧更新函数
void Bullet::update(float delta) {
    // 如果子弹已经死亡或未激活，则不进行任何逻辑更新
    if (m_isDead || !m_isActive) {
        return;
    }

    // 根据方向向量和速度移动子弹
    Vec2 moveDistance = m_direction * m_speed * delta;
    this->setPosition(this->getPosition() + moveDistance);

    // 获取当前子弹位置和屏幕尺寸
    Vec2 currentPos = this->getPosition();
    Size visibleSize = Director::getInstance()->getVisibleSize();

    // 简单边界检查：如果子弹完全飞出屏幕左边界或右边界，则让其死亡（回收）
    // 注意：根据游戏设计，可能只需要检查左边界，因为僵尸从右向左移动
    if (currentPos.x < -this->getContentSize().width || currentPos.x > visibleSize.width + this->getContentSize().width) {
        this->die();
    }

    // 在实际项目中，此处还应添加与僵尸的碰撞检测逻辑
    // 例如：检查是否与任何僵尸的边界框相交，如果相交，则对僵尸造成伤害并让子弹死亡
}

// 激活子弹
void Bullet::activate(const Vec2& startPosition, int row) {
    m_isActive = true;
    m_isDead = false;
    m_row = row; // 设置子弹所在的行，可用于优化碰撞检测（只检测同行的僵尸）

    // 设置子弹的初始位置和可见性
    this->setPosition(startPosition);
    this->setVisible(true);

    // 可以在此处重置子弹的其他状态或启动动画
}

// 子弹死亡/回收
void Bullet::die() {
    // 标记子弹为死亡状态
    m_isDead = true;
    m_isActive = false;

    // 停止所有可能存在的动作
    this->stopAllActions();
    // 设置子弹不可见
    this->setVisible(false);

    // 注意：这里并没有立即将子弹从父节点移除 (this->removeFromParent())
    // 这是为了便于实现对象池（Object Pool）模式。
    // 当需要真正移除时，可以在游戏逻辑中调用 this->removeFromParent()
    // 或者由管理类（如BulletManager）统一处理死亡子弹的回收和重用。

    CCLOG("Bullet died and is ready for recycling.");
}