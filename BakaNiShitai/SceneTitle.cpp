// SceneTitle.cpp
#include "SceneTitle.h"
#include "DxLib.h"

void SceneTitle::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    nextScene = SCENE_NONE;
    titleImg = imgMgr->titleLogo;
    pressAnyKeyImg[0] = imgMgr->pressAnyKey[0];
    pressAnyKeyImg[1] = imgMgr->pressAnyKey[1];
    animTimer = 0;
    animFrame = 0;
}

void SceneTitle::Update() {
    animTimer++;
    if (animTimer >= 10) {  // 10フレームで切り替え
        animTimer = 0;
        animFrame = (animFrame + 1) % 2;
    }
    // 何かキーを押したらメニューへ
    if (CheckHitKeyAll()) {
        nextScene = SCENE_MENU;
    }
}

void SceneTitle::Draw() {
    ClearDrawScreen();

    DrawBox(0.0f, 0.0f, 1280.0f, 920.0f, GetColor(255, 255, 255), TRUE);
    DrawBox(0.0f, 580.0f, 1280.0f, 920.0f, GetColor(0, 0, 0), TRUE);
    DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->blackboard[animFrame], TRUE);
    // タイトル画像を中央に表示
    DrawExtendGraphF(
        240.0f, 20.0f, 1020.0f, 540.0f,
        titleImg, TRUE
    );
    // PRESS ANY KEY
    DrawExtendGraphF(340.0f, 650.0f, 940.0f, 850.0f, pressAnyKeyImg[animFrame], TRUE);

}

SceneID SceneTitle::GetNextScene() {
    return nextScene;
}