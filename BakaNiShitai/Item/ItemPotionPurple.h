#pragma once
#include "Item.h"

// 紫ポーション
// ランダムな位置にワープする

class ItemPotionPurple : public Item {
public:
    ItemPotionPurple() : effectTimer(0), effectX(0.0f), effectY(0.0f) {}
    int effectTimer;
    float effectX, effectY;
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
    void Update() override;
    void Draw() override;
};