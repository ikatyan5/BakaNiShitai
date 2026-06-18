// Orb.cpp
#include "Orb.h"
#include "Config.h"
#include "Player.h"
#include <cmath>

const float ORB_HIT_W = 48.0f;
const float ORB_HIT_H = 48.0f;

// 色ごとの速度（赤・青・緑）
const float ORB_SPEEDS[3] = { 14.0f, 10.0f, 6.0f };

void Orb::Launch(float startX, float startY, bool facingRight, int id, int image, int colorIndex) {
    x = startX;
    y = startY;
    float speed = ORB_SPEEDS[colorIndex];
    vx = facingRight ? speed : -speed;
    angle = 0.0f;
    ownerID = id;
    orbImage = image;
    active = true;
}

void Orb::Update() {
    if (!active) return;
    x += vx;
    angle += (vx > 0) ? WEAPON_ROTATE : -WEAPON_ROTATE;
    if (x < 0 || x > SCREEN_W) active = false;
}

void Orb::Draw() {
    if (!active) return;
    DrawRotaGraphF(x, y, 1.5, angle, orbImage, TRUE);
}

bool Orb::CheckHit(float bx, float by, float bw, float bh) {
    if (!active) return false;
    return fabsf(x - bx) < (ORB_HIT_W + bw) / 2 &&
        fabsf(y - by) < (ORB_HIT_H + bh) / 2;
}

bool Orb::CheckParry(float bx, float by, float bw, float bh) {
    if (!active) return false;
    return fabsf(x - bx) < (ORB_HIT_W + bw) / 2 &&
        fabsf(y - by) < (ORB_HIT_H + bh) / 2;
}