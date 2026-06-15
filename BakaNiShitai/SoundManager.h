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

    void Load();
};
