#include "PlantPlacementCore.h"
#include "ui/CocosGUI.h"

#include"GameDataCenter.h"

#include "Plant/SunFlower.h"
#include "Plant/PeaShooter.h"
#include "Plant/SnowPeaShooter.h"
#include "Plant/CherryBomb.h"
#include "Plant/PotatoMine.h"
#include "Plant/WallNut.h"

USING_NS_CC;

bool PlantPlacementCore::init() {
    if (!Layer::init()) {
        return false;
    }

    // 初始化状态
    _selectedPlantIndex = -1;  // 未选中任何植物
    _draggingPlant = nullptr;

    // 获取屏幕信息
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 设置格子大小和起始位置
    _gridSize = Size(80, 100);

    // **修正：第一个格子（0行0列）的左下角位置**
    // 从左侧开始，离左边缘50像素，从底部开始，离底部20像素
    _gridStartPos = Vec2(origin.x + 50, origin.y + 20);

    // 初始化植物槽位（移到左上角）
    initPlantSlots();

    // 绘制调试网格（可选，帮助理解坐标）
    drawDebugGrids();

    // 设置触摸事件
    setupTouchHandlers();

    return true;
}

// 初始化植物槽位（左上角）
void PlantPlacementCore::initPlantSlots() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // **修改：将植物槽位移到左上角（垂直排列）**
    float startX = origin.x + 150;  // 离左边缘150像素
    float startY = origin.y + visibleSize.height - 5;  // 离顶部5像素

    for (int i = 0; i < 6; i++) {

        // 植物图标
        auto plantIcon = Sprite::create(_plantImages[i]);
        if (plantIcon) {
            plantIcon->setAnchorPoint(Vec2(0, 1));
            plantIcon->setPosition(Vec2(startX + i * 60, startY));
            plantIcon->setTag(i);  // 用tag标识植物类型
            this->addChild(plantIcon);
            _plantSlots.push_back(plantIcon);
        }
    }
}

// 绘制调试网格（帮助理解布局）
void PlantPlacementCore::drawDebugGrids() {
    auto drawNode = DrawNode::create();

    // 绘制所有格子
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            // 计算格子的左下角位置
            float left = _gridStartPos.x + col * _gridSize.width;
            float bottom = _gridStartPos.y + row * _gridSize.height;  // Y轴向上增加

            // 计算格子的四个角
            Vec2 bottomLeft(left, bottom);
            Vec2 bottomRight(left + _gridSize.width, bottom);
            Vec2 topRight(left + _gridSize.width, bottom + _gridSize.height);
            Vec2 topLeft(left, bottom + _gridSize.height);

            // 绘制格子边框
            drawNode->drawRect(bottomLeft, topRight, Color4F(0, 1, 0, 0.3f));

            // 在格子中心显示行列号（调试用）
            Vec2 center(left + _gridSize.width / 2, bottom + _gridSize.height / 2);
            auto label = Label::createWithTTF(
                StringUtils::format("(%d,%d)", row, col),
                "fonts/Marker Felt.ttf",
                10
            );
            label->setPosition(center);
            label->setColor(Color3B::WHITE);
            this->addChild(label, 1);
        }
    }

    // 标记起始点
    auto startMarker = DrawNode::create();
    startMarker->drawSolidCircle(_gridStartPos, 5, 0, 8, Color4F(1, 0, 0, 1));

    // 添加起始点标签
    auto startLabel = Label::createWithTTF("起始点(0,0)", "fonts/Marker Felt.ttf", 12);
    startLabel->setPosition(_gridStartPos + Vec2(0, -20));
    startLabel->setColor(Color3B::RED);

    this->addChild(drawNode, 0);
    this->addChild(startMarker, 1);
    this->addChild(startLabel, 1);
}

// 设置触摸事件（保持不变）
void PlantPlacementCore::setupTouchHandlers() {
    _touchListener = EventListenerTouchOneByOne::create();

    _touchListener->onTouchBegan = [this](Touch* touch, Event* event) -> bool {
        onTouchBegan(touch);
        return true;
        };

    _touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        onTouchMoved(touch);
        };

    _touchListener->onTouchEnded = [this](Touch* touch, Event* event) {
        onTouchEnded(touch);
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
}

// 触摸开始（关键修改：修正坐标计算）
void PlantPlacementCore::onTouchBegan(Touch* touch) {
    Vec2 touchPos = touch->getLocation();

    // 情况1：如果正在拖拽植物，则尝试放置
    if (_selectedPlantIndex != -1 && _draggingPlant) {
        placePlantAtPosition(touchPos);
        return;
    }

    // 情况2：检查是否点击了植物槽位
    for (size_t i = 0; i < _plantSlots.size(); i++) {
        auto slot = _plantSlots[i];
        if (slot && slot->getBoundingBox().containsPoint(touchPos)) {
            selectPlant((int)i);
            return;
        }
    }
}

// 触摸移动（保持不变）
void PlantPlacementCore::onTouchMoved(Touch* touch) {
    if (_selectedPlantIndex != -1 && _draggingPlant) {
        _draggingPlant->setPosition(touch->getLocation());
    }
}

// 触摸结束（保持不变）
void PlantPlacementCore::onTouchEnded(Touch* touch) {
    // 可以添加一些触摸结束的逻辑
}

