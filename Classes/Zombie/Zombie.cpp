#include "Zombie.h"
#include "GameScene/GameScene.h" // 假设您的游戏主场景类，用于获取屏幕尺寸等信息

USING_NS_CC;

bool Zombie::init() {
    // 1. 首先调用父类的init函数
    if (!Sprite::init()) {
        return false;
    }

    // 2. 设置僵尸的通用属性（与生成位置无关的初始化）
    // 例如：加载僵尸的默认纹理
    if (!this->initWithFile("Zombie/NormalZombie.png")) { // 请替换为您的僵尸图片路径
        // 如果图片加载失败，可以尝试使用一个默认的矩形精灵，便于调试
        auto drawNode = DrawNode::create();
        drawNode->drawSolidRect(Vec2(-20, -30), Vec2(20, 30), Color4F(0.8f, 0.2f, 0.2f, 1.0f));
        this->addChild(drawNode);
    }

    // 3. 初始化成员变量
    m_maxHealth = 100;
    m_health = m_maxHealth;
    m_speed = 20.0f; // 像素/秒
    m_damage = 10;
    m_isDead = false;
    m_row = 0; // 默认行，通常会被generate函数覆盖



    // 5. 开启每帧更新调度
    this->scheduleUpdate();

    return true;
}

void Zombie::generate(int row) {
    // 设置僵尸生成的行
    m_row = row;

    // 获取游戏场景的可视区域大小，用于确定生成位置
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 计算生成位置：屏幕右侧边缘之外，y坐标根据行数计算
    // 假设每行的高度为100个像素，从第0行开始
    float startX = origin.x + visibleSize.width + this->getContentSize().width / 2; // 从右侧屏幕外开始
    float startY = origin.y + 100 + (row * 100); // 根据行数计算Y坐标，100为基准高度，100为行间距

    this->setPosition(startX, startY);

    // 可以在这里添加入场特效或动画
    // 例如：僵尸从地下钻出的动画
    this->setScale(0.5f); // 先缩小
    auto scaleAction = ScaleTo::create(0.5f, 1.0f); // 半秒内恢复到正常大小
    this->runAction(scaleAction);

    CCLOG("Zombie generated at row: %d, position: (%.1f, %.1f)", row, startX, startY);
}

void Zombie::takeDamage(int damage) {
    if (m_isDead) return; // 如果已经死亡，则不再处理伤害

    m_health -= damage;
    CCLOG("Zombie took %d damage. Health: %d/%d", damage, m_health, m_maxHealth);

    // 受伤反馈，例如闪烁效果
    auto blink = Blink::create(0.2f, 1);
    this->runAction(blink);

    // 移除了原有的 die() 调用。现在只判断并设置死亡状态。
    if (m_health <= 0) {
        m_health = 0;
        m_isDead = true; // 关键修改：设置死亡标志，而非立即调用die
    }
}

void Zombie::update(float delta) {
    // 先检查是否已标记为死亡状态，是则处理死亡流程
    if (m_isDead) {
        this->die(); // 在update中统一处理死亡
        return; // 死亡后不再执行本帧其他逻辑
    }

    // 以下是僵尸正常的更新逻辑（例如移动）
    if (m_isDead) return; // 再次检查，确保安全

    this->setPositionX(this->getPositionX() - m_speed * delta);
    // ... 其他更新逻辑，如检查是否到达屏幕左侧
}

void Zombie::die() {
    if (m_isDead) return; // 防止重复调用

    CCLOG("Zombie at row %d is dead!", m_row);

    // 1. 停止所有动作和更新调度
    this->stopAllActions();
    this->unscheduleUpdate();

    // 2. 播放死亡动画（如果有）
    // 3. 进行下沉消失的动画，并在动画结束后移除自身
    auto fadeOut = FadeOut::create(0.5f);
    auto remove = RemoveSelf::create(true);
    this->runAction(Sequence::create(fadeOut, remove, nullptr));

    // 4. 通知游戏场景（例如，增加分数）
}