#include "GameDataCenter.h"
#include "GameSystem.h"
#include "GameScene.h"
#include "Zombie/Zombie.h"  // 假设Zombie类已存在
#include "Zombie/ConeheadZombie.h" 
#include "Zombie/BucketheadZombie.h" 
#include <typeinfo>

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
    this->addChild(bg,30);

    // 进度条
    auto progressSprite = Sprite::create("Menu/FlagMeterFull.png");

    _progressBar = ProgressTimer::create(progressSprite);
    _progressBar->setType(ProgressTimer::Type::BAR);
    _progressBar->setMidpoint(Vec2(0, 0.5f));
    _progressBar->setBarChangeRate(Vec2(1, 0));
    _progressBar->setPercentage(0);
    _progressBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _progressBar->setPosition(bg->getPosition());
    this->addChild(_progressBar,40);

    // 进度标签
    _progressLabel = Label::createWithTTF("0/30", "fonts/Marker Felt.ttf", 18);
    _progressLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _progressLabel->setPosition(Vec2(bg->getPositionX(), bg->getPositionY() - 20));
    _progressLabel->setColor(Color3B::WHITE);
    this->addChild(_progressLabel,41);
}

void GameSystem::startGame() {
    if (_isGameRunning) return;

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
    if (_isGameRunning) return;

    _isGameRunning = true;
    this->schedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie), _spawnInterval);

    // 恢复所有子节点的动作
    for (auto child : _gameLayer->getChildren()) {
        child->resume();
    }
}

void GameSystem::restartGame() {
    auto scene = Game::createScene();
    Director::getInstance()->replaceScene(scene);
}

void GameSystem::spawnZombie(float dt) {
    if (_zombiesSpawned >= _totalZombies || !_isGameRunning) {
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

void GameSystem::onZombieDefeated(Node* zombie) {
    _defeatedZombies++;
    // 更新进度
    updateProgress();

    // 如果击败了所有僵尸，游戏胜利
    if (_defeatedZombies >= _totalZombies) {
        pauseGame();

        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto victoryLabel = Label::createWithTTF("You win!", "fonts/Marker Felt.ttf", 48);
        victoryLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        victoryLabel->setColor(Color3B::YELLOW);
        this->addChild(victoryLabel, 100);
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
        zombie->setPosition(Vec2(visibleSize.width + 50, 50 + row * 100)); // 设置初始位置
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
    if (zombie)
    {
        CCLOG("Game: Zombie reached house at row %d! Game over for this row!", zombie->getRow());
        // 这里可以添加游戏失败逻辑、扣血、播放音效等
    }
}