#pragma once
#include "Item.h"

// バナナの皮
// 拾うとスタンする

class ItemBanana : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};