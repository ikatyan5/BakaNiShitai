// ItemPotionYellow.cpp
#include "ItemPotionYellow.h"
#include "Player.h"

void ItemPotionYellow::OnPickup(Player& player) {
    player.isGlowing = true;
    itemState = ITEM_INACTIVE;
}