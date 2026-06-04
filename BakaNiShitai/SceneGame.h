#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Weapon.h"
#include "Stage.h"
#include "ImageManager.h"
#include "ItemManager.h"

class SceneGame : public BaseScene {
public:
    void Init(ImageManager& imgMgr);
    void InitPlayers(bool keepWinCount = false);
    void Update() override;
    void Draw() override;
    void DrawUI();
    void EnterHitState(bool judgeValue);
    int GetNextScene() override;

private:
    enum GameState {
        STATE_PLAYING,
        STATE_HIT,
        STATE_RESULT,
        STATE_GAMEEND,
    };

    GameState state;
    Player player1;
    Player player2;
    Stage stage;

    static const int WEAPON_MAX = 8;
    Weapon weapons[WEAPON_MAX];
    ImageManager* imgMgr;
    ItemManager itemManager;
    bool JUDGE;
    int HIT_TIMER;
    int RESULT_TIMER;
    int weaponSpawnTimer;

    bool isDraw;
    int matchTime; // 実際に使う試合時間
    int timeTimer; // フレームカウンター


    int p1HpIndex = 0;
    int p2HpIndex = 0;

    int nextScene;
};