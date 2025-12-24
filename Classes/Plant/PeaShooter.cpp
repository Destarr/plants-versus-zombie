#include "Peashooter.h"
#include "GameScene/GameScene.h"  // 假设游戏场景类用于获取僵尸列表
#include "Zombie/Zombie.h"     // 僵尸类

USING_NS_CC;

// 构造函数
Peashooter::Peashooter()
    : m_peaDamage(20)
    , m_peaSpeed(300.0f)
    , m_bulletType("normal") {
    // 可以在构造函数中设置豌豆射手特有的默认值
}

// 析构函数
Peashooter::~Peashooter() {
    // 清理资源
}

// 初始化方法
bool Peashooter::init() {
    // 1. 首先调用父类Plant的init方法
    if (!Plant::init()) {
        return false;
    }

    // 2. 设置豌豆射手特有的属性
    this->setSunCost(100);          // 种植需要100阳光
    this->setMaxHealth(300);        // 最大生命值300
    this->setHealth(300);           // 当前生命值300
    this->setAttack(20);            // 攻击力20
    this->setAttackInterval(1.5f);  // 每1.5秒攻击一次

    // 3. 设置豌豆射手的纹理
    this->setTexture("Plant/PeaShooter.png");
    this->setContentSize(Size(70, 70));  // 设置合适的大小

    // 4. 设置豌豆射手特有的子弹属性
    m_peaDamage = 20;
    m_peaSpeed = 300.0f;
    m_bulletType = "normal";

    // 5. 初始化攻击计数器
    m_attackCounter = 0.0f;
    m_isReady = true;  // 初始时可以攻击

    CCLOG("Peashooter initialized successfully!");
    return true;
}

// 每帧更新函数
void Peashooter::update(float delta) {
    // 1. 首先调用父类的update函数
    Plant::update(delta);

    // 2. 如果植物已经死亡，则不再进行任何更新
    if (m_isDead) {
        return;
    }

    // 3. 更新攻击冷却计时器
    if (m_attackCounter > 0) {
        m_attackCounter -= delta;
        if (m_attackCounter <= 0) {
            m_attackCounter = 0;
            m_isReady = true;  // 攻击冷却结束，可以攻击
        }
    }

    // 4. 检查是否需要攻击
    if (m_isReady) {
        this->checkForAttack();
    }
}

// 检查是否需要攻击
void Peashooter::checkForAttack() {
    // 这里需要获取游戏场景中的僵尸列表
    // 假设GameScene有一个获取某行僵尸列表的方法
    auto gameScene = dynamic_cast<GameScene*>(this->getParent()->getParent());
    if (!gameScene) {
        return;  // 无法获取游戏场景
    }

    // 获取当前行的所有僵尸
    auto zombiesInRow = gameScene->getZombiesInRow(m_row);
    if (zombiesInRow.empty()) {
        return;  // 当前行没有僵尸，不需要攻击
    }

    // 检查是否有僵尸在攻击范围内
    for (auto zombie : zombiesInRow) {
        if (!zombie->getIsDead()) {  // 只检查存活的僵尸
            // 计算植物和僵尸的距离
            float distance = zombie->getPositionX() - this->getPositionX();

            // 如果僵尸在植物前方（距离为正）且在攻击范围内
            if (distance > 0 && distance < 500.0f) {  // 500像素攻击范围
                // 找到目标，进行攻击
                this->attack();
                break;  // 一次只攻击一个目标
            }
        }
    }
}

// 攻击方法
void Peashooter::attack() {
    if (!m_isReady) {
        return;  // 攻击冷却中，不能攻击
    }

    CCLOG("Peashooter is attacking!");

    // 1. 创建豌豆子弹
    auto bullet = this->createPeaBullet();
    if (!bullet) {
        return;
    }

    // 2. 设置子弹的初始位置（在豌豆射手前方）
    auto bulletPosition = this->getPosition();
    bulletPosition.x += 20;  // 稍微靠右一点
    bullet->setPosition(bulletPosition);

    // 3. 将子弹添加到场景中
    auto parent = this->getParent();
    if (parent) {
        parent->addChild(bullet);
    }

    // 4. 播放攻击动画
    auto scaleUp = ScaleTo::create(0.1f, 1.1f);
    auto scaleDown = ScaleTo::create(0.1f, 1.0f);
    auto sequence = Sequence::create(scaleUp, scaleDown, nullptr);
    this->runAction(sequence);

    // 5. 重置攻击冷却计时器
    m_attackCounter = m_attackInterval;
    m_isReady = false;
}

// 创建豌豆子弹
Bullet* Peashooter::createPeaBullet() {
    // 这里假设Bullet类有一个create方法
    auto bullet = Bullet::create();
    if (!bullet) {
        return nullptr;
    }

    // 设置子弹属性
    bullet->setDamage(m_peaDamage);
    bullet->setSpeed(m_peaSpeed);
    bullet->setRow(m_row);  // 子弹只在当前行移动
    bullet->setType(m_bulletType);

    return bullet;
}

// 种植到指定位置
void Peashooter::planted(int row, int col) {
    // 设置行和列
    m_row = row;
    m_col = col;

    CCLOG("Peashooter planted at row %d, col %d", row, col);

    // 可以在这里添加种植时的特效
    auto fadeIn = FadeIn::create(0.5f);
    this->runAction(fadeIn);
}