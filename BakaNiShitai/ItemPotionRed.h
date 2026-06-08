#pragma once
#include "Item.h"

class ItemPotionRed : public Item {
public:
    enum ExplodePhase {
        EXPLODE_NONE,
        EXPLODE_WINDUP,  // 爆発前の溜め（判定なし）
        EXPLODE_ACTIVE,  // 判定あり
    };

    ItemPotionRed() : exploding(false), explodeTimer(0), bombImage(-1), ownerID(0) {}

    bool exploding;
    int explodeTimer;
    int bombImage;
    int ownerID; // 拾ったプレイヤーのID（1か2）
    ExplodePhase explodePhase;

    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
    void Update() override;
    void Draw() override;
    bool CheckExplodeHit(Player& target);
};