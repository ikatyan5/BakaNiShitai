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
    prevEnter = false;
}

void SceneMenu::Update() {
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
        // チュートリ・設定は後で
    }

    prevUp = up;
    prevDown = down;
    prevEnter = enter;
}

void SceneMenu::Draw() {
    ClearDrawScreen();
    //DrawBoxF(0.0f, 0.0f, 1280.0f, 920.0f, GetColor(255, 255, 255), TRUE);
    DrawStringF(630.0f, 100.0f, _T("ENTER で決定だ"), GetColor(0, 0, 0));
    float startY = 250.0f;
    float gap = 220.0f;
    float itemW = 300.0f;
    float itemH = 200.0f;
    float centerX = (1280.0f - itemW) / 2.0f;

    for (int i = 0; i < MENU_COUNT; i++) {
        int idx = (selectIndex == i) ? 1 : 0;
        int img = -1;
        if (i == 0) img = imgMgr->menuTatakau[idx];
        if (i == 1) img = imgMgr->menuTutorial[idx];
        if (i == 2) img = imgMgr->menuSetting[idx];

        float y = startY + i * gap;
        if (i == 1 || i == 2) {
            float textX = centerX + itemW + 20.0f;
            float textY = y + itemH / 2.0f - 8.0f;
            DrawStringF(textX, textY, _T("（まだできてないよ～）"), GetColor(180, 180, 180));
        }
        DrawExtendGraphF(centerX, y, centerX + itemW, y + itemH, img, TRUE);
    }
}
SceneID SceneMenu::GetNextScene() {
    return nextScene;
}