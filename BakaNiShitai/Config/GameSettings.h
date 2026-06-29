// GameSettings.h
#pragma once
#include "DxLib.h"

// 設定画面で決める対戦設定
// 各プレイヤーの入力（パッド/キーボード）と使うパッド番号を持つ

struct GameSettings {
    bool p1UseGamepad = false;
    int  p1PadID = DX_INPUT_PAD1;
    bool p2UseGamepad = false;
    int  p2PadID = DX_INPUT_PAD2;
};