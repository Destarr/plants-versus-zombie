#ifndef __MENU_SYSTEM_H__
#define __MENU_SYSTEM_H__

#include "cocos2d.h"

class MenuSystem : public cocos2d::Node {
public:
    CREATE_FUNC(MenuSystem);
    virtual bool init();

private:
    cocos2d::LayerColor* _menuOverlay;
    bool _isMenuVisible;

    void createMenuButton();
    void createPauseMenu();
    void onMenuButtonClicked(cocos2d::Ref* sender);
    void onContinueClicked(cocos2d::Ref* sender);
    void onRestartClicked(cocos2d::Ref* sender);
};

#endif // __MENU_SYSTEM_H__