#ifndef __SUN_SYSTEM_H__
#define __SUN_SYSTEM_H__

#include "cocos2d.h"
#include <vector>

USING_NS_CC;

class SunSystem : public Layer {
public:
    CREATE_FUNC(SunSystem);
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void onExit() override;

    // 阳光管理
    void addSun(int amount);
    bool consumeSun(int amount);
    int getSunCount() const { return _sunCount; }

    // 生成掉落阳光
    void generateFallingSun();
    void generateFallingSunAtPosition(const Vec2& position, int sunValue = 25);

private:
    // 初始化UI
    void initUI();

    // 触摸事件
    void setupTouchHandlers();

    // 阳光动画
    void onSunCollected(Sprite* sun);
    void sunFallingAnimation(Sprite* sun);

    // 安全移除阳光
    void safeRemoveSun(Sprite* sun);

    void SunSystem::updateSunLabel();

    void SunSystem::update(float dt);

private:
    int _sunCount;  // 当前阳光总数
    Label* _sunLabel;  // 显示阳光数量的标签

    // 阳光生成控制
    float _sunGenerateInterval;
    float _timeSinceLastSun;

    // 触摸监听器
    EventListenerTouchOneByOne* _touchListener;

    // 存储所有阳光精灵 - 使用std::list可能更安全，但用vector也可以
    std::vector<Sprite*> _suns;

    // 阳光值
    const int DEFAULT_SUN_VALUE = 25;

    // 线程安全标志
    bool _isCollectingSun;
};

#endif // __SUN_SYSTEM_H__