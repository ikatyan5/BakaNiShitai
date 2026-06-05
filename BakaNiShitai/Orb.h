// Orb.h
#pragma once
#include "DxLib.h"

class Player;

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