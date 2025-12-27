#ifndef __GAME_SYSTEM_H__
#define __GAME_SYSTEM_H__

#include "cocos2d.h"
USING_NS_CC;

class GameSystem : public Node {
public:

    static GameSystem* create(Node* gameLayer);
    virtual bool init(Node* gameLayer);

    // 开始游戏
    void startGame();

    // 暂停游戏逻辑
    void pauseGame();

    // 恢复游戏逻辑
    void resumeGame();

    // 重新开始游戏
    void restartGame();

    void onZombieDefeated(Node* zombie);

private:
    Node* _gameLayer;
    ProgressTimer* _progressBar;
    Label* _progressLabel;

    int _defeatedZombies;
    int _totalZombies;
    int _zombiesSpawned;
    float _spawnInterval;
    bool _isGameRunning;

    void spawnZombie(float dt);
    template<typename T>
    void spawnZombie_standard(int row);

    //僵尸到达房子回调函数
    void onZombieReachHouse(Zombie* zombie);
    
    void updateProgress();
    void createProgressBar();
};

#endif // __GAME_SYSTEM_H__