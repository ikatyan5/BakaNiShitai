// ItemHankachi.h
#pragma once
#include "Item.h"

// ハンカチ
// 拾うと移動速度・ジャンプ力が上がる

class ItemHankachi : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};