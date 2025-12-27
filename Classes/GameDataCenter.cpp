#include "GameDataCenter.h"

GameDataCenter* GameDataCenter::s_instance = nullptr;

GameDataCenter* GameDataCenter::getInstance() {
    if (!s_instance) {
        s_instance = new GameDataCenter();
    }
    return s_instance;
}

void GameDataCenter::destroyInstance() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

GameDataCenter::~GameDataCenter() {
    clearAll();
}

std::vector<Zombie*>& GameDataCenter::getZombies() {
    return m_zombies;
}

std::vector<Plant*>& GameDataCenter::getPlants() {
    return m_plants;
}

std::vector<Bullet*>& GameDataCenter::getBullets() {
    return m_bullets;
}

void GameDataCenter::addZombie(Zombie* zombie) {
    if (zombie && std::find(m_zombies.begin(), m_zombies.end(), zombie) == m_zombies.end()) {
        m_zombies.push_back(zombie);
    }
}

void GameDataCenter::addPlant(Plant* plant) {
    if (plant && std::find(m_plants.begin(), m_plants.end(), plant) == m_plants.end()) {
        m_plants.push_back(plant);
    }
}

void GameDataCenter::addBullet(Bullet* bullet) {
    if (bullet && std::find(m_bullets.begin(), m_bullets.end(), bullet) == m_bullets.end()) {
        m_bullets.push_back(bullet);
    }
}

void GameDataCenter::removeZombie(Zombie* zombie) {
    auto it = std::find(m_zombies.begin(), m_zombies.end(), zombie);
    if (it != m_zombies.end()) {
        m_zombies.erase(it);
    }
}

void GameDataCenter::removePlant(Plant* plant) {
    auto it = std::find(m_plants.begin(), m_plants.end(), plant);
    if (it != m_plants.end()) {
        m_plants.erase(it);
    }
}

void GameDataCenter::removeBullet(Bullet* bullet) {
    auto it = std::find(m_bullets.begin(), m_bullets.end(), bullet);
    if (it != m_bullets.end()) {
        m_bullets.erase(it);
    }
}

void GameDataCenter::cleanupDeadObjects() {
    // 清理死亡僵尸
    for (auto it = m_zombies.begin(); it != m_zombies.end();) {
        Zombie* zombie = *it;
        if (!zombie || zombie->getIsDead()) {
            // 触发死亡回调
            if (zombie && m_zombieDeathCallback) {
                m_zombieDeathCallback(zombie);
            }

            if (zombie) {
                zombie->removeFromParent();
            }
            it = m_zombies.erase(it);
        }
        else {
            ++it;
        }
    }

    // 清理死亡子弹
    for (auto it = m_bullets.begin(); it != m_bullets.end();) {
        Bullet* bullet = *it;
        if (!bullet || bullet->getIsDead()) {
            if (bullet) {
                bullet->removeFromParent();
            }
            it = m_bullets.erase(it);
        }
        else {
            ++it;
        }
    }

    // 清理死亡植物
    for (auto it = m_plants.begin(); it != m_plants.end();) {
        Plant* plant = *it;
        if (!plant || plant->getIsDead()) {
            if (plant) {
                plant->removeFromParent();
            }
            it = m_plants.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GameDataCenter::clearAll() {
    // 清理所有对象
    for (auto zombie : m_zombies) {
        if (zombie) {
            zombie->removeFromParent();
        }
    }
    m_zombies.clear();

    for (auto plant : m_plants) {
        if (plant) {
            plant->removeFromParent();
        }
    }
    m_plants.clear();

    for (auto bullet : m_bullets) {
        if (bullet) {
            bullet->removeFromParent();
        }
    }
    m_bullets.clear();
}

void GameDataCenter::reset() {
    clearAll();
}