#pragma once
#include "Item.h"
class ItemKinoko : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};