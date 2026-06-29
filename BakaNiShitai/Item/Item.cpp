#include "Item.h"
#include "Config.h"

void Item::Update() {
    if (itemState == ITEM_FALLING) {
        y += 5.0f;
        if (y >= GROUND_Y - 40.0f) {
            y = GROUND_Y - 40.0f;
            itemState = ITEM_GROUND;
            groundTimer = WEAPON_GROUND_TIME; // 接地したら消滅カウント開始（武器と同じ時間）
        }
    }
    else if (itemState == ITEM_GROUND) {
        // 一定時間拾われなければ消える
        groundTimer--;
        if (groundTimer <= 0) {
            itemState = ITEM_INACTIVE;
        }
    }
}

void Item::Draw() {
    if (itemState == ITEM_INACTIVE) return;
    if (itemState == ITEM_HELD) return;
    DrawRotaGraphF(x, y, 1.2, 0.0, itemImage, TRUE);
}