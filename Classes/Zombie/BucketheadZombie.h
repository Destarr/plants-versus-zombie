#ifndef BUCKETHEADZOMBIE_H
#define BUCKETHEADZOMBIE_H

#include "Zombie.h"

class BucketheadZombie : public Zombie {
public:
    CREATE_FUNC(BucketheadZombie);

    /**
     * @brief 重写初始化方法，设置铁桶僵尸的专属属性
     * @return 初始化成功返回true，否则返回false
     */
    virtual bool init() override;
};

#endif // BUCKETHEADZOMBIE_H