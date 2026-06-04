#include "ItemPotionPurple.h"
#include "Player.h"
#include "Config.h"

void ItemPotionPurple::OnPickup(Player& player) {
    effectX = player.x;
    effectY = player.y - PLAYER_HIT_CY;
    player.x = (float)(rand() % 1100 + 90);
    player.y = GROUND_Y - 100.0f;

    effectX = player.x; // ワープ後の座標を保存！
    effectY = player.y - PLAYER_HIT_CY;

    effectTimer = 40;
    itemState = ITEM_EXPLODING;
}

void ItemPotionPurple::Update() {
    if (itemState == ITEM_EXPLODING) {
        effectTimer--;
        if (effectTimer <= 0) {
            itemState = ITEM_INACTIVE;
        }
    }
    else {
        Item::Update();
    }
}

void ItemPotionPurple::Draw() {
    if (itemState == ITEM_INACTIVE) return;
    if (itemState == ITEM_HELD) return;
    if (itemState == ITEM_EXPLODING) {
        float radius = (40 - effectTimer) * 5.0f; // 小さい円から大きくなる
        DrawCircleAA(effectX, effectY, radius, 64, GetColor(180, 0, 255), FALSE);
    }
    else {
        DrawRotaGraphF(x, y, 1.0, 0.0, itemImage, TRUE);
    }
}