// RestrictionManager.h
#pragma once
#include "RestrictionConfig.h"

const int RESTRICTION_MAX = 3; // 同時にかかる制限の最大数

class RestrictionManager {
public:
    RestrictionType active[RESTRICTION_MAX];
    int activeCount;
    bool used[REST_TYPE_MAX]; // この試合で既に出した妨害（同じものを繰り返さないための履歴）

    void Init();                 // 試合開始時。activeと履歴を全部リセットする
    void SelectRandom();         // この試合でまだ出ていない妨害からランダムで選ぶ
    bool IsActive(RestrictionType type) const; // 制限がかかってるか確認
};