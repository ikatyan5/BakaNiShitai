#pragma once
#include "Item.h"
class ItemBanana : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};