// 选择植物（保持不变）
void PlantPlacementCore::selectPlant(int plantIndex) {
    if (_selectedPlantIndex != -1) {
        cancelSelection();
    }
    //创建种植提示
    auto label = Label::createWithTTF(
        StringUtils::format("Please place it at map!"),
        "fonts/Marker Felt.ttf",
        90
    );
    label->setColor(Color3B::GREEN);
    label->setPosition(Vec2(400, 100));
    this->addChild(label, 100);

    // 淡出并移除提示
    label->runAction(Sequence::create(
        MoveBy::create(1.0f, Vec2(0, 30)),
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr));


    _selectedPlantIndex = plantIndex;

    if (_draggingPlant) {
        _draggingPlant->removeFromParent();
        _draggingPlant = nullptr;
    }

    _draggingPlant = Sprite::create(_plantImages[plantIndex]);
    _draggingPlant->setOpacity(180);
    this->addChild(_draggingPlant, 10);
    
    auto touchPos = Director::getInstance()->getVisibleOrigin();
    _draggingPlant->setAnchorPoint(Vec2(0, 0));
    _draggingPlant->setPosition(touchPos);
}

// 取消选择（保持不变）
void PlantPlacementCore::cancelSelection() {
    _selectedPlantIndex = -1;

    if (_draggingPlant) {
        _draggingPlant->removeFromParent();
        _draggingPlant = nullptr;
    }
}

// 放置植物到指定位置（关键修改：修正坐标计算）
void PlantPlacementCore::placePlantAtPosition(const Vec2& position) {
    if (_selectedPlantIndex == -1 || !_draggingPlant) {
        return;
    }

    // **修正：计算点击位置对应的行列**
    // 相对于起始点的偏移
    float offsetX = position.x - _gridStartPos.x;
    float offsetY = position.y - _gridStartPos.y;  // Y轴向上增加

    // 计算行列号
    int col = static_cast<int>(offsetX / _gridSize.width);
    int row = static_cast<int>(offsetY / _gridSize.height);

    // 检查是否在有效范围内
    if (col < 0 || col >= GRID_COLS || row < 0 || row >= GRID_ROWS) {
        auto label = Label::createWithTTF(
            StringUtils::format("You can't placed here!"),
            "fonts/Marker Felt.ttf",
            90
        );
        label->setColor(Color3B::RED);
        label->setPosition(Vec2(400, 100));
        this->addChild(label, 100);

        // 淡出并移除提示
        label->runAction(Sequence::create(
            MoveBy::create(1.0f, Vec2(0, 30)),
            FadeOut::create(0.5f),
            RemoveSelf::create(),
            nullptr));
        cancelSelection();
        return;
    }

    // **修正：计算格子中心位置**
    // 左下角 + 半个格子宽度 = 中心X
    // 左下角 + 半个格子高度 = 中心Y
    float gridCenterX = _gridStartPos.x + col * _gridSize.width + _gridSize.width / 2;
    float gridCenterY = _gridStartPos.y + row * _gridSize.height + _gridSize.height / 2;


    // 创建实际的植物 先能跑就行 优不优雅咱们另说
    if (_selectedPlantIndex == 0)
    {
        auto plant = SunFlower::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
    }
    if (_selectedPlantIndex == 1)
    {
        auto plant = Peashooter::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        // 添加到共享数据中心
        GameDataCenter::getInstance()->addPlant(plant);

        // 为攻击型植物设置回调（使用队友的接口）
        if (auto shooter = dynamic_cast<Peashooter*>(plant)) {
            // 设置子弹创建回调
            shooter->setCreateBulletCallback([]() -> Bullet* {
                auto bullet = Bullet::create();
                if (bullet) {
                    GameDataCenter::getInstance()->addBullet(bullet);
                }
                return bullet;
                });

            // 设置僵尸检测回调
            shooter->setZombieCheckCallback([](int row) -> bool {
                auto& zombies = GameDataCenter::getInstance()->getZombies();
                for (auto zombie : zombies) {
                    if (zombie && zombie->isAlive() && zombie->getRow() == row) {
                        return true;
                    }
                }
                return false;
                });
        }
    }
    if (_selectedPlantIndex == 2)
    {
        auto plant = SnowPeaShooter::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        // 添加到共享数据中心
        GameDataCenter::getInstance()->addPlant(plant);

        // 为攻击型植物设置回调（使用队友的接口）
        if (auto shooter = dynamic_cast<SnowPeaShooter*>(plant)) {
            // 设置子弹创建回调
            shooter->setCreateBulletCallback([]() -> Bullet* {
                auto bullet = SnowPea::create();
                if (bullet) {
                    GameDataCenter::getInstance()->addBullet(bullet);
                }
                return bullet;
                });

            // 设置僵尸检测回调（核心）
            plant->setZombieCheckCallback([this](int row) -> bool {
                // 遍历场景中的僵尸列表，检查指定行是否有存活僵尸
                auto& zombies = GameDataCenter::getInstance()->getZombies();
                for (auto zombie : zombies) {
                    if (zombie && zombie->isAlive() && !zombie->getIsDead() && zombie->getRow() == row) {
                        return true; // 找到存活僵尸
                    }
                }
                return false; // 无存活僵尸
                });
        }
    }
    if (_selectedPlantIndex == 3)
    {
        auto plant = CherryBomb::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
    }
    if (_selectedPlantIndex == 4)
    {
        auto plant = PotatoMine::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
    }
    if (_selectedPlantIndex == 5)
    {
        auto plant = WallNut::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
    }


    // 取消选择状态
    cancelSelection();

    auto label = Label::createWithTTF(
        StringUtils::format("Put at (%d,%d)", row, col),
        "fonts/Marker Felt.ttf",
        90
    );
    label->setColor(Color3B::GREEN);
    label->setPosition(Vec2(400,100));
    this->addChild(label, 100);

    // 淡出并移除提示
    label->runAction(Sequence::create(
        MoveBy::create(1.0f, Vec2(0, 30)),
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr
    ));
}
