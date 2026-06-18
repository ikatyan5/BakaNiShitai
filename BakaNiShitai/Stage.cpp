#include "Stage.h"
#include "Config.h"

void Stage::Init(int id) {
    stageID = id;
    platformCount = 0;

    if (id == 1) {
        // 一本道 地面だけ
        platforms[0] = { 0, 800.0f, SCREEN_W, 40.0f };
        platformCount = 1;
    }
    else if (id == 2) {
        // 地面 + 中央に長めの足場
        platforms[0] = { 0, 600.0f, SCREEN_W, 40.0f };
        platforms[1] = { 440.0f, 420.0f, 400.0f, 20.0f };
        platformCount = 2;
    }
    else if (id == 3) {
        // 地面 + 足場3つ
        platforms[0] = { 0, 600.0f, SCREEN_W, 40.0f };
        platforms[1] = { 200.0f, 450.0f, 250.0f, 20.0f };
        platforms[2] = { 515.0f, 350.0f, 250.0f, 20.0f };
        platforms[3] = { 830.0f, 450.0f, 250.0f, 20.0f };
        platformCount = 4;
    }
}

void Stage::Draw(){
    for (int i = 0; i < platformCount; i++) {
        // 塗りつぶしなし・白チョーク色で枠だけ
        DrawBoxAA(
            platforms[i].x, platforms[i].y,
            platforms[i].x + platforms[i].w,
            platforms[i].y + platforms[i].h,
            GetColor(220, 220, 200), FALSE
        );
    }
}

bool Stage::CheckLanding(float px, float py, float prevPy, float pw, float ph, float& outY) {
    for (int i = 0; i < platformCount; i++) {
        Platform& p = platforms[i];
        if (px + pw / 2 < p.x || px - pw / 2 > p.x + p.w) continue;
        if (prevPy <= p.y && py >= p.y) { // 地面をまたいだか
            outY = p.y;
            return true;
        }
    }
    return false;
}