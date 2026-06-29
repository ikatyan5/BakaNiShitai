#pragma once
#include "DxLib.h"

struct Platform {
    float x, y, w, h; // 左上座標と幅高さ
};

class Stage {
public:
    int stageID;
    Platform platforms[10]; // 足場の配列
    int platformCount;      // 足場の数

    void Init(int id);
    void Draw();
    bool CheckLanding(float px, float py, float prevPy, float pw, float ph, float& outY);
};