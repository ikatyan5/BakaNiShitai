#pragma once
#include "DxLib.h"
#include <random>

// 効果音をまとめて管理するクラス（ImageManagerの音版）
// main.cppで1回だけLoad()して、ゲーム全体で使い回す
class SoundManager {
public:
    int hit;          // 攻撃を当てた時
    int explosion;    // 赤ポーションで爆発した時
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

    // ---- BGM ----
    int bgmMenu;          // メニュー・チュートリアル・設定で流れるBGM
    int bgmGame[3];       // ゲーム画面のBGM（この中から1曲ランダムで流す）
    int currentGameBgm;   // 今選ばれているゲームBGMのハンドル（-1なら無し）

    // ゲームBGMの音量（0～255）。メニューBGMはそのまま、ゲームだけ下げる。
    static const int GAME_BGM_VOLUME = 200;

    void Load();

    // メニュー系BGMを流す。すでに鳴っていれば頭出しせず流しっぱなしにする。
    // ゲームBGMが鳴っていたら止める。
    void PlayMenuBgm();
    // ゲームBGMを3曲からランダムで1曲選んで流す。メニュー系BGMは止める。
    void PlayGameBgmRandom();
    // 今のゲームBGMを止める（曲の選択自体は保持）。刹那の見切り中に使う。
    void StopGameBgm();
    // 止めていたゲームBGMを再び流す（選ばれている曲をそのまま）。
    void ResumeGameBgm();

private:
    // rand()%3 だと起動直後にクセが出て同じ曲ばかりになるので、
    // ちゃんとした乱数エンジンで曲を選ぶ。
    std::mt19937 rng{ std::random_device{}() };
};
