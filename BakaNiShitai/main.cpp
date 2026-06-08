#include "DxLib.h"
#include "ImageManager.h"
#include "SceneGame.h"
#include <ctime>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    ChangeWindowMode(TRUE);
    SetGraphMode(1280, 920, 32);

    if (DxLib_Init() == -1) return -1;
    SetBackgroundColor(135, 206, 235); // 水色
    SetWaitVSyncFlag(TRUE);
    SetMainWindowText(_T("君たちを\"バカ\"にしたい"));
    SetDrawScreen(DX_SCREEN_BACK);
    srand((unsigned int)time(NULL));

    ImageManager imageManager;
    imageManager.Load();

    SceneGame sceneGame;
    sceneGame.Init(imageManager);

    while (ProcessMessage() == 0) {
        sceneGame.Update();
        sceneGame.Draw();

        ScreenFlip();
        WaitTimer(16);
    }

    DxLib_End();
    return 0;
}