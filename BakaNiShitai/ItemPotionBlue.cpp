// ItemPotionBlue.cpp

// ジャンプ力が上がる
// 1.8倍にするよ！

#include "ItemPotionBlue.h"
#include "Player.h"
#include "Config.h"

void ItemPotionBlue::OnPickup(Player& player) {
    player.jumpPower = JUMP_POWER * 1.8f;  // ジャンプ力1.8倍！
    itemState = ITEM_INACTIVE;
}