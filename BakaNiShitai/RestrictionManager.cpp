// RestrictionManager.cpp
#pragma once
#include "RestrictionManager.h"
#include "DebugConfig.h"
#include <cstdlib>

void RestrictionManager::Init() {
    activeCount = 0;
    for (int i = 0; i < RESTRICTION_MAX; i++) {
        active[i] = REST_NONE;
    }
}

void RestrictionManager::SelectRandom() {
    Init();

#ifdef _DEBUG
    if (DBG_FORCE_RESTRICTION) {
        active[0] = DBG_RESTRICTION_TYPE;
        activeCount = 1;
        return;
    }
#endif

    RestrictionType picked = (RestrictionType)(rand() % (REST_TYPE_MAX - 1) + 1);
    active[0] = picked;
    activeCount = 1;
}

bool RestrictionManager::IsActive(RestrictionType type) const {
    for (int i = 0; i < activeCount; i++) {
        if (active[i] == type) return true;
    }
    return false;
}