// ItemHankachi.h
#pragma once
#include "Item.h"

class ItemHankachi : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};