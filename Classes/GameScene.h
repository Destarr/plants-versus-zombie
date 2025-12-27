#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class Game : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(Game);

    void Game::update(float delta);

    // 阳光管理方法
    void addSun(int amount);
    bool consumeSun(int amount);
    int getCurrentSun() const;
    void updateSunDisplay();

    // 阳光显示UI
    void createSunDisplay();

private:
    // 阳光相关
    int m_currentSun;
    cocos2d::Label* m_sunLabel;
    cocos2d::Sprite* m_sunIcon;

    // 阳光变化回调
    std::function<void(int)> m_sunChangedCallback;
};

#endif // __GAME_SCENE_H__