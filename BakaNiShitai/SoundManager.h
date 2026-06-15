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
    int deflect;      // はたき落とし（タイミングずれの防御）
    int meteor;       // 隕石が落ち始めた時
    int attack;       // 素手で攻撃した時
    int countdown;    // カウントダウンの数字が変わる時（3・2・1）
    int decide;       // メニューで決定した時
    int tensai;       // 天才の杖を振った時
    int win;          // 試合に勝って「〇の勝ち！」が出る時

    void Load();
};
