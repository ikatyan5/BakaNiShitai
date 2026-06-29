// ItemHankachi.cpp
#include "ItemHankachi.h"
#include "Player.h"
#include "Config.h"

void ItemHankachi::OnPickup(Player& player) {
    player.jumpPower = JUMP_POWER * 1.2f;
    player.moveSpeed = 6.0f;
    itemState = ITEM_INACTIVE;
}