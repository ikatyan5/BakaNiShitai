// ItemPotionYellow.h
#pragma once
#include "Item.h"

// 黄ポーション
// プレイヤーの見た目が変わるだけ

class ItemPotionYellow : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};