#pragma once
#include "BaseScene.h"
#include "ImageManager.h"
#include "GameSettings.h"
#include "SceneID.h"

class SceneSettings : public BaseScene {
public:
    void Init(ImageManager& imgMgr, GameSettings& settings);
    void Update() override;
    void Draw() override;
    SceneID GetNextScene() override;
private:
    GameSettings* settings;
    SceneID nextScene;
    ImageManager* imgMgr;
    int animTimer = 0;
    int animFrame = 0;
    int selectRow;   // 0=P1, 1=P2
    bool prevUp, prevDown, prevEnter;
    bool prevLeft, prevRight;
    bool waitEnterRelease;
};