#ifndef __PLANT_PLACEMENT_CORE_H__
#define __PLANT_PLACEMENT_CORE_H__

#include "cocos2d.h"
#include "SunSystem.h"  // 新增：包含阳光系统头文件

USING_NS_CC;

class PlantPlacementCore : public Layer {
public:
    CREATE_FUNC(PlantPlacementCore);
    virtual bool init() override;

    // 设置阳光系统引用
    void setSunSystem(SunSystem* sunSystem) { _sunSystem = sunSystem; }

    void showCannotSelectTip(int plantIndex);
private:
    // 初始化函数
    void initPlantSlots();
    void initMapGrids();

    // 触摸事件
    void setupTouchHandlers();
    void onTouchBegan(Touch* touch);
    void onTouchMoved(Touch* touch);
    void onTouchEnded(Touch* touch);

    // 植物放置逻辑
    void selectPlant(int plantIndex);
    void cancelSelection();
    void placePlantAtPosition(const Vec2& position);

    // 冷却系统函数
    void startCooldown(int plantIndex);
    void updateCooldown(float dt);
    bool canSelectPlant(int plantIndex);
    void updatePlantSlotState(int index, bool enabled);

    void drawDebugGrids();

private:
    // 当前选中的植物信息
    int _selectedPlantIndex;    // -1表示未选中
    Sprite* _draggingPlant;     // 跟随鼠标的植物精灵

    // 植物槽位
    std::vector<Sprite*> _plantSlots;
    std::vector<Sprite*> _plantGraySlots;  // 新增：灰色卡牌
    std::vector<ProgressTimer*> _cooldownProgress;  // 新增：冷却进度条
    std::vector<std::string> _plantImages = {
        "Card/SunFlowerCard.png",
        "Card/PeaShooterCard.png",
        "Card/SnowPeaCard.png",
        "Card/CherryBombCard.png",
        "Card/TudouleiCard.png",
        "Card/WallNutCard.png"
    };

    // 植物冷却时间和阳光消耗
    std::vector<float> _cooldownDurations = { 2.5f, 2.5f, 2.5f, 10.0f, 5.0f, 10.0f };  // 冷却时间（秒）
    std::vector<float> _cooldownTimers;  // 当前冷却计时器
    std::vector<int> _sunCosts = { 50, 100, 175, 150, 25, 50 };  // 阳光消耗

    // 地图格子信息
    const int GRID_ROWS = 5;
    const int GRID_COLS = 9;
    Size _gridSize;
    Vec2 _gridStartPos;

    // 触摸监听器
    EventListenerTouchOneByOne* _touchListener;

    // 阳光系统引用
    SunSystem* _sunSystem;
};

#endif // __PLANT_PLACEMENT_CORE_H__