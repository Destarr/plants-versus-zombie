#include "SunFlower.h"


SunFlower::SunFlower() : Plant() {}

SunFlower::~SunFlower() {}

bool SunFlower::init() {
    if (!Plant::init()) {
        return false;
    }

    // 初始化向日葵专属属性
    this->setSunCost(SUN_FLOWER_COST);
    this->setMaxHealth(SUN_FLOWER_HEALTH);
    this->setHealth(SUN_FLOWER_HEALTH);
    this->setAttackInterval(SUN_PRODUCE_INTERVAL); // 复用attackInterval作为产阳光间隔
    this->m_attackCounter = this->getAttackInterval();
    this->setIsReady(false); // 初始状态设为未就绪

    // 替换向日葵的可视化占位符
    this->removeAllChildren(); // 清除父类的绿色占位符
    this->setTexture("Plant/SunFlower.png");

    CCLOG("SunFlower initialized, sun cost: %d, produce interval: %.1fs",
        this->getSunCost(), this->getAttackInterval());

    return true;
}

void SunFlower::update(float delta) {


    // 先调用父类的update，复用攻击计数器逻辑
    Plant::update(delta);

    if (m_isDead) return;

    // 如果计时完成（可生产阳光），执行生产逻辑
    if (m_isReady && !m_isDead) {
        this->attack();
    }
}

void SunFlower::attack() {
    // 重写attack方法，改为生产阳光
    this->produceSun();
    CCLOG("SunFlower produced sun at row %d, col %d, value: %d",
        m_row, m_col, SUN_PRODUCE_VALUE);
}

void SunFlower::produceSun() {
    // 创建阳光对象
    auto sun = SunBeam::create();
    if (!sun) {
        CCLOG("===== ERROR: SunBeam create failed! =====");
        return;
    }
    CCLOG("===== SunBeam create success! =====");

    // 设置阳光属性
    sun->setValue(SUN_PRODUCE_VALUE);
    sun->setPosition(this->getPosition() + Vec2(0, 30)); // 阳光生成在向日葵上方

    // 添加到当前场景（需确保向日葵已添加到场景中）
    this->getParent()->addChild(sun, 20); // 层级设高，避免被遮挡

    // 播放阳光动画
    sun->showCollectableAnimation();
}