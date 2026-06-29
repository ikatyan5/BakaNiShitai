#pragma once
#include "Item.h"

// 毒キノコ
// 拾うと操作が反転する

class ItemKinoko : public Item {
public:
    void OnPickup(Player& player) override;
    void OnUse(Player& player) override {}
};