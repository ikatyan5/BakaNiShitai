// RestrictionManager.h
#pragma once
#include "RestrictionConfig.h"
#include "Restriction.h"

const int RESTRICTION_MAX = 3; // 同時にかかる制限の最大数

// いま発生している妨害(Restriction)を1つ持つ係
// ランダム抽選と「同じ試合では二度出さない」履歴の管理を担当する

class RestrictionManager {
public:
    RestrictionType active[RESTRICTION_MAX];
    int activeCount;
    bool used[REST_TYPE_MAX]; // この試合で既に出した妨害（同じものを繰り返さないための履歴）

    RestrictionManager() = default;
    ~RestrictionManager();
    // activeRestriction を所有する（new で確保する）ため、コピーは禁止して二重 delete を防ぐ。
    RestrictionManager(const RestrictionManager&) = delete;
    RestrictionManager& operator=(const RestrictionManager&) = delete;

    void Init();                 // 試合開始時。activeと履歴を全部リセットする
    void SelectRandom();         // この試合でまだ出ていない妨害からランダムで選ぶ
    bool IsActive(RestrictionType type) const; // 制限がかかってるか確認

    // 現在かかっている妨害（Strategy）。挙動・描画・説明文はこのオブジェクトに委譲する。
    Restriction* Active() const { return activeRestriction; }
    const TCHAR* ActiveName() const;

private:
    Restriction* activeRestriction = nullptr;
    void SetActive(RestrictionType type); // active配列と activeRestriction をまとめて差し替える
};
