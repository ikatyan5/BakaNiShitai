// ItemPotionBlue.h
#pragma once
#include "Item.h"

class ItemPotionBlue : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}  // 即発動系なのでOnUseは空でOK
};