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
#include "SoundManager.h"

class SceneGame : public BaseScene {
public:
    void Init(ImageManager& imgMgr, GameSettings& settings, SoundManager& sndMgr);
    void InitPlayers(bool keepWinCount = false);
    void ResetGame(bool keepWinCount, bool keepRestriction = false);
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
    void DrawFlyExplosion();

    // 妨害（Restriction）が対戦中の状態を操作するためのアクセサ。
    // 各 Restriction はこれら経由でゲーム本体に手を入れる。
    Player& GetPlayer1() { return player1; }
    Player& GetPlayer2() { return player2; }
    SoundManager* GetSound() { return sound; }
    ImageManager& GetImageManager() { return *imgMgr; }
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
    SoundManager* sound;
    ItemManager itemManager;
    OrbManager orbManager;
    MeteorManager meteorManager;
    AdManager adManager;
    RestrictionManager restrictionManager;
    bool JUDGE;
    int HIT_TIMER;
    int RESULT_TIMER;
    int weaponSpawnTimer;

    int animTimer = 0;
    int animFrame = 0;

    bool isDraw;
    int matchTime; // 実際に使う試合時間
    int timeTimer; // フレームカウンター

    int mementoMoriTimer;       // 余韻タイマー
    int mementoMoriShooterID;   // 撃ったプレイヤーのID
    int mementoMoriWinnerID;    // ENDする側
    bool mementoMoriPending;    // END待ち状態か

    bool setsunaSignVisible; // ！表示フラグ

    bool setsunaRedoPending;  // 刹那で両者遅すぎ→引き分けやり直し待ち
    bool flyExplodeActive;    // フライング爆発演出が出ているか
    float flyExplodeX;        // フライング爆発の中心X
    float flyExplodeY;        // フライング爆発の中心Y
    int  flyExplodeTimer;     // フライング爆発の残り表示フレーム

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

    // 入れ替え＋分身の状態・挙動・描画は SwapRestriction（Restriction.cpp）へ移設済み。

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

    bool hyperDashing;
    int  hyperDashCooldown;
    float hyperDashDistance;

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
    void UpdateScreenFlip();
    void UpdateMeteor();
    void UpdateMashMove();
    void CheckMeleeHit(Player& attacker, Player& target, bool judgeValue);

    SceneID nextScene = SCENE_NONE;
};