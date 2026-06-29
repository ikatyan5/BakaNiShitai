// OrbManager.h
#pragma once
#include "Orb.h"
#include "ImageManager.h"

class Player;

const int ORB_MAX = 10;

// 杖から出るオーブをまとめて管理する係（発射・更新・当たり判定）

class OrbManager {
public:
    void Init(ImageManager& imgMgr);
    void Update(Player& player1, Player& player2);
    void Draw();
    void Launch(float x, float y, bool facingRight, int ownerID);
    void CheckParry(float atkX, float atkY, float atkW, float atkH, int ownerID);

    bool hitOccurred;
    int hitWinnerID;

private:
    Orb orbs[ORB_MAX];
    ImageManager* imgMgr;
};