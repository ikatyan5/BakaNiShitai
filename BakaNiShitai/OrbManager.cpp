// OrbManager.cpp
#include "OrbManager.h"
#include "Player.h"
#include "Config.h"

void OrbManager::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    hitOccurred = false;
    hitWinnerID = 0;
    for (int i = 0; i < ORB_MAX; i++) {
        orbs[i].active = false;
    }
}

void OrbManager::Launch(float x, float y, bool facingRight, int ownerID) {
    for (int i = 0; i < ORB_MAX; i++) {
        if (!orbs[i].active) {
            int colorIndex = rand() % 3;
            orbs[i].Launch(x, y, facingRight, ownerID, imgMgr->orbs[colorIndex], colorIndex);
            return;
        }
    }
}

void OrbManager::Update(Player& player1, Player& player2) {
    hitOccurred = false;

    for (int i = 0; i < ORB_MAX; i++) {
        if (!orbs[i].active) continue;
        orbs[i].Update();

        auto checkHit = [&](Player& target, int winnerID) {
            if (!orbs[i].active) return;
            if (orbs[i].ownerID == target.PlayerID) return; // Ž©•ª‚É‚Í“–‚½‚ç‚È‚¢
            if (orbs[i].CheckHit(target.x, target.y - PLAYER_HIT_CY, PLAYER_HIT_W, PLAYER_HIT_H)) {
                orbs[i].active = false;
                hitOccurred = true;
                hitWinnerID = winnerID;
            }
            };

        checkHit(player1, 2);
        checkHit(player2, 1);
    }
}

void OrbManager::CheckParry(float atkX, float atkY, float atkW, float atkH, int ownerID) {
    for (int i = 0; i < ORB_MAX; i++) {
        if (!orbs[i].active) continue;
        if (orbs[i].ownerID == ownerID) continue; // Ž©•ª‚ÌƒI[ƒu‚Í‚Í‚½‚©‚È‚¢
        if (orbs[i].CheckParry(atkX, atkY, atkW, atkH)) {
            orbs[i].active = false;
        }
    }
}

void OrbManager::Draw() {
    for (int i = 0; i < ORB_MAX; i++) {
        orbs[i].Draw();
    }
}