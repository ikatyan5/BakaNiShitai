#include "AdManager.h"
#include "Config.h"
#include <cmath>

void AdManager::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    for (int i = 0; i < AD_MAX; i++) {
        slots[i].active = false;
    }
    spawnTimer = AD_SPAWN_MIN + rand() % (AD_SPAWN_MAX - AD_SPAWN_MIN);
}

void AdManager::Update() {
    for (int i = 0; i < AD_MAX; i++) {
        if (!slots[i].active) continue;

        // 移動
        slots[i].x += slots[i].vx;
        slots[i].y += slots[i].vy;

        // 端で跳ね返り
        if (slots[i].x < 0.0f) {
            slots[i].x = 0.0f;
            slots[i].vx = fabsf(slots[i].vx);
        }
        if (slots[i].x + AD_W > SCREEN_W) {
            slots[i].x = SCREEN_W - AD_W;
            slots[i].vx = -fabsf(slots[i].vx);
        }
        if (slots[i].y < 0.0f) {
            slots[i].y = 0.0f;
            slots[i].vy = fabsf(slots[i].vy);
        }
        if (slots[i].y + AD_H > SCREEN_H) {
            slots[i].y = SCREEN_H - AD_H;
            slots[i].vy = -fabsf(slots[i].vy);
        }

        // タイマー
        slots[i].timer--;
        if (slots[i].timer <= 0) {
            slots[i].active = false;
        }
    }

    spawnTimer--;
    if (spawnTimer <= 0) {
        for (int i = 0; i < AD_MAX; i++) {
            if (!slots[i].active) {
                slots[i].active = true;
                slots[i].imageIndex = rand() % 3;
                slots[i].x = (float)(rand() % (SCREEN_W - AD_W));
                slots[i].y = (float)(rand() % (SCREEN_H - AD_H));
                // ランダムな方向に速度設定
                slots[i].vx = (rand() % 2 == 0) ? 6.0f : -6.0f;
                slots[i].vy = (rand() % 2 == 0) ? 6.0f : -6.0f;
                slots[i].timer = AD_DURATION;
                break;
            }
        }
        spawnTimer = AD_SPAWN_MIN + rand() % (AD_SPAWN_MAX - AD_SPAWN_MIN);
    }
}

void AdManager::Draw() {
    for (int i = 0; i < AD_MAX; i++) {
        if (!slots[i].active) continue;
        DrawExtendGraphF(
            slots[i].x, slots[i].y,
            slots[i].x + AD_W, slots[i].y + AD_H,
            imgMgr->adImages[slots[i].imageIndex], TRUE
        );
    }
}