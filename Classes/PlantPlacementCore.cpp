#include "PlantPlacementCore.h"
#include "ui/CocosGUI.h"
#include "GameDataCenter.h"
#include "SunSystem.h"

#include "Plant/SunFlower.h"
#include "Plant/PeaShooter.h"
#include "Plant/SnowPeaShooter.h"
#include "Plant/CherryBomb.h"
#include "Plant/PotatoMine.h"
#include "Plant/WallNut.h"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;

USING_NS_CC;

bool PlantPlacementCore::init() {
    if (!Layer::init()) {
        return false;
    }

    // 初始化状态
    _selectedPlantIndex = -1;  // 未选中任何植物
    _draggingPlant = nullptr;
    _sunSystem = nullptr;

    // 初始化冷却计时器
    for (int i = 0; i < 6; i++) {
        _cooldownTimers.push_back(0.0f);
    }

    // 获取屏幕信息
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 设置格子大小和起始位置
    _gridSize = Size(80, 100);

    // 修正：第一个格子（0行0列）的左下角位置
    _gridStartPos = Vec2(origin.x + 50, origin.y + 20);

    // 初始化植物槽位（移到左上角）
    initPlantSlots();

    // 绘制调试网格（可选）
    drawDebugGrids();

    // 设置触摸事件
    setupTouchHandlers();

    // 调度冷却更新
    schedule(schedule_selector(PlantPlacementCore::updateCooldown), 0.1f);

    return true;
}

// 初始化植物槽位（左上角）
void PlantPlacementCore::initPlantSlots() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 修改：将植物槽位移到左上角（垂直排列）
    float startX = origin.x + 150;  // 离左边缘150像素
    float startY = origin.y + visibleSize.height - 5;  // 离顶部5像素

    for (int i = 0; i < 6; i++) {
        // 正常颜色植物图标
        auto plantIcon = Sprite::create(_plantImages[i]);
        if (plantIcon) {
            plantIcon->setAnchorPoint(Vec2(0, 1));
            plantIcon->setPosition(Vec2(startX + i * 60, startY));
            plantIcon->setTag(i);  // 用tag标识植物类型
            this->addChild(plantIcon, 2);
            _plantSlots.push_back(plantIcon);
        }
        else {
            // 如果加载失败，使用一个占位符
            plantIcon = Sprite::create();
            plantIcon->setContentSize(Size(50, 50));
            plantIcon->setAnchorPoint(Vec2(0, 1));
            plantIcon->setPosition(Vec2(startX + i * 60, startY));
            plantIcon->setTag(i);
            this->addChild(plantIcon, 2);
            _plantSlots.push_back(plantIcon);
        }

        // 灰色植物图标（用于冷却和阳光不足）
        auto grayIcon = Sprite::create(_plantImages[i]);
        if (grayIcon) {
            grayIcon->setAnchorPoint(Vec2(0, 1));
            grayIcon->setPosition(Vec2(startX + i * 60, startY));
            grayIcon->setTag(i + 10);
            grayIcon->setColor(Color3B(100, 100, 100));  // 变灰
            grayIcon->setVisible(false);
            this->addChild(grayIcon, 1);
            _plantGraySlots.push_back(grayIcon);
        }
        else {
            // 如果加载失败，使用一个占位符
            grayIcon = Sprite::create();
            grayIcon->setContentSize(Size(50, 50));
            grayIcon->setAnchorPoint(Vec2(0, 1));
            grayIcon->setPosition(Vec2(startX + i * 60, startY));
            grayIcon->setTag(i + 10);
            grayIcon->setColor(Color3B(100, 100, 100));
            grayIcon->setVisible(false);
            this->addChild(grayIcon, 1);
            _plantGraySlots.push_back(grayIcon);
        }

        // 冷却进度条（从上到下）
        // 先创建一个简单的矩形精灵作为进度条背景
        auto maskSprite = Sprite::create();
        if (maskSprite) {
            // 创建一个小的矩形精灵
            maskSprite->setTextureRect(Rect(0, 0, 50, 50));
            maskSprite->setColor(Color3B::BLACK);
            maskSprite->setOpacity(150);  // 半透明

            auto progress = ProgressTimer::create(maskSprite);
            if (progress) {
                progress->setType(ProgressTimer::Type::BAR);
                progress->setMidpoint(Vec2(0.5f, 1.0f));  // 从上到下
                progress->setBarChangeRate(Vec2(0, 1));   // 垂直变化
                progress->setPercentage(100);  // 初始满的
                progress->setAnchorPoint(Vec2(0, 1));
                progress->setPosition(Vec2(startX + i * 60, startY));
                progress->setTag(i + 20);
                progress->setVisible(false);  // 默认隐藏
                this->addChild(progress, 3);
                _cooldownProgress.push_back(progress);
            }
            else {
                _cooldownProgress.push_back(nullptr);
            }
        }
        else {
            _cooldownProgress.push_back(nullptr);
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
    auto startLabel = Label::createWithTTF("origin(0,0)", "fonts/Marker Felt.ttf", 12);
    startLabel->setPosition(_gridStartPos + Vec2(0, -20));
    startLabel->setColor(Color3B::RED);

    this->addChild(drawNode, 0);
    this->addChild(startMarker, 1);
    this->addChild(startLabel, 1);
}

// 设置触摸事件
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

// 触摸开始
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
            if (canSelectPlant((int)i)) {
                selectPlant((int)i);
            }
            else {
                // 显示无法选择的提示
                showCannotSelectTip((int)i);
            }
            return;
        }
    }
}

