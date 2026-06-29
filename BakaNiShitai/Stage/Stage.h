#pragma once
#include "DxLib.h"

// 足場一つ分の矩形
struct Platform {
    float x, y, w, h; // 左上座標と幅高さ
};

// 対戦ステージ。足場(Platform)の配置を持ち、着地判定を行う
// stageID で将来のステージ選択に対応する想定

class Stage {
public:
    int stageID;
    Platform platforms[10]; // 足場の配列
    int platformCount;      // 足場の数

    void Init(int id);
    void Draw();
    bool CheckLanding(float px, float py, float prevPy, float pw, float ph, float& outY);
};