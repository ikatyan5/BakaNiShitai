#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Weapon.h"
#include "Stage.h"
#include "ImageManager.h"
#include "ItemManager.h"
#include "OrbManager.h"

class SceneGame : public BaseScene {
public:
    void Init(ImageManager& imgMgr);
    void InitPlayers(bool keepWinCount = false);
    void ResetGame(bool keepWinCount);
    void Update() override;
    void Draw() override;
    void DrawUI();
    void EnterHitState(bool judgeValue, bool addScore = false);
    int GetNextScene() override;
    void SpawnWeapon();
    void CheckParry(Player& attacker, int ownerID);
    void CheckWeaponHit(Player& target, Player& attacker, bool judgeValue, int targetID);
    void ThrowWeapon(Player& player, int ownerID);
    void PickupWeapon(Player& player);
    void CheckMementoMori(Player& attacker, Player& target, bool judgeValue);
    void DrawMementoMori(Player& attacker);
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
    OrbManager orbManager;
    bool JUDGE;
    int HIT_TIMER;
    int RESULT_TIMER;
    int weaponSpawnTimer;

    bool isDraw;
    int matchTime; // 実際に使う試合時間
    int timeTimer; // フレームカウンター

    int mementoMoriTimer;       // 余韻タイマー
    int mementoMoriShooterID;   // 撃ったプレイヤーのID
    int mementoMoriWinnerID;    // ENDする側
    bool mementoMoriPending;    // END待ち状態か

    bool p1Glowing;
    bool p2Glowing;

    int p1HpIndex = 0;
    int p2HpIndex = 0;

    int nextScene;
};