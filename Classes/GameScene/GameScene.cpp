// GameScene.cpp
#include "GameScene.h"
#include "CollisionDetector.h"

bool GameScene::init() {
    if (!Scene::init()) {
        return false;
    }

    // 获取可视区域大小
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始化游戏数据
    initGameData();

    // 创建背景
    auto background = Sprite::create("background.png");
    background->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
    this->addChild(background, 0);

    // 初始化UI
    initUI();

    // 初始化触摸监听
    initTouchListener();

    // 设置游戏循环
    this->scheduleUpdate();

    // 设置碰撞检测调度（每秒检测30次）
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::checkCollisions), 1.0f / 30.0f);

    // 设置僵尸生成（每10秒生成一个）
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::spawnZombie), 10.0f);

    // 设置阳光生成（每15秒生成一次）
    this->schedule(CC_SCHEDULE_SELECTOR(GameScene::generateSun), 15.0f);

    CCLOG("GameScene initialized successfully");
    return true;
}

void GameScene::initGameData() {
    m_sunCount = 100;        // 初始阳光数量
    m_score = 0;             // 初始得分
    m_isGameOver = false;    // 游戏状态
    m_zombieSpawnTimer = 0.0f;
    m_sunGenerateTimer = 0.0f;
}

void GameScene::initUI() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建阳光计数标签
    m_sunCountLabel = Label::createWithTTF("100", "fonts/Marker Felt.ttf", 24);
    m_sunCountLabel->setPosition(Vec2(origin.x + 80, origin.y + visibleSize.height - 30));
    m_sunCountLabel->setColor(Color3B::YELLOW);
    this->addChild(m_sunCountLabel, 10);

    // 创建得分标签
    m_scoreLabel = Label::createWithTTF("Score: 0", "fonts/Marker Felt.ttf", 20);
    m_scoreLabel->setPosition(Vec2(origin.x + visibleSize.width - 80, origin.y + visibleSize.height - 30));
    this->addChild(m_scoreLabel, 10);

    // 创建植物选择菜单（这里简化实现）
    m_plantMenu = Menu::create();
    m_plantMenu->setPosition(Vec2::ZERO);
    this->addChild(m_plantMenu, 5);
}

