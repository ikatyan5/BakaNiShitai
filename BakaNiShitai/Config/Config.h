#pragma once

// ゲーム全体の基本設定値。
// 画面サイズ・物理（重力/ジャンプ）・当たり判定・武器/アイテムの調整値をまとめる


// 画面サイズ（ウィンドウの幅・高さ）。描画や画面端判定で広く使う。
const int SCREEN_W = 1280;
const int SCREEN_H = 920;

// プレイヤーの当たり判定
const float PLAYER_HIT_W = 60.0f;
const float PLAYER_HIT_H = 110.0f;
const float PLAYER_HIT_CY = 55.0f;

const float GRAVITY = 0.5f;// 重力加速度
const float JUMP_POWER = -12.0f; // ジャンプの初速
const float GROUND_Y = 800.0f; // 仮の地面の位置
const int WINNING_SCORE = 3; // 勝利条件のスコア

// 武器関係
// 投げ速度は武器ごとに WeaponConfig.h の WEAPON_DATA[].throwSpeed を使う
const float WEAPON_ROTATE = 0.15f;			// 回転速度
const int   WEAPON_W = 40;					// 判定の幅
const int   WEAPON_H = 20;					// 判定の高さ
const int WEAPON_SPAWN_INTERVAL = 180;		// 武器がスポーンする間隔
const int WEAPON_GROUND_TIME = 300;			// 地面に落ちてから消えるまでの時間
const float WEAPON_GROUND_OFFSET = 15.0f;	// 地面にめり込む量
const int PARRY_FRAME = 6;					// はたき返し判定フレーム ×６して
const int BARE_HAND_CHARGE_FRAMES = 7;  // 素手の構えフレーム
const int BARE_HAND_ATTACK_FRAMES = 8;  // 素手の攻撃判定フレーム


// アイテム関係
const int ITEM_SPAWN_INTERVAL = 300;
const int DEFAULT_TIME = 30;
const float ITEM_HIT_W = 58.0f;  // アイテムの当たり判定幅
const float ITEM_HIT_H = 74.0f;  // アイテムの当たり判定高さ