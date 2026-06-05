#include "ItemPotionRed.h"
#include "Player.h"
#include "Config.h"
#include <cmath>

void ItemPotionRed::OnPickup(Player& player) {
    player.isBlinking = true;
    player.blinkTimer = 180; // 3秒くらい
    player.canAttack = false;
    ownerID = player.PlayerID;
    itemState = ITEM_INACTIVE;
}

void ItemPotionRed::Update() {
    if (exploding && itemState == ITEM_EXPLODING) {
        explodeTimer -= 3;
        if (explodeTimer <= 0) {
            itemState = ITEM_INACTIVE;
        }
    }
    else {
        Item::Update();
    }
}

void ItemPotionRed::Draw() {
    if (itemState == ITEM_INACTIVE) return;
    if (itemState == ITEM_HELD) return;
    if (exploding) {
        DrawRotaGraphF(x, y, 9.0, 0.0, bombImage, TRUE);
        DrawCircleAA(x, y, 150.0f, 64, GetColor(0, 255, 255), FALSE);
    }
    else {
        // 通常時はポーション赤の絵
        DrawRotaGraphF(x, y, 1.0, 0.0, itemImage, TRUE);
    }
}

bool ItemPotionRed::CheckExplodeHit(Player& target) {
    if (!exploding) return false;
    if (explodeTimer > 40) return false;
    float range = 150.0f;
    // プレイヤーの当たり判定の一番近い点を求める
    float closestX = max(target.x - PLAYER_HIT_W / 2, min(x, target.x + PLAYER_HIT_W / 2));
    float closestY = max((target.y - PLAYER_HIT_CY) - PLAYER_HIT_H / 2, min(y, (target.y - PLAYER_HIT_CY) + PLAYER_HIT_H / 2));
    float dx = x - closestX;
    float dy = y - closestY;
    return dx * dx + dy * dy < range * range;
}