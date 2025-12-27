// GameSystem.h 中需要添加的成员和方法声明
#ifndef __GAME_SYSTEM_H__
#define __GAME_SYSTEM_H__

#include "cocos2d.h"

class Node;
class Zombie;

class GameSystem : public cocos2d::Node {
public:
    static GameSystem* create(Node* gameLayer);
    bool init(Node* gameLayer);

    void startGame();
    void pauseGame();
    void resumeGame();

    // 新增函数声明
    void onRestartClicked(cocos2d::Ref* sender);
    void showGameOverUI(bool isWin);

    template<typename T>
    void spawnZombie_standard(int row);

private:
    void createProgressBar();
    void spawnZombie(float dt);
    void onZombieDefeated(Zombie* zombie);
    void onZombieReachHouse(Zombie* zombie);
    void updateProgress();

private:
    Node* _gameLayer;
    cocos2d::ProgressTimer* _progressBar;
    cocos2d::Label* _progressLabel;

    int _defeatedZombies;
    int _zombiesSpawned;
    int _totalZombies;
    float _spawnInterval;
    bool _isGameRunning;
    bool _isGameOver;
};

#endif // __GAME_SYSTEM_H__