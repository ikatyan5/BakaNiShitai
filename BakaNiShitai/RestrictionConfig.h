// RestrictionConfig.h
#pragma once

// 丸が付いているのは実装済み

enum RestrictionType {
    REST_NONE,
    REST_GRAVITY_ZERO,        // 重力ゼロ
    REST_HOVER_JUMP,          // ホバリングジャンプ
    REST_THROW_NO_DAMAGE,     // 投げダメなし
    REST_MELEE_NO_DAMAGE,     // 近接ダメなし（弾き延長）
    REST_STICK_ONLY,          // 杖だけ
    REST_BOOMERANG_ONLY,      // ブーメランだけ
    REST_SETSUNA,             // 刹那の見切り
    REST_GRAVITY_CONTROL,     // 〇 重力操作（下キーで急落下）
    REST_SCREEN_FLIP,         // 画面上下反転
    REST_MASH_MOVE,           // 連打移動
    REST_METEOR,              // 隕石避けモード
    REST_ONIIGOKKO,           // 鬼ごっこモード
    REST_JUMP_LIMIT,          // 〇 ジャンプ制限（空中のみ横移動）
    REST_SCREEN_BLUR,         // 画面ぼやけ
    REST_WINDOW_MOVE,         // ウィンドウ移動
    REST_TYPE_MAX
};