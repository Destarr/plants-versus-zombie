// SunEvents.h
#pragma once
#include "cocos2d.h"

// 阳光事件类型
const std::string EVENT_SUN_ADDED = "sun_added_event";
const std::string EVENT_SUN_CONSUMED = "sun_consumed_event";

// 阳光增加事件
class SunAddedEvent : public cocos2d::EventCustom {
public:
    SunAddedEvent(int amount)
        : EventCustom(EVENT_SUN_ADDED)
        , amount(amount) {
    }

    int amount;
};