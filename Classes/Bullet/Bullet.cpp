#include "Bullet.h"
#include "GameScene/GameScene.h" // 假设有GameScene用于碰撞检测
#include "Zombie/Zombie.h"    // 僵尸类

USING_NS_CC;

// 构造函数
Bullet::Bullet()
    : m_damage(20)      // 默认伤害值
    , m_speed(300.0f)  // 默认速度
    , m_row(-1)        // 未设置行
    , m_direction(Vec2(1, 0)) // 默认向右移动
    , m_type("normal")  // 默认类型
    , m_isActive(false) // 初始未激活
{
}

// 初始化方法
bool Bullet::init() {
    if (!Sprite::init()) {
        return false;
    }

    // 设置子弹纹理（这里使用一个简单的圆形作为占位符）
    auto drawer = DrawNode::create();
    drawer->drawDot(Vec2(0, 0), 5, Color4F(1.0f, 1.0f, 0.0f, 1.0f)); // 黄色圆点
    this->addChild(drawer);

    // 或者使用图片纹理（取消注释下面的代码，使用图片）
    // this->setTexture("bullets/pea.png");
    // this->setContentSize(Size(10, 10));

    // 初始设置为不可见，直到被激活
    this->setVisible(false);

    // 启动update调度
    this->scheduleUpdate();

    return true;
}

// 每帧更新
void Bullet::update(float delta) {
    if (!m_isActive) {
        return; // 如果子弹未激活，不进行任何处理
    }

    // 移动子弹
    this->moveBullet(delta);

    // 检查碰撞
    this->checkCollision();

    // 检查子弹是否飞出屏幕（简单判断x坐标）
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (this->getPositionX() > visibleSize.width + 50) {
        this->setVisible(false);
        m_isActive = false;
        this->removeFromParent(); // 或者可以放入对象池重用
    }
}

// 激活子弹
void Bullet::activate(const Vec2& startPosition, int row) {
    this->setPosition(startPosition);
    this->setVisible(true);
    m_row = row;
    m_isActive = true;

    // 根据类型设置不同的外观和属性
    if (m_type == "ice") {
        // 寒冰豌豆：蓝色，有减速效果
        this->setColor(Color3B(100, 100, 255));
    }
    else if (m_type == "fire") {
        // 火焰豌豆：红色，有持续伤害
        this->setColor(Color3B(255, 100, 100));
    }

    CCLOG("Bullet activated at position (%.1f, %.1f), row: %d",
        startPosition.x, startPosition.y, row);
}

// 子弹移动逻辑
void Bullet::moveBullet(float delta) {
    if (!m_isActive) return;

    // 根据方向和速度移动子弹
    Vec2 movement = m_direction * m_speed * delta;
    this->setPosition(this->getPosition() + movement);
}

// 碰撞检测
void Bullet::checkCollision() {
    if (!m_isActive) return;

    // 获取游戏场景（假设子弹被添加到GameScene中）
    auto gameScene = dynamic_cast<GameScene*>(this->getParent());
    if (!gameScene) return;

    // 获取当前行的所有僵尸
    auto zombiesInRow = gameScene->getZombiesInRow(m_row);

    // 检查子弹是否与任何僵尸碰撞
    for (auto zombie : zombiesInRow) {
        if (zombie && !zombie->getIsDead()) {
            // 使用矩形碰撞检测
            if (this->getBoundingBox().intersectsRect(zombie->getBoundingBox())) {
                // 碰撞发生！
                CCLOG("Bullet hit zombie!");

                // 对僵尸造成伤害
                zombie->takeDamage(m_damage);

                // 特殊效果处理
                if (m_type == "ice") {
                    // 寒冰效果：减速
                    zombie->applySlowEffect(0.5f, 3.0f); // 减速50%，持续3秒
                }
                else if (m_type == "fire") {
                    // 火焰效果：持续伤害
                    zombie->applyBurnEffect(m_damage / 2, 3.0f); // 每秒造成一半伤害，持续3秒
                }

                // 子弹碰撞后消失
                this->onCollision();
                return; // 一次只击中一个目标
            }
        }
    }
}

// 子弹碰撞后的处理（可被子类重写）
void Bullet::onCollision() {
    // 播放碰撞特效（例如：小爆炸动画）
    auto fadeOut = FadeOut::create(0.1f);
    auto remove = CallFunc::create([this]() {
        this->setVisible(false);
        m_isActive = false;
        this->removeFromParent();
        });
    auto sequence = Sequence::create(fadeOut, remove, nullptr);
    this->runAction(sequence);

    // 可以在这里添加音效
    // CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("sounds/pea_hit.wav");
}