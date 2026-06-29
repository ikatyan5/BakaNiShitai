// ItemPotionBlue.h
#pragma once
#include "Item.h"

// 青ポーション
// ジャンプ力が上がる

class ItemPotionBlue : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};