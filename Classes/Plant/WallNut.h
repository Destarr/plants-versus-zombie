#pragma once
#include "Plant.h"

// 坚果墙子类 - 无需实现受损状态
class WallNut : public Plant {
public:
    // 遵循cocos2d-x的创建宏
    CREATE_FUNC(WallNut);

    // 重写析构函数
    virtual ~WallNut() override;

protected:
    // 重写初始化函数，设置坚果墙的特有属性
    virtual bool init() override;

    // 重写攻击函数（坚果墙无攻击能力，空实现）
    virtual void attack() override;

private:
    // 私有构造函数，遵循单例/创建模式
    WallNut();
};