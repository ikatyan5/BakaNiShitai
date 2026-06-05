#pragma once
#include "DxLib.h"
#include "WeaponConfig.h"

class ImageManager {
public:
    // プレイヤー画像
    int player1[7];
    int player2[7];

    // UI画像
    int p1Hp[2];
    int p2Hp[2];
    int p1Score[2];
    int p2Score[2];
    int numbers[10];

    // 武器画像
    int kama;
    int konbou;
    int ken;
    int gun;
    int houki;
    int hammer;
    int stick;
    int orbs[3];

    // アイテム
    int potionBlue;
	int potionRed;
    int potionPurple;
    int bomb;

    int weaponImages[WEAPON_TYPE_MAX];
    void Load();
};