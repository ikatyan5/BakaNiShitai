#include "DxLib.h"
#include "ImageManager.h"
#include "SceneID.h"
#include "BaseScene.h"
#include "SceneTitle.h"
#include "SceneMenu.h"
#include "SceneGame.h"
#include "SceneSettings.h"
#include "SceneTutorial.h"
#include "GameSettings.h"
#include <ctime>

BaseScene* CreateScene(SceneID id, ImageManager& imgMgr, GameSettings& settings) {
    switch (id) {
    case SCENE_TITLE: {
        auto* s = new SceneTitle();
        s->Init(imgMgr);
        return s;
    }
    case SCENE_MENU: {
        auto* s = new SceneMenu();
        s->Init(imgMgr);
        return s;
    }
    case SCENE_GAME: {
        auto* s = new SceneGame();
        s->Init(imgMgr, settings);
        return s;
    }
    case SCENE_SETTINGS: {
        auto* s = new SceneSettings();
        s->Init(imgMgr,settings);
        return s;
    }
    case SCENE_TUTORIAL: {
        auto* s = new SceneTutorial();
        s->Init(imgMgr);
        return s;
    }
    default:
        return nullptr;
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    SetGraphMode(1280, 920, 32);
    if (DxLib_Init() == -1) return -1;
    int padNum = GetJoypadNum();
    //printfDx(_T("接続パッド数=%d\n"), padNum);
    SetBackgroundColor(135, 206, 235);
    SetWaitVSyncFlag(TRUE);
    SetMainWindowText(_T("君たちを\"バカ\"にしたい"));
    SetDrawScreen(DX_SCREEN_BACK);
    srand((unsigned int)time(NULL));

    ImageManager imageManager;
    imageManager.Load();

    SceneID currentID = SCENE_TITLE;
    GameSettings settings;
    BaseScene* currentScene = CreateScene(currentID, imageManager, settings);

    while (ProcessMessage() == 0) {
        LONGLONG start = GetNowHiPerformanceCount();
        ClearDrawScreen();
        currentScene->Update();
        currentScene->Draw();
        ScreenFlip();

        SceneID next = currentScene->GetNextScene();
        if (next != SCENE_NONE) {
            delete currentScene;
            currentID = next;
            currentScene = CreateScene(currentID, imageManager, settings);
        }
        while (GetNowHiPerformanceCount() - start < 16667) {}
    }

    delete currentScene;
    DxLib_End();
    return 0;
}