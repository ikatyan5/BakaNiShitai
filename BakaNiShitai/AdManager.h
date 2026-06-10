#pragma once
#include "DxLib.h"
#include "ImageManager.h"

class AdManager {
public:
    void Init(ImageManager& imgMgr);
    void Update();
    void Draw();

private:
    static const int AD_MAX = 5;
    static const int AD_DURATION = 360;      // 6•b
    static const int AD_SPAWN_MIN = 120;     // 2•b
    static const int AD_SPAWN_MAX = 300;     // 5•b
    static const int AD_W = 456;
    static const int AD_H = 328;

    struct AdSlot {
        bool active;
        int imageIndex;
        float x;
        float y;
        float vx;
        float vy;
        int timer;
    };

    AdSlot slots[AD_MAX];
    int spawnTimer;
    ImageManager* imgMgr;
};