// 触摸移动
void PlantPlacementCore::onTouchMoved(Touch* touch) {
    if (_selectedPlantIndex != -1 && _draggingPlant) {
        _draggingPlant->setPosition(touch->getLocation());
    }
}

// 触摸结束
void PlantPlacementCore::onTouchEnded(Touch* touch) {
    // 可以添加一些触摸结束的逻辑
}

// 更新冷却系统
void PlantPlacementCore::updateCooldown(float dt) {
    for (int i = 0; i < 6; i++) {
        if (_cooldownTimers[i] > 0) {
            _cooldownTimers[i] -= dt;
            if (_cooldownTimers[i] < 0) {
                _cooldownTimers[i] = 0;
            }

            // 更新进度条
            float progressPercentage = (_cooldownTimers[i] / _cooldownDurations[i]) * 100;
            if (_cooldownProgress[i]) {
                _cooldownProgress[i]->setPercentage(progressPercentage);
            }

            // 如果冷却完成
            if (_cooldownTimers[i] <= 0) {
                updatePlantSlotState(i, true);
            }
        }
    }
}

// 开始冷却
void PlantPlacementCore::startCooldown(int plantIndex) {
    if (plantIndex >= 0 && plantIndex < 6) {
        _cooldownTimers[plantIndex] = _cooldownDurations[plantIndex];
        updatePlantSlotState(plantIndex, false);

        // 显示冷却进度条
        if (_cooldownProgress[plantIndex]) {
            _cooldownProgress[plantIndex]->setVisible(true);
            _cooldownProgress[plantIndex]->setPercentage(100);
        }
    }
}

// 检查是否可以选中植物
bool PlantPlacementCore::canSelectPlant(int plantIndex) {
    // 检查冷却
    if (_cooldownTimers[plantIndex] > 0) {
        return false;
    }

    // 检查阳光是否足够
    if (_sunSystem && _sunSystem->getSunCount() < _sunCosts[plantIndex]) {
        return false;
    }

    return true;
}

// 更新植物槽位状态
void PlantPlacementCore::updatePlantSlotState(int index, bool enabled) {
    if (index >= 0 && index < _plantSlots.size()) {
        if (enabled) {
            // 可用状态
            _plantSlots[index]->setVisible(true);
            if (_plantGraySlots[index]) {
                _plantGraySlots[index]->setVisible(false);
            }
            if (_cooldownProgress[index]) {
                _cooldownProgress[index]->setVisible(false);
            }
        }
        else {
            // 不可用状态（冷却中）
            _plantSlots[index]->setVisible(false);
            if (_plantGraySlots[index]) {
                _plantGraySlots[index]->setVisible(true);
            }
        }
    }
}

// 显示无法选择的提示
void PlantPlacementCore::showCannotSelectTip(int plantIndex) {
    std::string tipText;

    // 检查原因
    if (_cooldownTimers[plantIndex] > 0) {
        tipText = "Plant on Cooldown!";
    }
    else if (_sunSystem && _sunSystem->getSunCount() < _sunCosts[plantIndex]) {
        tipText = "Not enough sun!";
        tipText += " Need: " + std::to_string(_sunCosts[plantIndex]);
    }
    else {
        tipText = "Can't select!";
    }

    auto label = Label::createWithTTF(tipText, "fonts/Marker Felt.ttf", 60);
    label->setColor(Color3B::RED);
    label->setPosition(Vec2(400, 100));
    this->addChild(label, 100);

    // 淡出并移除提示
    label->runAction(Sequence::create(
        MoveBy::create(1.0f, Vec2(0, 30)),
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr));
}

