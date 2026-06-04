#pragma once
#include "Item.h"

class ItemPotionRed : public Item {
public:
    ItemPotionRed() : exploding(false), explodeTimer(0), bombImage(-1), ownerID(0) {}

    int blinkTimer;
    bool exploding;
    int explodeTimer;
    int bombImage;
    int ownerID; // 拾ったプレイヤーのID（1か2）

    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
    void Update() override;
    void Draw() override;
    bool CheckExplodeHit(Player& target);
};