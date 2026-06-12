#include "ItemKinoko.h"
#include "Player.h"
void ItemKinoko::OnPickup(Player& player) {
    player.reverseTimer = 60; // 1•b
    itemState = ITEM_INACTIVE;
}