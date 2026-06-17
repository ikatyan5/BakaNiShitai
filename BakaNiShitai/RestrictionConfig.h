// RestrictionConfig.h
#pragma once

// 〇が付いているのは実装済み

enum RestrictionType {
    REST_NONE,
    REST_GRAVITY_ZERO,        // 〇 重力ゼロ
    REST_HOVER_JUMP,          // 〇 ホバリングジャンプ
    REST_THROW_NO_DAMAGE,     // 〇 投げダメなし
    REST_MELEE_MUSOU,         // 〇 近接無双（武器近接で場外まで吹っ飛ばす＋最初からシールド）
    REST_STICK_ONLY,          // 〇 杖だけ
    REST_BOOMERANG_ONLY,      // 〇 ブーメランだけ
    REST_SETSUNA,             // 〇 刹那の見切り
    REST_GRAVITY_INSANE,      // 〇 重力ランダム
    REST_SCREEN_FLIP,         // 〇 画面上下反転
    REST_MASH_MOVE,           // 〇 連打移動
    REST_METEOR,              // 〇 隕石避けモード
    REST_HYPETSUYOI,          // 〇 ハイパー強いモード
    REST_ICE_FLOOR,           // 〇 氷の床（地上がツルツル滑る慣性移動）
    REST_SCREEN_BLUR,         // 〇 画面ぼやけ
    REST_SWAP,                // 〇 入れ替え（一定時間ごとに2人の位置を交換）
    REST_TUG,                 // 〇 綱引き（常に画面中央へ引っ張られる）
    REST_TYPE_MAX
};