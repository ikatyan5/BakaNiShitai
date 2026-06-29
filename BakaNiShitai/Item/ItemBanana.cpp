#include "ItemBanana.h"
#include "Player.h"
void ItemBanana::OnPickup(Player& player) {
    player.EnterStun();
    itemState = ITEM_INACTIVE;
}