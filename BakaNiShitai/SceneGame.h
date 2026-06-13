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
#include "AdManager.h"
#include "RestrictionManager.h"
#include "GameSettings.h"

class SceneGame : public BaseScene {
public:
    void Init(ImageManager& imgMgr, GameSettings& settings);
    void InitPlayers(bool keepWinCount = false);
    void ResetGame(bool keepWinCount);
    void Update() override;
    void Draw() override;
    void DrawUI();
    void EnterHitState(bool judgeValue, bool addScore = false);
    SceneID GetNextScene() override;
    void SpawnWeapon();
    void CheckParry(Player& attacker, int ownerID);
    void CheckTensaiTsue(Player& player);
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
    GameSettings* settings;
    ItemManager itemManager;
    OrbManager orbManager;
    MeteorManager meteorManager;
    AdManager adManager;
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

    int gravityInsaneLevel;  // 0～4の重力段階
    int gravityInsaneTimer;  // 切り替えまでのカウントダウン

    int flipPattern;   // 0=上下+左右  1=上下+スワップ  2=左右+スワップ
    int flipTimer;     // 切り替えまでのカウントダウン

    bool wallEndLeft;
    bool wallEndRight;
    int wallEndTimer;

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

    struct FallingUI {
        float x, y;        // 現在の中心座標
        float baseX, baseY;// 揺れの基準になる元の中心座標
        float vy;          // 落下速度
        float angle;       // 回転角度
        float angle2;      // 回転角度（時間の１の位用）
        int count;         // 落下カウント（確率の素）
        bool falling;      // 落下中か
        bool landed;       // 着地して転がってるか
    };
    FallingUI hpUI[2];     // 0=P1, 1=P2
    FallingUI scoreUI[2];  // 0=P1, 1=P2
    FallingUI timeUI;
    int uiShakeTimer;      // 揺れ演出用のフレームカウンタ
    void InitFallingUI();
    void UpdateFallingUI(bool enteredHeavy);
    void UpdateSetsuna();

    SceneID nextScene = SCENE_NONE;
};