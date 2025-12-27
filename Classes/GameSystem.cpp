#include "GameDataCenter.h"
#include "GameSystem.h"
#include "GameScene.h"
#include "Zombie/Zombie.h"
#include "Zombie/ConeheadZombie.h" 
#include "Zombie/BucketheadZombie.h" 
#include <typeinfo>


#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;

USING_NS_CC;

GameSystem* GameSystem::create(Node* gameLayer) {
    GameSystem* pRet = new (std::nothrow) GameSystem();
    if (pRet && pRet->init(gameLayer)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool GameSystem::init(Node* gameLayer) {
    if (!Node::init()) {
        return false;
    }

    _gameLayer = gameLayer;
    _defeatedZombies = 0;
    _zombiesSpawned = 0;
    _totalZombies = 30;  // 总共30个僵尸
    _spawnInterval = 5.0f;
    _isGameRunning = false;
    _isGameOver = false;

    // 创建进度条UI
    createProgressBar();

    // 设置僵尸死亡回调
    GameDataCenter::getInstance()->setZombieDeathCallback(
        [this](Zombie* zombie) {
            this->onZombieDefeated(zombie);
        }
    );
    return true;
}

void GameSystem::createProgressBar() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 进度条背景
    auto bg = Sprite::create("Menu//FlagMeterEmpty.png");
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    bg->setPosition(Vec2(visibleSize.width - 150, 80));
    this->addChild(bg, 30);

    // 进度条
    auto progressSprite = Sprite::create("Menu/FlagMeterFull.png");

    _progressBar = ProgressTimer::create(progressSprite);
    _progressBar->setType(ProgressTimer::Type::BAR);
    _progressBar->setMidpoint(Vec2(0, 0.5f));
    _progressBar->setBarChangeRate(Vec2(1, 0));
    _progressBar->setPercentage(0);
    _progressBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _progressBar->setPosition(bg->getPosition());
    this->addChild(_progressBar, 40);

    // 进度标签
    _progressLabel = Label::createWithTTF("0/30", "fonts/Marker Felt.ttf", 18);
    _progressLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _progressLabel->setPosition(Vec2(bg->getPositionX(), bg->getPositionY() - 20));
    _progressLabel->setColor(Color3B::WHITE);
    this->addChild(_progressLabel, 41);
}

void GameSystem::startGame() {
    if (_isGameRunning || _isGameOver) return;

    _isGameRunning = true;
    this->schedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie), _spawnInterval);
}

void GameSystem::pauseGame() {
    if (!_isGameRunning) return;

    _isGameRunning = false;
    this->unschedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie));

    // 暂停所有子节点的动作
    for (auto child : _gameLayer->getChildren()) {
        child->pause();
    }
}

void GameSystem::resumeGame() {
    if (_isGameRunning || _isGameOver) return;

    _isGameRunning = true;
    this->schedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie), _spawnInterval);

    // 恢复所有子节点的动作
    for (auto child : _gameLayer->getChildren()) {
        child->resume();
    }
}

void GameSystem::spawnZombie(float dt) {
    if (_zombiesSpawned >= _totalZombies || !_isGameRunning || _isGameOver) {
        return;
    }

    // 创建僵尸 - 使用现有的Zombie类
    if (_zombiesSpawned <= 10)
    {
        int randomrow = random<int>(0, 4);
        spawnZombie_standard<Zombie>(randomrow);
        _zombiesSpawned++;
    }
    else if (_zombiesSpawned > 10 && _zombiesSpawned < 20)
    {
        int randomrow = random<int>(0, 4);
        spawnZombie_standard<ConeheadZombie>(randomrow);
        _zombiesSpawned++;
    }
    else
    {
        int randomrow = random<int>(0, 4);
        spawnZombie_standard<BucketheadZombie>(randomrow);
        _zombiesSpawned++;
    }


    // 随着游戏进行，加快生成速度
    if (_spawnInterval > 3.0f && _zombiesSpawned % 5 == 0) {
        _spawnInterval -= 0.2f;
        this->unschedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie));
        this->schedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie), _spawnInterval);
    }
}

void GameSystem::onZombieDefeated(Zombie* zombie) {
    if (_isGameOver) return;

    _defeatedZombies++;
    // 更新进度
    updateProgress();

    // 如果击败了所有僵尸，游戏胜利
    if (_defeatedZombies >= _totalZombies) {
        AudioEngine::play2d("music/winmusic.ogg", false, 1.0f);
        Director::getInstance()->pause();
        _isGameOver = true;
        showGameOverUI(true); // true表示胜利
    }
}

