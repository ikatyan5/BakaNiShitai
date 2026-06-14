// SceneMenu.cpp
#include "SceneMenu.h"
#include "DxLib.h"

static const TCHAR* MENU_ITEMS[] = {
    _T("たたかう"),
    _T("チュートリ"),
    _T("設定"),
};
static const int MENU_COUNT = 3;

void SceneMenu::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    nextScene = SCENE_NONE;
    selectIndex = 0;
    prevUp = false;
    prevDown = false;
    prevEnter = true;
}

void SceneMenu::Update() {
    animTimer++;
    if (animTimer >= 10) {
        animTimer = 0;
        animFrame = (animFrame + 1) % 2;
    }
    animTimer2++;
    if (animTimer2 >= 40) { // 10→20で半分の速度
        animTimer2 = 0;
        animFrame2 = (animFrame2 + 1) % 2;
    }

    bool up = CheckHitKey(KEY_INPUT_UP);
    bool down = CheckHitKey(KEY_INPUT_DOWN);
    bool enter = CheckHitKey(KEY_INPUT_RETURN);

    if (up && !prevUp) {
        selectIndex = (selectIndex - 1 + MENU_COUNT) % MENU_COUNT;
    }
    if (down && !prevDown) {
        selectIndex = (selectIndex + 1) % MENU_COUNT;
    }
    if (enter && !prevEnter) {
        if (selectIndex == 0) nextScene = SCENE_GAME;
        if (selectIndex == 2) nextScene = SCENE_SETTINGS;
    }

    prevUp = up;
    prevDown = down;
    prevEnter = enter;
}

void SceneMenu::Draw() {
    ClearDrawScreen();
    DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->blackboard[animFrame], TRUE);

    // Tips枠（仮）
    DrawStringF(630.0f, 100.0f, _T("ENTER で決定だ"), GetColor(0, 0, 0));

    int idx0 = (selectIndex == 0) ? (2 + animFrame) : animFrame;
    int idx1 = (selectIndex == 1) ? (2 + animFrame) : animFrame;
    int idx2 = (selectIndex == 2) ? (2 + animFrame) : animFrame;

    // たたかう（大きめ）500x200基準
    DrawExtendGraphF(170.0f, 140.0f, 170.0f + 450.0f, 140.0f + 200.0f, imgMgr->menuTatakau[idx0], TRUE);


    // チュートリ（ふつう）
    DrawExtendGraphF(180.0f, 370.0f, 170.0f + 350.0f, 370.0f + 120.0f, imgMgr->menuTutorial[idx1], TRUE);

    // 非選択：0と1を切り替え、選択中：2と3を切り替え
    DrawExtendGraphF(170.0f, 490.0f, 170.0f + 220.0f, 480.0f + 210.0f, imgMgr->menuSetting[idx2], TRUE);

    // 右側アニメ
    int* currentAnim = (selectIndex == 0) ? imgMgr->menuAnimTatakau :
        (selectIndex == 1) ? imgMgr->menuAnimTutorial :
        imgMgr->menuAnimSetting;
    DrawExtendGraphF(700.0f, 250.0f, 1150.0f, 700.0f, currentAnim[animFrame2], TRUE);
}

SceneID SceneMenu::GetNextScene() {
    return nextScene;
}