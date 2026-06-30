#include "SceneSettings.h"
#include "Config.h"
#include "DxLib.h"

void SceneSettings::Init(ImageManager& imgMgr_, GameSettings& settings_) {
    settings = &settings_;
    nextScene = SCENE_NONE;
    selectRow = 0;
    imgMgr = &imgMgr_;
    animTimer = 0;
    animFrame = 0;
    prevUp = prevDown = prevEnter = prevLeft = prevRight = false;
    waitEnterRelease = true;
}

void SceneSettings::Update() {
    animTimer++;
    if (animTimer >= 10) {
        animTimer = 0;
        animFrame = (animFrame + 1) % 2;
    }
    bool up = CheckHitKey(KEY_INPUT_UP);
    bool down = CheckHitKey(KEY_INPUT_DOWN);
    bool left = CheckHitKey(KEY_INPUT_LEFT);
    bool right = CheckHitKey(KEY_INPUT_RIGHT);
    bool enter = CheckHitKey(KEY_INPUT_RETURN);
    bool back = CheckHitKey(KEY_INPUT_ESCAPE);

    if (waitEnterRelease) {
        if (!enter) waitEnterRelease = false;
        prevEnter = enter;
        return;
    }

    if (up && !prevUp)     selectRow = (selectRow - 1 + 2) % 2;
    if (down && !prevDown) selectRow = (selectRow + 1) % 2;

    if ((left && !prevLeft) || (right && !prevRight)) {
        if (selectRow == 0) settings->p1UseGamepad = !settings->p1UseGamepad;
        if (selectRow == 1) settings->p2UseGamepad = !settings->p2UseGamepad;
    }

    if (enter && !prevEnter) nextScene = SCENE_MENU;
    if (back) nextScene = SCENE_MENU;

    prevUp = up; prevDown = down;
    prevLeft = left; prevRight = right;
    prevEnter = enter;
}

void SceneSettings::Draw() {
    ClearDrawScreen();
    DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboardSetting[animFrame], TRUE);
    DrawStringF(550.0f, 100.0f, ("設定"), GetColor(255, 255, 255));
    DrawStringF(480.0f, 480.0f, ("↑ ↓で上下、← → で切り替え、ENTERで戻る"), GetColor(255, 255, 255));

    int p1Idx = (selectRow == 0) ? (2 + animFrame) : animFrame;
    int p2Idx = (selectRow == 1) ? (2 + animFrame) : animFrame;

    int p1Img = settings->p1UseGamepad ? imgMgr->controller[p1Idx] : imgMgr->keyboard[p1Idx];
    DrawExtendGraphF(500.0f, 165.0f, 1000.0f, 365.0f, p1Img, TRUE);

    int p2Img = settings->p2UseGamepad ? imgMgr->controller[p2Idx] : imgMgr->keyboard[p2Idx];
    DrawExtendGraphF(500.0f, 520.0f, 1000.0f, 720.0f, p2Img, TRUE);
}

SceneID SceneSettings::GetNextScene() {
    return nextScene;
}