// 选择植物
void PlantPlacementCore::selectPlant(int plantIndex) {
    if (_selectedPlantIndex != -1) {
        cancelSelection();
    }

    // 检查阳光是否足够
    if (_sunSystem && _sunSystem->getSunCount() < _sunCosts[plantIndex]) {
        showCannotSelectTip(plantIndex);
        return;
    }

    // 创建种植提示
    auto label = Label::createWithTTF("Please place it at map!", "fonts/Marker Felt.ttf", 90);
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

// 取消选择
void PlantPlacementCore::cancelSelection() {
    _selectedPlantIndex = -1;

    if (_draggingPlant) {
        _draggingPlant->removeFromParent();
        _draggingPlant = nullptr;
    }
}

// 放置植物到指定位置
void PlantPlacementCore::placePlantAtPosition(const Vec2& position) {
    if (_selectedPlantIndex == -1 || !_draggingPlant) {
        return;
    }

    // 检查阳光是否足够
    if (_sunSystem && _sunSystem->getSunCount() < _sunCosts[_selectedPlantIndex]) {
        showCannotSelectTip(_selectedPlantIndex);
        cancelSelection();
        return;
    }

    // 计算点击位置对应的行列
    float offsetX = position.x - _gridStartPos.x;
    float offsetY = position.y - _gridStartPos.y;

    int col = static_cast<int>(offsetX / _gridSize.width);
    int row = static_cast<int>(offsetY / _gridSize.height);

    // 检查是否在有效范围内
    if (col < 0 || col >= GRID_COLS || row < 0 || row >= GRID_ROWS) {
        auto label = Label::createWithTTF("You can't placed here!", "fonts/Marker Felt.ttf", 90);
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

    // 消耗阳光
    if (_sunSystem) {
        if (!_sunSystem->consumeSun(_sunCosts[_selectedPlantIndex])) {
            showCannotSelectTip(_selectedPlantIndex);
            cancelSelection();
            return;
        }
    }

    // 计算格子中心位置
    float gridCenterX = _gridStartPos.x + col * _gridSize.width + _gridSize.width / 2;
    float gridCenterY = _gridStartPos.y + row * _gridSize.height + _gridSize.height / 2;

    // 创建实际的植物
    if (_selectedPlantIndex == 0)
    {
        auto plant = SunFlower::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
    }
    else if (_selectedPlantIndex == 1)
    {
        auto plant = Peashooter::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);

        if (auto shooter = dynamic_cast<Peashooter*>(plant)) {
            shooter->setCreateBulletCallback([]() -> Bullet* {
                auto bullet = Bullet::create();
                if (bullet) {
                    GameDataCenter::getInstance()->addBullet(bullet);
                }
                return bullet;
                });

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
    else if (_selectedPlantIndex == 2)
    {
        auto plant = SnowPeaShooter::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);

        if (auto shooter = dynamic_cast<SnowPeaShooter*>(plant)) {
            shooter->setCreateBulletCallback([]() -> Bullet* {
                auto bullet = SnowPea::create();
                if (bullet) {
                    GameDataCenter::getInstance()->addBullet(bullet);
                }
                return bullet;
                });

            plant->setZombieCheckCallback([this](int row) -> bool {
                auto& zombies = GameDataCenter::getInstance()->getZombies();
                for (auto zombie : zombies) {
                    if (zombie && zombie->isAlive() && !zombie->getIsDead() && zombie->getRow() == row) {
                        return true;
                    }
                }
                return false;
                });
        }
    }
    else if (_selectedPlantIndex == 3)
    {
        auto plant = CherryBomb::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
        auto& zombielist = GameDataCenter::getInstance()->getZombies();
        plant->setZombiesList(&zombielist);
    }
    else if (_selectedPlantIndex == 4)
    {
        auto plant = PotatoMine::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
        auto& zombielist = GameDataCenter::getInstance()->getZombies();
        plant->setZombiesList(&zombielist);
    }
    else if (_selectedPlantIndex == 5)
    {
        auto plant = WallNut::create();
        plant->setPosition(Vec2(gridCenterX, gridCenterY));
        plant->planted(row, col, 80, 100);
        this->addChild(plant);
        GameDataCenter::getInstance()->addPlant(plant);
    }

    //这个可以.ogg!!!!原来那个不行!!!
    AudioEngine::play2d("music/plant.ogg", false, 1.0f);

    // 启动冷却
    startCooldown(_selectedPlantIndex);

    // 取消选择状态
    cancelSelection();

    auto label = Label::createWithTTF(StringUtils::format("Put at (%d,%d)", row, col), "fonts/Marker Felt.ttf", 90);
    label->setColor(Color3B::GREEN);
    label->setPosition(Vec2(400, 100));
    this->addChild(label, 100);

    // 淡出并移除提示
    label->runAction(Sequence::create(
        MoveBy::create(1.0f, Vec2(0, 30)),
        FadeOut::create(0.5f),
        RemoveSelf::create(),
        nullptr
    ));
}

// 初始化地图格子（如果需要的话）
void PlantPlacementCore::initMapGrids() {
    // 可以在这里初始化地图格子，如果需要的话
    // 目前已经在drawDebugGrids中绘制了
}