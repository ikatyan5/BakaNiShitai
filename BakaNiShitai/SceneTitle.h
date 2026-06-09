// SceneTitle.h
#pragma once
#include "BaseScene.h"
#include "ImageManager.h"

class SceneTitle : public BaseScene {
public:
    void Init(ImageManager& imgMgr);
    void Update() override;
    void Draw() override;
    SceneID GetNextScene() override;

private:
    ImageManager* imgMgr = nullptr;
    SceneID nextScene = SCENE_NONE;
    int titleImg = -1;
    int pressAnyKeyImg[2];
    int animTimer = 0;
    int animFrame = 0;
};