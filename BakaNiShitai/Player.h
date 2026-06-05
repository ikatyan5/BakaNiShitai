#pragma once
#include "Dxlib.h"
#include "Weapon.h"
#include "Stage.h"
#include "ImageManager.h"

class Player
{
public:
	float x, y;         // 位置
	float vx, vy;       // 速度
	float jumpPower;	// ジャンプの初速(デフォルトはJUMP_POWER)
	bool onGround;      // 地面にいるか
	bool attacking;     // 攻撃中か
	int attackTimer;    // 攻撃判定の残りフレーム数
	int PlayerID;       // プレイヤーID（1か2）
	bool wantThrow;     // 武器を投げたいか
	bool facingRight;   // 向いている方向
	int winCount;       // 勝利数
	int jumpCount; // 残りジャンプ回数
	int holdingWeaponIndex; // 持ってる武器のインデックス（-1なら持ってない）

	float prevY;
	bool wantExplode;
	bool isBlinking;
	int blinkTimer;
	bool isReadyThrow;
	bool canAttack;

	int animFrame;   // 今何枚目か
	int animTimer;   // 切り替えカウンター

	void Init(float startX, float startY, int id, bool facingR, ImageManager& imgMgr, int keepWinCount = 0);
	bool GetOnGround() { return onGround; }
	bool GetAttacking() { return attacking; }
	bool GetFacingRight() { return facingRight; }
	int GetWinCount() { return winCount; }

	bool CheckHit(float ax, float ay, float aw, float ah,
		float bx, float by, float bw, float bh);
	bool CheckAttackHit(Player& other, Weapon* weapons);
	void UpdateInput();
	void UpdatePosition(Stage& stage);
	void UpdateJump();
	void UpdateAnim();
	void UpdateAttack(Weapon* weapons);
	void Update(Stage& stage, Weapon* weapons);
	void ApplyGravity();
	void Draw(Weapon* weapons);
private:
	int playerImage[7];


	float weaponDrawX;
	float weaponDrawY;

	bool prevJumpKey; // 前フレームのジャンプキー状態
	bool prevAttackKey;
	bool prevThrowKey;
};