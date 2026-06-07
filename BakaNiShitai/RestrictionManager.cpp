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

    // REST_NONEとREST_TYPE_MAXを除いた範囲からランダムに選ぶ
    // 同じ制限が重複しないようにする
    int attempts = 0;
    while (activeCount < RESTRICTION_MAX && attempts < 100) {
        attempts++;
        RestrictionType picked = (RestrictionType)(rand() % (REST_TYPE_MAX - 1) + 1);

        // 重複チェック
        bool duplicate = false;
        for (int i = 0; i < activeCount; i++) {
            if (active[i] == picked) { duplicate = true; break; }
        }
        if (!duplicate) {
            active[activeCount++] = picked;
        }
    }
}

bool RestrictionManager::IsActive(RestrictionType type) const {
    for (int i = 0; i < activeCount; i++) {
        if (active[i] == type) return true;
    }
    return false;
}