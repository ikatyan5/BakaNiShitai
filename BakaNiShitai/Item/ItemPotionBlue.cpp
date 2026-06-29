// ItemPotionBlue.cpp


#include "ItemPotionBlue.h"
#include "Player.h"
#include "Config.h"

void ItemPotionBlue::OnPickup(Player& player) {
    player.jumpPower = JUMP_POWER * 1.5f;  // ジャンプ力1.5倍
    itemState = ITEM_INACTIVE;
}