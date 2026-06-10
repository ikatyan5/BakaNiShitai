#pragma once
#include "BaseScene.h"
#include "SceneID.h"
#include "Player.h"
#include "Weapon.h"
#include "Stage.h"
#include "ImageManager.h"
#include "ItemManager.h"
#include "OrbManager.h"
#include "MeteorManager.h"
#include "RestrictionManager.h"

class SceneGame : public BaseScene {
public:
    void Init(ImageManager& imgMgr);
    void InitPlayers(bool keepWinCount = false);
    void ResetGame(bool keepWinCount);
    void Update() override;
    void Draw() override;
    void DrawUI();
    void EnterHitState(bool judgeValue, bool addScore = false);
    SceneID GetNextScene() override;
    void SpawnWeapon();
    void CheckParry(Player& attacker, int ownerID);
    void CheckWeaponHit(Player& target, Player& attacker, bool judgeValue, int targetID);
    void CheckHyperTouch();
    void ThrowWeapon(Player& player, int ownerID);
    void CheckStickOrb(Player& player, int ownerID);
    void PickupWeapon(Player& player);
    void CheckMementoMori(Player& attacker, Player& target, bool judgeValue);
    void DrawMementoMori(Player& attacker);
private:
    enum GameState {
        STATE_COUNTDOWN,
        STATE_PLAYING,
        STATE_HIT,
        STATE_RESULT,
        STATE_GAMEEND,
    };

    GameState state;
    Player player1;
    Player player2;
    Stage stage;

    static const int WEAPON_MAX = 16;
    Weapon weapons[WEAPON_MAX];
    ImageManager* imgMgr;
    ItemManager itemManager;
    OrbManager orbManager;
    MeteorManager meteorManager;
    RestrictionManager restrictionManager;
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

    bool setsunaSignVisible; // ！表示フラグ

    int countdownTimer;

    int currentTex;  // 今フレームの描画テクスチャ
    int prevTex;     // 前フレームの描画テクスチャ
    int blurMode;    // 0=クリア 1=弱 2=強
    int blurTimer;   // 切り替えまでのカウントダウン

    int hyperPlayerID;  // ハイパー強い側のプレイヤーID（1か2）

    // 刹那の見切り
    enum SetsunaPhase {
        SETSUNA_SLIDE,   // スライドイン中
        SETSUNA_READY,   // Ready表示中
        SETSUNA_WAIT,    // ！待機中
        SETSUNA_ACTIVE,  // ！表示中・攻撃可能
    };
    SetsunaPhase setsunaPhase;
    int setsunaPhaseTimer;
    float setsunaP1UIX;
    float setsunaP2UIX;

    bool p1Glowing;
    bool p2Glowing;

    int p1HpIndex = 0;
    int p2HpIndex = 0;

    SceneID nextScene = SCENE_NONE;
};