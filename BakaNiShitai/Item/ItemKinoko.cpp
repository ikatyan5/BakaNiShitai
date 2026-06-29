#include "ItemKinoko.h"
#include "Player.h"
void ItemKinoko::OnPickup(Player& player) {
    player.reverseTimer = 60; // 1秒
    itemState = ITEM_INACTIVE;
}