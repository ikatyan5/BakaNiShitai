#include "Weapon.h"
#include "WeaponConfig.h"
#include "Config.h"
#include <cmath>

void Weapon::Init(WeaponType type, ImageManager& imgMgr) {
    weaponState = WEAPON_INACTIVE;  // active = false ‚Ì‘ã‚í‚è
    angle = 0.0f;
    x = y = vx = 0.0f;
    ownerID = 0;
    weaponType = type;
    groundTimer = 0;
    selfHitTimer = 0;
    parryRemain = WEAPON_DATA[type].parryCount;
    weaponImage = imgMgr.weaponImages[type];
}

void Weapon::Throw(float startX, float startY, bool facingRight, int id, WeaponType type, ImageManager& imgMgr) {
    x = startX;
    y = startY;
    vx = facingRight ? WEAPON_SPEED : -WEAPON_SPEED;
    angle = 0.0f;
    selfHitTimer = 20;
    weaponState = WEAPON_THROWN;  // active = true ‚Ì‘ã‚í‚è
    parryRemain = WEAPON_DATA[type].parryCount;
    ownerID = id;
    weaponType = type;
}

void Weapon::Update() {
    if (weaponState == WEAPON_FALLING) {
        y += 5.0f;
        if (y >= GROUND_Y - WEAPON_GROUND_OFFSET) {
            y = GROUND_Y - WEAPON_GROUND_OFFSET;
            groundTimer++;
            if (groundTimer >= WEAPON_GROUND_TIME) {
                weaponState = WEAPON_INACTIVE;
                groundTimer = 0;
            }
        }
    }
    else if (weaponState == WEAPON_THROWN) {
        if (selfHitTimer > 0) selfHitTimer--;
        x += vx;
        angle += (vx > 0) ? WEAPON_ROTATE : -WEAPON_ROTATE; // Œü‚«‚Å‰ñ“]•ûŒü•Ï‚¦‚é
        if (x < 0 || x > 1280) weaponState = WEAPON_INACTIVE;
    }
}

bool Weapon::CheckHit(float bx, float by, float bw, float bh, int targetID) {
    if (weaponState != WEAPON_THROWN) return false;
    if (targetID == ownerID && selfHitTimer > 0) return false; // —P—\’†‚ÍŽ©•ª‚É“–‚½‚ç‚È‚¢
    float aw = WEAPON_DATA[weaponType].hitW;
    float ah = WEAPON_DATA[weaponType].hitH;
    return fabsf(x - bx) < (aw + bw) / 2 &&
        fabsf(y - by) < (ah + bh) / 2;
}

bool Weapon::CheckParry(float bx, float by, float bw, float bh) {
    if (weaponState != WEAPON_THROWN) return false;
    float aw = WEAPON_DATA[weaponType].parryW;
    float ah = WEAPON_DATA[weaponType].parryH;
    return fabsf(x - bx) < (aw + bw) / 2 &&
        fabsf(y - by) < (ah + bh) / 2;
}

void Weapon::Draw() {
    if (weaponState == WEAPON_INACTIVE) return;
    if (weaponState == WEAPON_HELD) return;
    double scale = (weaponState == WEAPON_FALLING) ? 1.0 : 1.5;
    bool flipX = (vx < 0); // ¶‚É”ò‚ñ‚Å‚é‚Æ‚«”½“]
    DrawRotaGraphF(x, y, scale, angle, weaponImage, TRUE, flipX);
#ifdef _DEBUG
    float hw = WEAPON_DATA[weaponType].hitW / 2;
    float hh = WEAPON_DATA[weaponType].hitH / 2;
    DrawBoxAA(x - hw, y - hh, x + hw, y + hh, GetColor(0, 255, 255), FALSE);
#endif
}