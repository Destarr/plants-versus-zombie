#include "CollisionDetector.h"
#include"Plant/PotatoMine.h"

#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;

void CollisionDetector::detectBulletZombieCollisions(
    const std::vector<Bullet*>& bullets,
    const std::vector<Zombie*>& zombies
) {
    // 遍历所有子弹，逐个检测碰撞
    for (auto bullet : bullets) {
        if (bullet && bullet->getIsActive() && !bullet->getIsDead()) {
            checkSingleBulletCollisions(bullet, zombies);
        }
    }
}

void CollisionDetector::checkSingleBulletCollisions(Bullet* bullet, const std::vector<Zombie*>& zombies) {
    int bulletRow = bullet->getRow();
    // 只检测同行的僵尸（优化性能，避免全量检测）
    for (auto zombie : zombies) {
        if (zombie && zombie->isAlive() && !zombie->getIsDead() && zombie->getRow() == bulletRow) {
            // 调用基础碰撞检测方法
            if (checkCollision(bullet, zombie)) {
                // 触发子弹命中僵尸的逻辑
                bullet->onHitZombie(zombie);
                // 子弹命中后立即退出，避免一颗子弹命中多个僵尸
                break;
            }
        }
    }
}

void CollisionDetector::detectZombiePlantCollisions(
    const std::vector<Zombie*>& zombies,
    const std::vector<Plant*>& plants
) {
    CCLOG("Collision detect: zombies count = %zu, plants count = %zu", zombies.size(), plants.size());

    for (auto zombie : zombies) {
        // 仅检测存活、未死亡、未抵达房子的僵尸
        if (zombie && zombie->isAlive() && !zombie->getIsDead() && !zombie->getHasReachedHouse()) {
            checkSingleZombieCollisions(zombie, plants);
        }
    }
}

void CollisionDetector::checkSingleZombieCollisions(Zombie* zombie, const std::vector<Plant*>& plants) {
    int zombieRow = zombie->getRow();
    int zombieDamage = zombie->getDamage(); // 获取僵尸攻击力

    CCLOG("Check zombie row %d, damage %d, position (%.1f, %.1f)",
        zombieRow, zombieDamage,
        zombie->getPositionX(), zombie->getPositionY());

    for (auto plant : plants) {
        // 仅检测存活、未死亡、同行的植物
        if (plant && !plant->getIsDead() && plant->getRow() == zombieRow) {
            CCLOG("Check plant row %d, health %d, position (%.1f, %.1f)",
                plant->getRow(), plant->getHealth(),
                plant->getPositionX(), plant->getPositionY());

            // ========== 土豆地雷专属逻辑 ==========
            PotatoMine* potatoMine = dynamic_cast<PotatoMine*>(plant);
            if (potatoMine) {
                if (checkCollision(zombie, potatoMine)) {

                    // 僵尸触碰土豆地雷，触发爆炸
                    potatoMine->onZombieCollision(zombie);
                }
            }

            if (checkCollision(zombie, plant)) {

                zombie->setSpeed(0.0f); // 僵尸停止移动，专注攻击植物

                //检查攻击冷却，仅冷却结束时扣血
                if (zombie->m_attackCounter <= 0) {
                    CCLOG("Collision detected! Zombie hit plant, damage: %d", zombieDamage);
                    AudioEngine::play2d("music/chomp.ogg", false, 1.0f);
                    plant->takeDamage(zombieDamage);
                    // 重置攻击冷却计时器
                    zombie->m_attackCounter = zombie->getAttackInterval();

                    // 3. 检查植物是否被击杀
                    if (plant->getHealth() <= 0 || plant->getIsDead()) {
                        AudioEngine::play2d("music/gulp.ogg", false, 1.0f);
                        CCLOG("Plant died! Zombie resume moving, speed=%.1f", zombie->getOriginalSpeed());
                        zombie->setSpeed(zombie->getOriginalSpeed()); // 恢复原始速度
                    }
                }
                else {
                    CCLOG("Zombie attack cooldown: %.2f seconds left", zombie->m_attackCounter);
                }

                // 一个僵尸只攻击第一个碰撞到的植物
                break;
            }
            else {
                // 未碰撞到植物，但当前速度为0，恢复速度（避免僵尸卡住）
                if (zombie->getSpeed() <= 0) {
                    CCLOG("Zombie row %d not collide with plant, resume speed to %.1f",
                        zombieRow, zombie->getOriginalSpeed());
                    zombie->setSpeed(zombie->getOriginalSpeed());
                }
            }
        }
    }
}