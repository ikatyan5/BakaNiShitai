// ItemPotionYellow.h
#pragma once
#include "Item.h"

class ItemPotionYellow : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};