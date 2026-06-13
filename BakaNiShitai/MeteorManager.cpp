// MeteorManager.cpp
#include "MeteorManager.h"
#include "Player.h"
#include "Config.h"
#include <cmath>
#include <cstdlib>

void MeteorManager::Init() {
    hitOccurred = false;
    hitWinnerID = 0;
    spawnTimer = 0;
    elapsedFrames = 0;
    edgeSpawnTimer = 0;
    edgeSpawnLeft = true;
    tensaiSpawnTimer = 0;
    tensaiSpawnCount = 0;
    for (int i = 0; i < METEOR_MAX; i++) {
        meteors[i].Init();
    }
}

float MeteorManager::GetCurrentSpeed() {
    float t = elapsedFrames / 1800.0f;
    if (t > 1.0f) t = 1.0f;
    return 4.0f + t * 18.0f;
}

int MeteorManager::GetCurrentInterval() {
    float t = elapsedFrames / 1800.0f;
    if (t > 1.0f) t = 1.0f;
    return (int)(180.0f - t * 40.0f);
}

bool MeteorManager::HasActiveMeteor() const {
    for (int i = 0; i < METEOR_MAX; i++) {
        if (meteors[i].active) return true;
    }
    return false;
}

void MeteorManager::Spawn(float targetX, float speed) {
    for (int i = 0; i < METEOR_MAX; i++) {
        if (!meteors[i].active) {
            float startX = (float)(rand() % 1280);
            float startY = 0.0f;
            float s = (speed < 0.0f) ? GetCurrentSpeed() : speed;
            float dx = targetX - startX;
            float dy = 800.0f - startY;
            float dist = sqrtf(dx * dx + dy * dy);
            meteors[i].x = startX;
            meteors[i].y = startY;
            meteors[i].vx = (dx / dist) * s;
            meteors[i].vy = (dy / dist) * s;
            meteors[i].targetX = targetX;
            meteors[i].targetY = 800.0f;
            meteors[i].active = true;
            return;
        }
    }
}

void MeteorManager::SpawnTensai() {
    tensaiSpawnCount = 5;
    tensaiSpawnTimer = 1;
}

void MeteorManager::Update(Player& player1, Player& player2, bool tensaiMode) {
    hitOccurred = false;
    elapsedFrames++;

    if (tensaiSpawnCount > 0) {
        if (tensaiSpawnTimer > 0) tensaiSpawnTimer--;
        else {
            Spawn((float)(rand() % 1100 + 90), 18.0f);
            tensaiSpawnCount--;
            tensaiSpawnTimer = 6;
        }
    }

    // テンサイモードのときは通常スポーンをスキップ
    if (!tensaiMode) {
        spawnTimer++;
        if (spawnTimer >= GetCurrentInterval()) {
            spawnTimer = 0;
            Spawn((float)(rand() % 1100 + 90));
        }
        edgeSpawnTimer++;
        if (edgeSpawnTimer >= 150) {
            edgeSpawnTimer = 0;
            float targetX = edgeSpawnLeft
                ? (float)(rand() % 200)
                : (float)(rand() % 200 + 1080);
            edgeSpawnLeft = !edgeSpawnLeft;
            Spawn(targetX);
        }
    }

    for (int i = 0; i < METEOR_MAX; i++) {
        if (!meteors[i].active) continue;
        meteors[i].Update();
        auto checkHit = [&](Player& target, int winnerID) {
            if (!meteors[i].active) return;
            float dx = meteors[i].x - target.x;
            float dy = meteors[i].y - (target.y - PLAYER_HIT_CY);
            if (fabsf(dx) < (96.0f + PLAYER_HIT_W) / 2 &&
                fabsf(dy) < (96.0f + PLAYER_HIT_H) / 2) {
                hitOccurred = true;
                hitWinnerID = winnerID;
            }
            };
        checkHit(player1, 2);
        if (!hitOccurred) checkHit(player2, 1);
    }
}

void MeteorManager::Draw(ImageManager& imgMgr) {
    for (int i = 0; i < METEOR_MAX; i++) {
        if (!meteors[i].active) continue;


        // 予告マーカー（地面に丸）
        DrawCircleAA(meteors[i].targetX, 800.0f, 20.0f, 16,
            GetColor(255, 100, 0), FALSE);

        // 隕石本体
        DrawRotaGraphF(
            meteors[i].x, meteors[i].y,
            4.0, meteors[i].angle,
            imgMgr.meteo, TRUE, FALSE
        );
    }
}