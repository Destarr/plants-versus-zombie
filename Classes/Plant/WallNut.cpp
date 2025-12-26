#include "WallNut.h"

WallNut::WallNut() : Plant() {
    // 初始化坚果墙的基础属性
    // 生命值、阳光消耗等核心属性在这里初始化
    m_sunCost = 50;          // 坚果墙阳光消耗50
    m_maxHealth = 4000;      // 坚果墙生命值4000（远高于普通植物）
    m_health = m_maxHealth;  // 初始生命值等于最大生命值
    m_damage = 0;            // 坚果墙无攻击力
    m_attackInterval = 0.0f; // 无攻击间隔
}

WallNut::~WallNut() {}

bool WallNut::init() {
    // 先调用父类的初始化函数
    if (!Plant::init()) {
        return false;
    }

    // 移除父类的绿色占位符，替换为坚果墙的视觉样式
    // 清空现有子节点（移除绿色方块）
    this->removeAllChildren();

    this->setTexture("Plant/WallNut.png");

    // 日志输出，确认坚果墙初始化成功
    CCLOG("WallNut initialized with health: %d, sun cost: %d", m_health, m_sunCost);

    return true;
}

void WallNut::attack() {
    // 坚果墙无攻击能力，重写为空实现
    // 覆盖父类的攻击日志，避免不必要的输出
}