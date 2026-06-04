#pragma once
#include "DxLib.h"

struct Platform {
    float x, y, w, h; // ¶ãÀ•W‚Æ•‚‚³
};

class Stage {
public:
    int stageID;
    Platform platforms[10]; // ‘«ê‚Ì”z—ñ
    int platformCount;      // ‘«ê‚Ì”

    void Init(int id);
    void Draw();
    bool CheckLanding(float px, float py, float prevPy, float pw, float ph, float& outY);
};