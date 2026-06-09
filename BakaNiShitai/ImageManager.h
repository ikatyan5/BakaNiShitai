#pragma once
#include "DxLib.h"
#include "WeaponConfig.h"

class ImageManager {
public:
    // プレイヤー画像
    int player1[7];
    int player2[7];
    int player3[7];

    // UI画像
    int p1Hp[2];
    int p2Hp[2];
    int p3Hp[2];
    int p1Score[2];
    int p2Score[2];
    int p3Score[2];
    int numbers[10];
    int setsunaP1[2];
    int setsunaP2[2];
    int surpMark;

    // 武器画像
    int kama;
    int konbou;
    int ken;
    int gun;
    int houki;
    int hammer;
    int stick;
    int boomerang;
    int memento_mori;
    int pikohan;
    int orbs[3];
    int meteo;

    // エフェクト
    int mementoMoriEffect;
    int shield;
    int stan[2];

    // アイテム
    int potionBlue;
	int potionRed;
    int potionPurple;
    int potionYellow;
    int hankachi;
    int bomb;

    int weaponImages[WEAPON_TYPE_MAX];
    void Load();
};