#pragma once
#include "DxLib.h"
#include "WeaponConfig.h"

// 画像をまとめて読み込んで持っておくクラス
// main で1回だけ読み込み、全体で使い回す（SoundManagerの画像版）

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
    int pressAnyKey[2];
    int menuTatakau[4];
    int menuTutorial[4];
    int menuSetting[4];
    int adImages[3];
    int surpMark;
    int titleLogo;
    int zukan;
    int zukan_restriction;
    int keyboard[4];
    int controller[4];
    int blackboard[2];
    int blackboardSetting[2];
    int blackboardGame[2];
    int menuAnimTatakau[2];
    int menuAnimTutorial[2];
    int menuAnimSetting[2];

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
    int gyaku[2];
    int tensai[2];

    // アイテム
    int potionBlue;
    int potionRed;
    int potionPurple;
    int potionYellow;
    int hankachi;
    int bomb;
    int banana;
    int kinoko;

    int weaponImages[WEAPON_TYPE_MAX];
    void Load();
};