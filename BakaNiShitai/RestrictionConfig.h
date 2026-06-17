// RestrictionConfig.h
#pragma once

// 〇が付いているのは実装済み

enum RestrictionType {
    REST_NONE,
    REST_GRAVITY_ZERO,        // 〇 重力ゼロ
    REST_HOVER_JUMP,          // 〇 ホバリングジャンプ
    REST_THROW_NO_DAMAGE,     // 〇 投げダメなし
    REST_MELEE_NO_DAMAGE,     // 〇 近接ダメなし（弾き延長）
    REST_STICK_ONLY,          // 〇 杖だけ
    REST_BOOMERANG_ONLY,      // 〇 ブーメランだけ
    REST_SETSUNA,             // 〇 刹那の見切り
    REST_GRAVITY_INSANE,      // 〇 重力ランダム
    REST_SCREEN_FLIP,         // 〇 画面上下反転
    REST_MASH_MOVE,           // 〇 連打移動
    REST_METEOR,              // 〇 隕石避けモード
    REST_HYPETSUYOI,          // 〇 ハイパー強いモード
    REST_JUMP_LIMIT,          // 〇 ジャンプ制限（空中のみ横移動）
    REST_SCREEN_BLUR,         // 〇 画面ぼやけ
    REST_SWAP,                // 〇 入れ替え（一定時間ごとに2人の位置を交換）
    REST_TUG,                 // 〇 綱引き（常に画面中央へ引っ張られる）
    REST_TYPE_MAX
};