// SceneMenu.h
#pragma once
#include "BaseScene.h"
#include "ImageManager.h"

class SceneMenu : public BaseScene {
public:
    void Init(ImageManager& imgMgr);
    void Update() override;
    void Draw() override;
    SceneID GetNextScene() override;

private:
    ImageManager* imgMgr = nullptr;
    SceneID nextScene = SCENE_NONE;
    int selectIndex = 0;      // 現在選択中の項目
    bool prevUp = false;      // 前フレームの上キー状態
    bool prevDown = false;    // 前フレームの下キー状態
    bool prevEnter = false;   // 前フレームのEnterキー状態

    int tipsTimer = 0;
    int tipsIndex = 0;

    int animTimer = 0;
    int animFrame = 0;
    int animTimer2 = 0;
    int animFrame2 = 0;
};