void GameScene::initTouchListener() {
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void GameScene::update(float delta) {
    if (m_isGameOver) return;

    // 更新游戏逻辑
    m_zombieSpawnTimer += delta;
    m_sunGenerateTimer += delta;

    // 清理死亡对象
    cleanupDeadObjects();

    // 更新UI显示
    updateUI();
}

void GameScene::checkCollisions(float delta) {
    if (m_isGameOver) return;

    checkBulletZombieCollisions();
    checkPlantZombieCollisions();
}

void GameScene::checkBulletZombieCollisions() {
    for (auto* bullet : m_bullets) {
        if (!bullet || bullet->getIsDead() || !bullet->getIsActive()) continue;

        for (auto* zombie : m_zombies) {
            if (!zombie || zombie->getIsDead()) continue;

            // 使用 CollisionDetector 进行碰撞检测
            if (CollisionDetector::checkBulletZombieCollision(bullet, zombie)) {
                // 僵尸受到伤害
                zombie->takeDamage(bullet->getDamage());

                // 子弹消失
                bullet->die();

                // 一颗子弹只能击中一个僵尸
                break;
            }
        }
    }
}

void GameScene::checkPlantZombieCollisions() {
    for (auto* plant : m_plants) {
        if (!plant || plant->getIsDead()) continue;

        for (auto* zombie : m_zombies) {
            if (!zombie || zombie->getIsDead()) continue;

            // 只在同一行检测碰撞，优化性能
            if (plant->getRow() != zombie->getRow()) continue;

            // 使用 CollisionDetector 进行碰撞检测
            if (CollisionDetector::checkPlantZombieCollision(plant, zombie)) {
                // 植物受到僵尸攻击
                plant->takeDamage(zombie->getDamage());

                // 可以在这里添加僵尸啃食植物的逻辑
                CCLOG("Zombie is attacking plant at row %d", plant->getRow());
            }
        }
    }
}

bool GameScene::checkZombieReachHouse(Zombie* zombie) {
    if (!zombie || zombie->getIsDead()) {
        return false;
    }

    // 获取僵尸位置
    Vec2 zombiePos = zombie->getPosition();

    // 定义房子的边界（假设房子在屏幕左侧，x=50的位置）
    float houseBoundary = 50.0f;

    // 检查僵尸是否越过边界
    if (zombiePos.x <= houseBoundary) {
        // 僵尸抵达房子，触发事件
        onZombieReachHouse(zombie);
        return true;
    }

    return false;
}

void GameScene::onZombieReachHouse(Zombie* zombie) {
    if (!zombie) return;

    CCLOG("Zombie reached house! Game over triggered.");

    // 这里可以触发游戏结束逻辑
    // 例如：减少玩家生命值、播放音效、显示提示等

    // 示例：触发游戏结束
    // gameOver();

    // 或者只是标记僵尸状态，不立即结束游戏
    // 可以让僵尸开始啃食房子动画等
}

void GameScene::spawnZombie(float delta) {
    if (m_isGameOver) return;

    // 随机选择一行生成僵尸
    int row = rand() % 5; // 5行

    auto zombie = Zombie::create();
    if (zombie) {
        auto visibleSize = Director::getInstance()->getVisibleSize();

        // 设置僵尸初始位置（屏幕右侧外）
        zombie->setPosition(Vec2(visibleSize.width + 100, 100 + row * 100));
        zombie->setRow(row);

        // 设置游戏场景引用，用于边界检测
        zombie->setGameScene(this);

        this->addChild(zombie);
        m_zombies.pushBack(zombie);

        CCLOG("Zombie spawned at row %d", row);
    }
}

void GameScene::generateSun(float delta) {
    if (m_isGameOver) return;

    m_sunCount += 25; // 每次生成25阳光
    CCLOG("Sun generated, current sun: %d", m_sunCount);
}

void GameScene::onTouchEnded(Touch* touch, Event* event) {
    if (m_isGameOver) return;

    Vec2 touchLocation = touch->getLocation();

    // 转换触摸位置到网格坐标
    Vec2 gridPos = convertToGridPosition(touchLocation);
    int row = static_cast<int>(gridPos.x);
    int col = static_cast<int>(gridPos.y);

    // 检查是否在有效网格范围内（5行9列）
    if (row >= 0 && row < 5 && col >= 0 && col < 9) {
        // 这里可以添加植物种植逻辑
        CCLOG("Touch at grid position: row=%d, col=%d", row, col);

        // 示例：在点击位置种植向日葵（需要阳光足够等逻辑）
        if (m_sunCount >= 50) { // 假设向日葵成本50阳光
            plantAtPosition("sunflower", row, col);
            m_sunCount -= 50;
        }
    }
}

Vec2 GameScene::convertToGridPosition(const Vec2& position) {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 简化的网格坐标转换（实际需要根据游戏背景调整）
    int row = static_cast<int>((position.y - 100) / 100); // 假设每行高100，起始y=100
    int col = static_cast<int>((position.x - 80) / 80);   // 假设每列宽80，起始x=80

    return Vec2(row, col);
}

void GameScene::plantAtPosition(const std::string& plantType, int row, int col) {
    // 检查该位置是否已有植物
    for (auto* plant : m_plants) {
        if (plant->getRow() == row && plant->getCol() == col && !plant->getIsDead()) {
            CCLOG("Position already has a plant");
            return;
        }
    }

    Plant* newPlant = nullptr;

    if (plantType == "sunflower") {
        newPlant = Plant::create();
        // 设置向日葵属性
        if (newPlant) {
            newPlant->setSunCost(50);
            newPlant->setHealth(100);
            newPlant->setMaxHealth(100);
        }
    }
    // 可以添加其他植物类型...

    if (newPlant) {
        // 设置植物位置（根据网格坐标计算实际位置）
        float x = 80 + col * 80;  // 根据实际布局调整
        float y = 100 + row * 100; // 根据实际布局调整

        newPlant->setPosition(Vec2(x, y));
        newPlant->setRow(row);
        newPlant->setCol(col);

        this->addChild(newPlant);
        m_plants.pushBack(newPlant);

        CCLOG("Plant %s planted at row=%d, col=%d", plantType.c_str(), row, col);
    }
}

void GameScene::cleanupDeadObjects() {
    // 清理死亡的僵尸
    m_zombies.erase(std::remove_if(m_zombies.begin(), m_zombies.end(),
        [](Zombie* zombie) {
            bool dead = zombie && zombie->getIsDead();
            if (dead) {
                zombie->removeFromParent(); // 从场景中移除
            }
            return dead;
        }), m_zombies.end());

    // 清理死亡的植物
    m_plants.erase(std::remove_if(m_plants.begin(), m_plants.end(),
        [](Plant* plant) {
            bool dead = plant && plant->getIsDead();
            if (dead) {
                plant->removeFromParent(); // 从场景中移除
            }
            return dead;
        }), m_plants.end());

    // 清理死亡的子弹
    m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(),
        [](Bullet* bullet) {
            bool dead = bullet && bullet->getIsDead();
            if (dead) {
                bullet->removeFromParent(); // 从场景中移除
            }
            return dead;
        }), m_bullets.end());
}

void GameScene::updateUI() {
    // 更新阳光计数显示
    m_sunCountLabel->setString(std::to_string(m_sunCount));

    // 更新得分显示
    m_scoreLabel->setString("Score: " + std::to_string(m_score));
}

void GameScene::gameOver() {
    m_isGameOver = true;

    // 停止所有调度器
    this->unscheduleAllCallbacks();

    // 显示游戏结束界面
    auto gameOverLabel = Label::createWithTTF("GAME OVER", "fonts/Marker Felt.ttf", 48);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    gameOverLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
        origin.y + visibleSize.height / 2));
    gameOverLabel->setColor(Color3B::RED);
    this->addChild(gameOverLabel, 100);

    CCLOG("Game Over!");
}

void GameScene::onEnterTransitionDidFinish() {
    Scene::onEnterTransitionDidFinish();
    CCLOG("GameScene transition finished");
}

void GameScene::onExit() {
    // 清理资源
    this->unscheduleAllCallbacks();
    Scene::onExit();
    CCLOG("GameScene exited");
}