#include "Weapon.h"
#include "WeaponConfig.h"
#include "Config.h"
#include <cmath>

void Weapon::Init(WeaponType type, ImageManager& imgMgr) {
    weaponState = WEAPON_INACTIVE;  // active = false の代わり
    angle = 0.0f;
    x = y = vx = 0.0f;
    vy = 0.0f;
    throwGravity = 0.0f;
    ownerID = 0;
    weaponType = type;
    groundTimer = 0;
    selfHitTimer = 0;
    orbFired = false; 
    tensaiFired = false;
    boomerangDecel = 0.0f;
    boomerangReturning = false;
    parryRemain = WEAPON_DATA[type].parryCount;
    weaponImage = imgMgr.weaponImages[type];
}

void Weapon::Throw(float startX, float startY, bool facingRight, int id, WeaponType type, ImageManager& imgMgr) {
    x = startX;
    y = startY;
    vx = facingRight ? WEAPON_SPEED : -WEAPON_SPEED;
    vy = 0.0f;
    throwGravity = 0.0f;
    if (type == WEAPON_BOOMERANG) {
        vx = facingRight ? 16.0f : -16.0f;
        boomerangDecel = facingRight ? 0.3f : -0.3f;
        boomerangReturning = rand() % 2 == 0; // ランダムで挙動選択！
    }
    angle = 0.0f;
    selfHitTimer = 20;
    weaponState = WEAPON_THROWN;  // active = true の代わり
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

        // ブーメラン処理
        if (weaponType == WEAPON_BOOMERANG) {
            vx -= boomerangDecel;
            if (!boomerangReturning) {
                if ((boomerangDecel > 0 && vx <= 0) || (boomerangDecel < 0 && vx >= 0)) {
                    boomerangReturning = true;
                    boomerangDecel = -boomerangDecel; // 加速方向反転
                    vx = boomerangDecel * 0.1f; // 折り返し初速をほぼゼロから
                }
            }
        }

        x += vx;
        y += vy;
        vy += throwGravity;
        angle += (vx > 0) ? WEAPON_ROTATE : -WEAPON_ROTATE; // 向きで回転方向変える
        
        // 消滅判定
        if (x < 0 || x > 1280) weaponState = WEAPON_INACTIVE;
        else if (y < 0)weaponState = WEAPON_INACTIVE;
        else if (y >= GROUND_Y)weaponState = WEAPON_INACTIVE;

    }
}

bool Weapon::CheckHit(float bx, float by, float bw, float bh, int targetID) {
    if (weaponState != WEAPON_THROWN) return false;
    if (targetID == ownerID && selfHitTimer > 0) return false; // 猶予中は自分に当たらない
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
    bool flipX = (vx < 0); // 左に飛んでるとき反転
    DrawRotaGraphF(x, y, scale, angle, weaponImage, TRUE, flipX);
#ifdef _DEBUG
    float hw = WEAPON_DATA[weaponType].hitW / 2;
    float hh = WEAPON_DATA[weaponType].hitH / 2;
    DrawBoxAA(x - hw, y - hh, x + hw, y + hh, GetColor(0, 255, 255), FALSE);
#endif
}