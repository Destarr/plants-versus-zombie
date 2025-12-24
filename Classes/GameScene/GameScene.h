// GameScene.h
#ifndef GAME_SCENE_H
#define GAME_SCENE_H

#include "cocos2d.h"
#include "Zombie/Zombie.h"
#include "Bullet/Bullet.h"
#include "Plant/Plant.h"

USING_NS_CC;

class GameScene : public Scene {
public:
    // 使用CREATE_FUNC宏简化create函数编写
    CREATE_FUNC(GameScene);

    /**
     * @brief 初始化场景
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;

    /**
     * @brief 每帧更新函数，游戏主循环
     * @param delta 距离上一帧的时间间隔
     */
    virtual void update(float delta) override;

    /**
     * @brief 场景切换过渡结束回调
     */
    virtual void onEnterTransitionDidFinish() override;

    /**
     * @brief 场景退出回调
     */
    virtual void onExit() override;

    /**
     * @brief 检查僵尸是否抵达房子边界
     * @param zombie 要检查的僵尸对象
    * @return 如果抵达房子返回true，否则返回false
     */
    bool checkZombieReachHouse(Zombie* zombie);

    /**
     * @brief 处理僵尸抵达房子的事件
     * @param zombie 抵达房子的僵尸
     */
    void onZombieReachHouse(Zombie* zombie);

private:
    // 游戏对象容器
    Vector<Zombie*> m_zombies;           ///< 场景中存活的僵尸集合
    Vector<Bullet*> m_bullets;          ///< 场景中激活的子弹集合
    Vector<Plant*> m_plants;            ///< 场景中种植的植物集合

    // 游戏状态变量
    int m_sunCount;                      ///< 当前阳光数量
    int m_score;                         ///< 游戏得分
    bool m_isGameOver;                   ///< 游戏是否结束
    float m_zombieSpawnTimer;           ///< 僵尸生成计时器
    float m_sunGenerateTimer;           ///< 阳光生成计时器

    // UI元素
    Label* m_sunCountLabel;              ///< 阳光数量显示标签
    Label* m_scoreLabel;                  ///< 得分显示标签
    Menu* m_plantMenu;                   ///< 植物选择菜单

    /**
     * @brief 初始化游戏UI界面
     */
    void initUI();

    /**
     * @brief 初始化游戏数据
     */
    void initGameData();

    /**
     * @brief 初始化触摸事件监听
     */
    void initTouchListener();

    /**
     * @brief 碰撞检测总调度函数
     */
    void checkCollisions(float delta);

    /**
     * @brief 子弹与僵尸的碰撞检测
     */
    void checkBulletZombieCollisions();

    /**
     * @brief 植物与僵尸的碰撞检测
     */
    void checkPlantZombieCollisions();

    /**
     * @brief 生成僵尸
     */
    void spawnZombie(float delta);

    /**
     * @brief 生成阳光
     */
    void generateSun(float delta);

    /**
     * @brief 处理屏幕触摸事件
     * @param touch 触摸对象
     * @param event 触摸事件
     */
    void onTouchEnded(Touch* touch, Event* event);

    /**
     * @brief 在网格位置种植植物
     * @param plantType 植物类型
     * @param row 行坐标(0-4)
     * @param col 列坐标(0-8)
     */
    void plantAtPosition(const std::string& plantType, int row, int col);

    /**
     * @brief 屏幕坐标转换为网格坐标
     * @param position 屏幕坐标
     * @return 网格坐标(Vec2(row, col))
     */
    Vec2 convertToGridPosition(const Vec2& position);

    /**
     * @brief 游戏结束处理
     */
    void gameOver();

    /**
     * @brief 清理死亡对象，优化性能
     */
    void cleanupDeadObjects();

    /**
     * @brief 更新UI显示
     */
    void updateUI();

};

#endif // GAME_SCENE_H