#ifndef CONEHEADZOMBIE_H
#define CONEHEADZOMBIE_H

#include "Zombie.h"

class ConeheadZombie : public Zombie {
public:
    // 沿用父类的CREATE_FUNC宏，保证创建对象的方式一致
    CREATE_FUNC(ConeheadZombie);

    /**
     * @brief 重写初始化方法，设置路障僵尸的专属属性
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;
};

#endif // CONEHEADZOMBIE_H