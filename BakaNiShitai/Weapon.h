#pragma once
#include "DxLib.h"
#include "WeaponConfig.h"
#include "ImageManager.h"

class Weapon {
public:
    float x, y;
    float vx;
    float vy;
    float throwGravity;
    float angle;
    int weaponType;
    int ownerID; 

    enum WeaponState {
        WEAPON_INACTIVE, // 非アクティブ
        WEAPON_FALLING,  // 空から降ってくる
        WEAPON_HELD,     // 持ってる
        WEAPON_THROWN,   // 投げた
    };
    WeaponState weaponState;

    void Init(WeaponType type, ImageManager& imgMgr);
    void Throw(float startX, float startY, bool facingRight, int id, WeaponType type, ImageManager& imgMgr);
    void Update();
    bool CheckHit(float bx, float by, float bw, float bh, int targetID);
    bool CheckParry(float bx, float by, float bw, float bh);
    void Draw();


    int weaponImage;
    int groundTimer;
    bool orbFired;
    bool tensaiFired;
    int parryRemain;        // 残りはたき落とし回数
    int selfHitTimer;       // 投げた直後の自分への当たり判定猶予
    bool boomerangReturning;
    float boomerangDecel;   // ブーメランの減速値

};