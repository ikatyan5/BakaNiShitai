#pragma once
#include "DxLib.h"

// 効果音をまとめて管理するクラス（ImageManagerの音版）
// main.cppで1回だけLoad()して、ゲーム全体で使い回す
class SoundManager {
public:
    int hit;          // 攻撃を当てた時
    int explosion;    // 赤ポーションが爆発した時
    int pickup;       // アイテムを拾った時
    int mementoMori;  // メメントモリを撃った時
    int menu;         // メニューのカーソル移動
    int parry;        // はじき返し（弾き返し）成功
    int setsuna;      // 刹那の見切り：！が出るまでの余興音
    int weaponThrow;  // 武器を投げた時（throwはC++予約語なのでこの名前）
    int weaponPickup; // 武器を拾った時
    int jump;         // ジャンプした時
    int setsunaSign;  // 刹那の見切り：！が出た合図

    void Load();
};
