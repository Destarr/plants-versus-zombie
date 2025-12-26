#pragma once
#include "cocos2d.h"
USING_NS_CC;

class SunBeam : public cocos2d::Sprite {
public:
    SunBeam();
    virtual ~SunBeam();
    CREATE_FUNC(SunBeam);

    virtual bool init() override;
    // 新增：阳光的帧更新逻辑（管理存活时间、收集状态）
    virtual void update(float delta) override;
    // 新增：阳光被收集的逻辑
    void collect();
    // 新增：阳光销毁/死亡逻辑（超时/被收集后调用）
    void die();

    // 阳光的基础属性
    CC_SYNTHESIZE(int, m_value, Value);       // 阳光值（默认25）
    CC_SYNTHESIZE(bool, m_isCollected, IsCollected); // 是否已被收集
    CC_SYNTHESIZE(bool, m_isDead, IsDead);           // 是否已死亡
    CC_SYNTHESIZE(float, m_lifeTime, LifeTime);      // 存活时长（秒，超时自动消失）

    // 阳光生成后的动画（简单上升+悬浮）
    void showCollectableAnimation();

    // 新增：阳光被收集的回调（供场景接收阳光数值）
    std::function<void(int)> onCollectCallback;

private:
    // 默认存活时间（15秒未收集则自动消失）
    const float DEFAULT_LIFE_TIME = 15.0f;
    // 已存活时间（用于计时）
    float m_aliveTime;
};