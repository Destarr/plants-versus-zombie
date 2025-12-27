#include "SunFlower.h"
#include "GameDataCenter.h"
#include "SunSystem.h"

USING_NS_CC;

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
    this->setAttackInterval(SUN_PRODUCE_INTERVAL);
    this->m_attackCounter = this->getAttackInterval();
    this->setIsReady(false);

    // 替换向日葵的可视化占位符
    this->removeAllChildren();
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
        // 重置状态
        m_isReady = false;
        m_attackCounter = this->getAttackInterval();

        // 生产阳光
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
    // 获取阳光系统
    auto sunSystem = GameDataCenter::getInstance()->getSunSystem();

    if (!sunSystem) {
        CCLOG("SunFlower: SunSystem not found!");
        return;
    }

    CCLOG("SunFlower producing sun");

    // 计算阳光生成位置（在向日葵位置上方）
    Vec2 sunflowerPos = this->getPosition();
    Vec2 sunPos = sunflowerPos + Vec2(0, 50);

    // 转换为世界坐标
    if (this->getParent()) {
        sunPos = this->getParent()->convertToWorldSpace(sunPos);
    }

    // 通过阳光系统生成可收集的阳光
    sunSystem->generateFallingSunAtPosition(sunPos, SUN_PRODUCE_VALUE);

    // 播放向日葵产阳光的动画
    auto scaleUp = ScaleTo::create(0.1f, 1.2f);
    auto scaleDown = ScaleTo::create(0.1f, 1.0f);
    this->runAction(Sequence::create(scaleUp, scaleDown, nullptr));
}