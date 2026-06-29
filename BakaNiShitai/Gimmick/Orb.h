// Orb.h
#pragma once
#include "DxLib.h"

class Player;

// 杖から撃ち出される弾（オーブ）1発ぶん。飛んで、当たり判定とパリィ判定を持つ

class Orb {
public:
    float x, y;
    float vx;
    float angle;
    int orbImage;
    int ownerID;
    bool active;

    void Launch(float startX, float startY, bool facingRight, int id, int image, int colorIndex);
    void Update();
    void Draw();
    bool CheckHit(float bx, float by, float bw, float bh);
    bool CheckParry(float bx, float by, float bw, float bh);
};