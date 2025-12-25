#include "GameScene.h"
#include "SimpleAudioEngine.h"
#include"Plant/PeaShooter.h"
#include"Plant/SunFlower.h"
#include"Manager/CollisionDetector.h"
//#include "CardShop.h";

USING_NS_CC;

Scene* Game::createScene()
{
    return Game::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool Game::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.


    //// create menu, it's an autorelease object
    //auto menu = Menu::create(adventureModeItem, NULL);
    //menu->setPosition(Vec2::ZERO);
    //this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label


    // add "HelloWorld" splash screen"
    auto spriteMap = Sprite::create("map.jpg");
    if (spriteMap == nullptr)
    {
        problemLoading("'map.jpg'");
    }
    else
    {

        // position the sprite on the center of the screen
        spriteMap->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

        // add the sprite as a child to this layer
        this->addChild(spriteMap, 0);
    }

    auto spriteShop = Sprite::create("Shop.png");
    if (spriteShop == nullptr)
    {
        problemLoading("'Shop.png'");
    }
    else
    {
        spriteShop->setAnchorPoint(Vec2(0, 1));
        // position the sprite on the center of the screen
        spriteShop->setPosition(Vec2(0, visibleSize.height + origin.y));

        // add the sprite as a child to this layer
        spriteMap->addChild(spriteShop, 0);
    }

    //auto adventureModeItem = MenuItemImage::create(
    //    "adventureMode.png",
    //    "adventureModeSelected.png",
    //    CC_CALLBACK_1(Game::plantSelectedCallback, this));

    //if (adventureModeItem == nullptr ||
    //    adventureModeItem->getContentSize().width <= 0 ||
    //    adventureModeItem->getContentSize().height <= 0)
    //{
    //    problemLoading("'adventureMode.png' and 'adventureModeSelected.png'");
    //}
    //else
    //{
    //    float x = origin.x + visibleSize.width / 9 * 6;
    //    float y = origin.y + visibleSize.height / 6 * 4;
    //    adventureModeItem->setPosition(Vec2(x, y));
    //}

    //// create menu, it's an autorelease object
    //auto menu = Menu::create(adventureModeItem, NULL);
    //menu->setPosition(Vec2::ZERO);
    //this->addChild(menu, 1);
    //auto cardshop = CardShop::create();
    //this->addChild(cardshop, 0);

    float gridWidth = 80.0f;
    float gridHeight = 80.0f;
    Vec2 gridOrigin = Vec2(100, 150);

    float zombieStartX = visibleSize.width + origin.x - 50; // 屏幕右侧外50像素
    float zombieY = origin.y + visibleSize.height / 2;     // 垂直居中
    spawnZombie(1, zombieStartX, zombieY); // 生成第2行的僵尸

    auto sunflower = SunFlower::create();
    if (sunflower) {
        float gridWidth = 80.0f;
        float gridHeight = 80.0f;
        Vec2 gridOrigin = Vec2(50, 150);
        sunflower->setGridOrigin(gridOrigin);

        // 种植到第2行第0列
        sunflower->planted(1, 0, gridWidth, gridHeight);

        this->addChild(sunflower, 5);
        this->addPlant(sunflower);
    }

    auto peashooter1 = Peashooter::create();
    if (peashooter1) {

        peashooter1->setGridOrigin(gridOrigin) ;

        // 种植到第2行第5列
        peashooter1->planted(1, 5, gridWidth, gridHeight);

        // 设置子弹创建回调（原有）
        peashooter1->setCreateBulletCallback([this]() -> Bullet* {
            Bullet* bullet = Bullet::create();
            if (bullet) {
                this->addBullet(bullet);
            }
            return bullet;
            });

        // 新增：设置僵尸检测回调（核心）
        peashooter1->setZombieCheckCallback([this](int row) -> bool {
            // 遍历场景中的僵尸列表，检查指定行是否有存活僵尸
            for (auto zombie : m_zombies) {
                if (zombie && zombie->isAlive() && !zombie->getIsDead() && zombie->getRow() == row) {
                    return true; // 找到存活僵尸
                }
            }
            return false; // 无存活僵尸
            });

        this->addChild(peashooter1, 5);
        this->addPlant(peashooter1);
    }

    auto peashooter2 = Peashooter::create();
    if (peashooter2) {

        peashooter2->setGridOrigin(gridOrigin);

        // 种植到第2行第3列
        peashooter2->planted(1, 3, gridWidth, gridHeight);

        // 设置子弹创建回调（原有）
        peashooter2->setCreateBulletCallback([this]() -> Bullet* {
            Bullet* bullet = Bullet::create();
            if (bullet) {
                this->addBullet(bullet);
            }
            return bullet;
            });

        // 新增：设置僵尸检测回调（核心）
        peashooter2->setZombieCheckCallback([this](int row) -> bool {
            // 遍历场景中的僵尸列表，检查指定行是否有存活僵尸
            for (auto zombie : m_zombies) {
                if (zombie && zombie->isAlive() && !zombie->getIsDead() && zombie->getRow() == row) {
                    return true; // 找到存活僵尸
                }
            }
            return false; // 无存活僵尸
            });

        this->addChild(peashooter2, 5);
        this->addPlant(peashooter2);
    }
    //开启场景的update（用于碰撞检测） ==========
    this->scheduleUpdate();

    return true;
}

