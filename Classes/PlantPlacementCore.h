
#ifndef __PLANT_PLACEMENT_CORE_H__
#define __PLANT_PLACEMENT_CORE_H__

#include "cocos2d.h"

USING_NS_CC;

class PlantPlacementCore : public Layer {
public:
    CREATE_FUNC(PlantPlacementCore);
    virtual bool init() override;

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

    void drawDebugGrids();

private:
    // 当前选中的植物信息
    int _selectedPlantIndex;    // -1表示未选中
    Sprite* _draggingPlant;     // 跟随鼠标的植物精灵

    // 植物槽位
    std::vector<Sprite*> _plantSlots;
    std::vector<std::string> _plantImages = {
        "Card/SunFlowerCard.png",
        "Card/PeaShooterCard.png",
        "Card/SnowPeaCard.png",
        "Card/CherryBombCard.png",
        "Card/TudouleiCard.png",
        "Card/WallNutCard.png"
    };

    // 地图格子信息
    const int GRID_ROWS = 5;
    const int GRID_COLS = 9;
    Size _gridSize;
    Vec2 _gridStartPos;

    // 触摸监听器
    EventListenerTouchOneByOne* _touchListener;
};

#endif // __PLANT_PLACEMENT_CORE_H__