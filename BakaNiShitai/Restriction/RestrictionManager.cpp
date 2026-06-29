// RestrictionManager.cpp
#include "RestrictionManager.h"
#include "DebugConfig.h"
#include <cstdlib>

RestrictionManager::~RestrictionManager() {
    delete activeRestriction;
}

// active配列（横断的な IsActive 用）と activeRestriction（Strategy 本体）をまとめて差し替える。
void RestrictionManager::SetActive(RestrictionType type) {
    active[0] = type;
    activeCount = 1;
    delete activeRestriction;
    activeRestriction = CreateRestriction(type);
}

void RestrictionManager::Init() {
    activeCount = 0;
    for (int i = 0; i < RESTRICTION_MAX; i++) {
        active[i] = REST_NONE;
    }
    // 試合開始なので「出した妨害」の履歴も全部消す
    for (int i = 0; i < REST_TYPE_MAX; i++) {
        used[i] = false;
    }
    delete activeRestriction;
    activeRestriction = nullptr;
}

void RestrictionManager::SelectRandom() {
    // activeだけリセット。used履歴は試合中ずっと保持する（Initは呼ばない）
    activeCount = 0;
    for (int i = 0; i < RESTRICTION_MAX; i++) {
        active[i] = REST_NONE;
    }

#ifdef _DEBUG
    if (DBG_FORCE_RESTRICTION) {
        SetActive(DBG_RESTRICTION_TYPE);
        return;
    }
#endif

    // この試合でまだ出ていない妨害だけを候補にする
    RestrictionType candidates[REST_TYPE_MAX];
    int candidateCount = 0;
    for (int t = REST_NONE + 1; t < REST_TYPE_MAX; t++) {
        if (!used[t]) {
            candidates[candidateCount++] = (RestrictionType)t;
        }
    }

    // 全種類出し切ったときの保険。履歴をリセットして全部を候補に戻す
    if (candidateCount == 0) {
        for (int i = 0; i < REST_TYPE_MAX; i++) used[i] = false;
        for (int t = REST_NONE + 1; t < REST_TYPE_MAX; t++) {
            candidates[candidateCount++] = (RestrictionType)t;
        }
    }

    RestrictionType picked = candidates[rand() % candidateCount];
    used[picked] = true;
    SetActive(picked);
}

bool RestrictionManager::IsActive(RestrictionType type) const {
    for (int i = 0; i < activeCount; i++) {
        if (active[i] == type) return true;
    }
    return false;
}

const TCHAR* RestrictionManager::ActiveName() const {
    return activeRestriction ? activeRestriction->Name() : _T("");
}
