#include "GameSystem.h"
#include "GameScene.h"
#include "Zombie/Zombie.h"  // 假设Zombie类已存在

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
    _spawnInterval = 3.0f;
    _isGameRunning = false;

    // 创建进度条UI
    createProgressBar();

    return true;
}

void GameSystem::createProgressBar() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 进度条背景
    auto bg = Sprite::create("Menu//FlagMeterEmpty.png");
    bg->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    bg->setPosition(Vec2(visibleSize.width - 150, 80));
    this->addChild(bg);

    // 进度条
    auto progressSprite = Sprite::create("Menu/FlagMeterFull.png");
    if (!progressSprite) {
        progressSprite = Sprite::create();
        progressSprite->setTextureRect(Rect(0, 0, 245, 20));
        progressSprite->setColor(Color3B(0, 200, 0));
    }

    _progressBar = ProgressTimer::create(progressSprite);
    _progressBar->setType(ProgressTimer::Type::BAR);
    _progressBar->setMidpoint(Vec2(0, 0.5f));
    _progressBar->setBarChangeRate(Vec2(1, 0));
    _progressBar->setPercentage(0);
    _progressBar->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _progressBar->setPosition(bg->getPosition());
    this->addChild(_progressBar);

    // 进度标签
    _progressLabel = Label::createWithTTF("进度: 0/30", "fonts/Marker Felt.ttf", 18);
    _progressLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _progressLabel->setPosition(Vec2(bg->getPositionX(), bg->getPositionY() - 20));
    _progressLabel->setColor(Color3B::WHITE);
    this->addChild(_progressLabel);
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
    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}

void GameSystem::spawnZombie(float dt) {
    if (_zombiesSpawned >= _totalZombies || !_isGameRunning) {
        return;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建僵尸 - 使用现有的Zombie类
    auto zombie = Zombie::create();  // 假设Zombie类已实现
    _zombiesSpawned++;

    // 设置僵尸位置（右侧屏幕外）
    float randomY = cocos2d::random(100.0f, visibleSize.height - 100);
    zombie->setPosition(Vec2(visibleSize.width + 50, randomY));

    // 为僵尸添加标签以便识别
    zombie->setTag(1000 + _zombiesSpawned);

    // 添加触摸事件监听器（当僵尸被点击时，视为被击败）
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto target = event->getCurrentTarget();
        if (target->getBoundingBox().containsPoint(touch->getLocation())) {
            this->onZombieDefeated(target);
            return true;
        }
        return false;
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, zombie);

    // 添加僵尸到游戏层
    _gameLayer->addChild(zombie, 5);


    // 随着游戏进行，加快生成速度
    if (_spawnInterval > 1.0f && _zombiesSpawned % 5 == 0) {
        _spawnInterval -= 0.2f;
        this->unschedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie));
        this->schedule(CC_SCHEDULE_SELECTOR(GameSystem::spawnZombie), _spawnInterval);
    }
}

void GameSystem::onZombieDefeated(Node* zombie) {
    _defeatedZombies++;

    // 从父节点移除僵尸
    zombie->removeFromParent();

    // 更新进度
    updateProgress();

    // 如果击败了所有僵尸，游戏胜利
    if (_defeatedZombies >= _totalZombies) {
        pauseGame();

        auto visibleSize = Director::getInstance()->getVisibleSize();
        auto victoryLabel = Label::createWithTTF("游戏胜利！", "fonts/Marker Felt.ttf", 48);
        victoryLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
        victoryLabel->setColor(Color3B::YELLOW);
        this->addChild(victoryLabel, 100);
    }
}

void GameSystem::updateProgress() {
    float progress = (float)_defeatedZombies / _totalZombies * 100.0f;

    // 更新进度条
    _progressBar->setPercentage(progress);

    // 根据进度改变颜色
    if (progress > 66) {
        _progressBar->getSprite()->setColor(Color3B(0, 200, 0));
    }
    else if (progress > 33) {
        _progressBar->getSprite()->setColor(Color3B(255, 200, 0));
    }
    else {
        _progressBar->getSprite()->setColor(Color3B(200, 0, 0));
    }

    // 更新标签
    std::string text = StringUtils::format("进度: %d/%d", _defeatedZombies, _totalZombies);
    _progressLabel->setString(text);
}