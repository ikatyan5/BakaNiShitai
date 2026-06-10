#include "SceneSettings.h"
#include "DxLib.h"

void SceneSettings::Init(ImageManager& imgMgr, GameSettings& settings_) {
    settings = &settings_;
    nextScene = SCENE_NONE;
    selectRow = 0;
    prevUp = prevDown = prevEnter = prevLeft = prevRight = false;
    waitEnterRelease = true;
}

void SceneSettings::Update() {
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
    DrawStringF(480.0f, 100.0f, _T("設定"), GetColor(0, 0, 0));
    DrawStringF(340.0f, 180.0f, _T("← → で切り替え、ENTERで戻る"), GetColor(120, 120, 120));

    const TCHAR* p1Device = settings->p1UseGamepad ? _T("コントローラー1") : _T("キーボード");
    const TCHAR* p2Device = settings->p2UseGamepad ? _T("コントローラー2") : _T("キーボード");

    int p1Color = (selectRow == 0) ? GetColor(255, 0, 0) : GetColor(0, 0, 0);
    int p2Color = (selectRow == 1) ? GetColor(255, 0, 0) : GetColor(0, 0, 0);

    DrawFormatStringF(340.0f, 380.0f, p1Color, _T("P1:  %s"), p1Device);
    DrawFormatStringF(340.0f, 500.0f, p2Color, _T("P2:  %s"), p2Device);
}

SceneID SceneSettings::GetNextScene() {
    return nextScene;
}