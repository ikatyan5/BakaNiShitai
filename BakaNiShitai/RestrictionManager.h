// RestrictionManager.h
#pragma once
#include "RestrictionConfig.h"

const int RESTRICTION_MAX = 3; // 同時にかかる制限の最大数

class RestrictionManager {
public:
    RestrictionType active[RESTRICTION_MAX];
    int activeCount;

    void Init();
    void SelectRandom();         // ランダムで制限を選ぶ
    bool IsActive(RestrictionType type) const; // 制限がかかってるか確認
};