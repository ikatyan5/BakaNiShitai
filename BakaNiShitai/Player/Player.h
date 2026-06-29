#pragma once
#include "Dxlib.h"
#include "Weapon.h"
#include "Stage.h"
#include "ImageManager.h"
#include "RestrictionManager.h"

// 対戦キャラ1人ぶん 位置・速度・状態を持ち、移動・ジャンプ・攻撃・武器投げを担当する
// 1P と 2P はこのクラスのインスタンスとして2つ作られる

class Player
{
public:
	float x, y;				// 位置
	float vx, vy;			// 速度
	float moveSpeed;		// 移動速度
	float jumpPower;		// ジャンプの初速(デフォルトはJUMP_POWER)
	bool onGround;			// 地面にいるか
	bool attacking;			// 攻撃中か
	int attackTimer;		// 攻撃判定の残りフレーム数
	int PlayerID;			// プレイヤーID（1か2）
	bool wantThrow;			// 武器を投げたいか
	bool facingRight;		// 向いている方向
	int winCount;			// 勝利数
	int jumpCount;			// 残りジャンプ回数
	bool justJumped;		// このフレームでジャンプしたか（音用。SceneGameが読んで戻す）
	bool justBareAttacked;	// このフレームで素手攻撃を出したか（音用。SceneGameが読んで戻す）
	int holdingWeaponIndex; // 持ってる武器のインデックス（-1なら持ってない）
	
	bool isDashing;

	float prevY;
	bool wantExplode;
	bool isBlinking;
	int blinkTimer;
	bool isReadyThrow;
	bool canAttack;
	bool isGlowing;

	int animFrame;   // 今何枚目か
	int animTimer;   // 切り替えカウンター
	int pikohanRespawnTimer;
	int stanTimer;      // スタン残りフレーム
	bool isStunned;     // スタン中かどうか
	bool hasShield;       // シールド持ちか
	bool freezeAnim; // アニメ固定フラグ
	bool dashAttack;
	
	bool useGamepad; // trueならコントローラー入力を使う
	int padID;       // DX_INPUT_PAD1 or DX_INPUT_PAD2
	int gravityInsaneLevel; // REST_GRAVITY_INSANEの重力段階
	float accelMult;        // REST_ACCEL：時間で上がる加速倍率（移動・ジャンプ・投げに使う）
	int reverseTimer; // 操作反転タイマー
	int tensaiAnimTimer;
	void EnterStun();   // スタン開始
	void EnterStunShort(); // 短スタン（投げピコハン用）

	int knockbackCount;       // 蓄積カウント 0-3
	int knockbackDecayTimer;  // 300フレームでcount-1
	float knockbackVx;        // ノックバック中の速度
	int knockbackTimer;       // ノックバック継続フレーム
	bool isKnockedBack;       // ノックバック中か

	void AddKnockbackCount(); // 投げ命中時に呼ぶ
	void ApplyKnockback(float kbVx, float kbVy); // 近接命中時に呼ぶ

	void Init(float startX, float startY, int id, bool facingR, ImageManager& imgMgr, int keepWinCount = 0);
	bool GetOnGround() { return onGround; }
	bool GetAttacking() { return attacking; }
	bool GetFacingRight() { return facingRight; }
	int GetWinCount() { return winCount; }

	int GetAttackFrames(Weapon* weapons);
	bool CheckHit(float ax, float ay, float aw, float ah,
		float bx, float by, float bw, float bh);
	bool CheckAttackHit(Player& other, Weapon* weapons);
	void UpdateInput(const RestrictionManager& restrictions, Weapon* weapons);
	void UpdatePosition(Stage& stage);
	void UpdateJump(const RestrictionManager& restrictions);
	void UpdateAnim(Weapon* weapons);
	void UpdateAttack(Weapon* weapons, const RestrictionManager& restrictions);
	void Update(Stage& stage, Weapon* weapons, const RestrictionManager& restrictions);
	void ApplyGravity(const RestrictionManager& restrictions);
	void Draw(Weapon* weapons, ImageManager& imgMgr);
	// 分身用：指定座標に「体だけ」を描く。画像・アニメ・黄色状態は本体に追従し、向きだけ引数で渡す。
	void DrawDecoy(float dx, float dy, bool faceRight, int weaponType, bool showWeapon, ImageManager& imgMgr);
	void SwapImageWith(Player& other);
private:
	int playerImage[7];
	int playerGlowImage[7];
	float weaponDrawX;
	float weaponDrawY;
	bool prevJumpKey; // 前フレームのジャンプキー状態
	bool prevAttackKey;
	bool prevThrowKey;
	bool prevLeftKey;
	bool prevRightKey;

	int airTime;		// 空中にいたフレーム数
};