// 每帧更新 - 核心碰撞检测逻辑
void Game::update(float delta)
{
    // 1. 检测子弹与僵尸的碰撞
    CollisionDetector::detectBulletZombieCollisions(m_bullets, m_zombies);

    // 僵尸-植物碰撞检测（每帧执行，无间隔扣血）
    CollisionDetector::detectZombiePlantCollisions(m_zombies, m_plants);

    // 2. 清理死亡的僵尸/子弹（避免列表越界和内存泄漏）
    cleanupDeadObjects();
}

// 添加植物到列表
void Game::addPlant(Plant* plant)
{
    if (plant && std::find(m_plants.begin(), m_plants.end(), plant) == m_plants.end()) {
        m_plants.push_back(plant);
    }
}

void Game::plantSelectedCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

// 新增：添加子弹到管理列表
void Game::addBullet(Bullet* bullet)
{
    if (bullet && std::find(m_bullets.begin(), m_bullets.end(), bullet) == m_bullets.end()) {
        m_bullets.push_back(bullet);
    }
}

void Game::cleanupDeadObjects()
{
    // 清理死亡僵尸
    for (auto it = m_zombies.begin(); it != m_zombies.end();) {
        Zombie* zombie = *it;
        if (!zombie || zombie->getIsDead()) {
            it = m_zombies.erase(it); // 从列表移除
        }
        else {
            ++it;
        }
    }

    // 清理死亡子弹
    for (auto it = m_bullets.begin(); it != m_bullets.end();) {
        Bullet* bullet = *it;
        if (!bullet || bullet->getIsDead()) {
            it = m_bullets.erase(it); // 从列表移除
        }
        else {
            ++it;
        }
    }

    // 清理死亡植物
    for (auto it = m_plants.begin(); it != m_plants.end();) {
        Plant* plant = *it;
        if (!plant || plant->getIsDead()) {
            it = m_plants.erase(it);
        }
        else {
            ++it;
        }
    }
}

// 新增：生成僵尸的方法实现
void Game::spawnZombie(int row, float startX, float startY)
{
    // 创建僵尸实例
    auto zombie = Zombie::create();
    if (zombie)
    {
        // 设置僵尸属性
        zombie->setRow(row);                  // 设置所在行
        zombie->setPosition(Vec2(startX, startY)); // 设置初始位置
        zombie->setHouseBoundary(0.0f);     // 设置房子边界位置（可根据实际调整）

        // 设置僵尸到达房子的回调
        zombie->setReachHouseCallback(CC_CALLBACK_1(Game::onZombieReachHouse, this));



        // 将僵尸添加到场景
        this->addChild(zombie, 10); // z轴设为10，确保在地图上方显示

        m_zombies.push_back(zombie);

        CCLOG("Zombie spawned at row %d, position (%.1f, %.1f)", row, startX, startY);
    }
}

// 新增：僵尸到达房子的回调处理
void Game::onZombieReachHouse(Zombie* zombie)
{
    if (zombie)
    {
        CCLOG("Game: Zombie reached house at row %d! Game over for this row!", zombie->getRow());
        // 这里可以添加游戏失败逻辑、扣血、播放音效等
    }
}
