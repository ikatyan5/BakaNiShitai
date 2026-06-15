#pragma once
#include "DxLib.h"

class Player;

class Item {
public:
    float x, y;

    enum ItemState {
        ITEM_INACTIVE,
        ITEM_FALLING,
        ITEM_GROUND,
        ITEM_HELD,
        ITEM_EXPLODING,
    };
    ItemState itemState;

    int itemImage;
    int groundTimer = 0; // 接地してから消えるまでの残りフレーム

    virtual void OnPickup(Player& player) = 0;
    virtual void OnUse(Player& player) = 0;
    virtual void Update();
    virtual void Draw();
    virtual ~Item() {}
};