void GameSystem::updateProgress() {
    float progress = (float)_defeatedZombies / _totalZombies * 100.0f;

    // 更新进度条
    _progressBar->setPercentage(progress);

    // 更新标签
    std::string text = StringUtils::format("%d/%d", _defeatedZombies, _totalZombies);
    _progressLabel->setString(text);
}

template<typename T>
void GameSystem::spawnZombie_standard(int row)
{
    // 创建僵尸实例
    auto zombie = T::create();
    if (zombie)
    {
        auto visibleSize = Director::getInstance()->getVisibleSize();
        // 设置僵尸属性
        zombie->setRow(row);                  // 设置所在行
        zombie->setPosition(Vec2(visibleSize.width + 50, 90 + row * 100)); // 设置初始位置
        zombie->setHouseBoundary(0.0f);     // 设置房子边界位置（可根据实际调整）

        // 设置僵尸到达房子的回调
        zombie->setReachHouseCallback(CC_CALLBACK_1(GameSystem::onZombieReachHouse, this));

        GameDataCenter::getInstance()->getZombies().push_back(zombie);

        // 添加僵尸到游戏层
        _gameLayer->addChild(zombie, 5);
    }
}

// 新增：僵尸到达房子的回调处理
void GameSystem::onZombieReachHouse(Zombie* zombie)
{
    if (zombie && !_isGameOver)
    {
        CCLOG("Game: Zombie reached house at row %d! Game over for this row!", zombie->getRow());

        AudioEngine::play2d("music/losemusic.ogg", false, 1.0f);
        Director::getInstance()->pause();
        _isGameOver = true;
        showGameOverUI(false); // false表示失败
    }
}

// 新增：重新开始按钮回调函数
void GameSystem::onRestartClicked(Ref* sender) {
    // 使用replaceScene重新开始
    auto scene = Game::createScene();
    Director::getInstance()->replaceScene(scene);
    Director::getInstance()->resume();
}

// 新增：显示游戏结束UI
void GameSystem::showGameOverUI(bool isWin) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 center = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    // 创建半透明黑色背景
    auto overlay = LayerColor::create(Color4B(0, 0, 0, 180), visibleSize.width, visibleSize.height);
    overlay->setPosition(Vec2::ZERO);
    this->addChild(overlay, 200);

    // 游戏结束文本
    std::string gameOverText = isWin ? "You Win!" : "You Lost!";
    Color3B textColor = isWin ? Color3B::GREEN : Color3B::RED;

    auto gameOverLabel = Label::createWithTTF(gameOverText, "fonts/Marker Felt.ttf", 64);
    gameOverLabel->setPosition(Vec2(center.x, center.y + 100));
    gameOverLabel->setColor(textColor);
    this->addChild(gameOverLabel, 201);

    // 使用MenuItemImage创建重新开始按钮
    auto restartItem = MenuItemImage::create(
        "Menu/restartButton.png",   // 未点击时的图片
        "Menu/restartButtonSelected.png", // 点击时的图片
        CC_CALLBACK_1(GameSystem::onRestartClicked, this));

    // 设置按钮位置和缩放
    restartItem->setPosition(Vec2(center.x, center.y - 50));
    restartItem->setScale(1.5f);

    // 创建菜单并添加按钮
    auto restartMenu = Menu::create(restartItem, nullptr);
    restartMenu->setPosition(Vec2::ZERO);
    this->addChild(restartMenu, 201);

    // 显示最终分数/统计信息
    std::string statsText = StringUtils::format("Zombies Defeated: %d/%d", _defeatedZombies, _totalZombies);
    auto statsLabel = Label::createWithTTF(statsText, "fonts/Marker Felt.ttf", 28);
    statsLabel->setPosition(Vec2(center.x, center.y - 150));
    statsLabel->setColor(Color3B::YELLOW);
    this->addChild(statsLabel, 201);

    // 如果是失败，显示具体原因
    if (!isWin) {
        auto reasonLabel = Label::createWithTTF("A zombie reached your house!", "fonts/Marker Felt.ttf", 24);
        reasonLabel->setPosition(Vec2(center.x, center.y - 200));
        reasonLabel->setColor(Color3B::ORANGE);
        this->addChild(reasonLabel, 201);
    }
}