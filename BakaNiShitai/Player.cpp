#include "Player.h"
#include "Config.h"
#include <cmath>

void Player::Init(float startX, float startY, int id, bool facingR, ImageManager& imgMgr, int keepWinCount) {
	x = startX;
	y = startY;
	vx = vy = 0.0f;
	jumpPower = JUMP_POWER;
	onGround = true;
	attacking = false;
	attackTimer = 0;
	PlayerID = id;
	wantThrow = false;
	facingRight = facingR;
	winCount = keepWinCount;
	holdingWeaponIndex = -1;
	isBlinking = false;
	blinkTimer = 0;
	wantExplode = false;
	animFrame = 0;
	animTimer = 0;
	prevJumpKey = false;
	weaponDrawX = 0.0f;
	weaponDrawY = 0.0f;
	prevAttackKey = false;
	prevThrowKey = false;
	isReadyThrow = false;
	canAttack = true;
	for (int i = 0; i < 7; i++) {
		playerImage[i] = (id == 1) ? imgMgr.player1[i] : imgMgr.player2[i];
	}
}

// 左右移動とキー入力処理
void Player::UpdateInput() {
	vx = 0;
	if (attacking)return;

	if (PlayerID == 1) {
		if (CheckHitKey(KEY_INPUT_A)) { vx = isBlinking ? -8.0f : -5.0f; facingRight = false; }
		if (CheckHitKey(KEY_INPUT_D)) { vx = isBlinking ? 8.0f : 5.0f; facingRight = true; }
	}
	else if (PlayerID == 2) {
		if (CheckHitKey(KEY_INPUT_LEFT)) { vx = isBlinking ? -8.0f : -5.0f; facingRight = false; }
		if (CheckHitKey(KEY_INPUT_RIGHT)) { vx = isBlinking ? 8.0f : 5.0f; facingRight = true; }
	}
}

// ジャンプの処理
void Player::UpdateJump() {
	bool jumpKey = false;
	if (PlayerID == 1) jumpKey = CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_SPACE);
	if (PlayerID == 2) jumpKey = CheckHitKey(KEY_INPUT_UP);

	if (jumpKey && !prevJumpKey && jumpCount > 0) {
		vy = jumpPower;
		jumpCount--;
	}
	prevJumpKey = jumpKey;
}

// 攻撃の処理
void Player::UpdateAttack(Weapon* weapons) {
	if (!canAttack) return;

	if (attackTimer > 0) {
		attackTimer--;
		if (attackTimer == 0) attacking = false;
	}

	if (PlayerID == 1) {
		bool attackKey = CheckHitKey(KEY_INPUT_F);
		bool throwKey = CheckHitKey(KEY_INPUT_G) && CheckHitKey(KEY_INPUT_F);
		isReadyThrow = CheckHitKey(KEY_INPUT_G) && holdingWeaponIndex != -1;
		if (throwKey && !prevThrowKey && attackTimer == 0 && holdingWeaponIndex != -1) {
			wantThrow = true;
			attackTimer = 15;
		}
		else if (attackKey && !prevAttackKey && attackTimer == 0) {
			attacking = true;
			attackTimer = 15;
		}
		prevAttackKey = attackKey;
		prevThrowKey = throwKey;
	}
	else if (PlayerID == 2) {
		bool attackKey = GetMouseInput() & MOUSE_INPUT_LEFT;
		bool throwKey = (GetMouseInput() & MOUSE_INPUT_RIGHT) && (GetMouseInput() & MOUSE_INPUT_LEFT);
		isReadyThrow = (GetMouseInput() & MOUSE_INPUT_RIGHT) && holdingWeaponIndex != -1;
		if (throwKey && !prevThrowKey && attackTimer == 0 && holdingWeaponIndex != -1) {
			wantThrow = true;
			attackTimer = 15;
		}
		else if (attackKey && !prevAttackKey && attackTimer == 0) {
			attacking = true;
			attackTimer = 15;
		}
		prevAttackKey = attackKey;
		prevThrowKey = throwKey;
	}
}

void Player::UpdateAnim() {
	if (animFrame == 6) return;
	// 点滅処理
	if (isBlinking) {
		blinkTimer--;
		if (blinkTimer <= 0) {
			isBlinking = false;
			wantExplode = true;
			canAttack = true;
		}
	}

	animTimer++;


	if (isReadyThrow) {
		animFrame = 5;
	}
	else if (attacking) {
		// 攻撃中は[3]と[4]を切り替え
		if (animTimer < 8) animFrame = 3;
		else animFrame = 4;
	}
	else if (!onGround) {
		animFrame = 1;
	}
	else if (vx != 0 && onGround) {
		// 移動中は[1]と[2]を切り替え
		if (animTimer % 16 < 8) animFrame = 1;
		else animFrame = 2;
	}
	else {
		// 待機は[0]
		animFrame = 0;
		animTimer = 0;
	}
}

void Player::Update(Stage& stage, Weapon* weapons) {
	UpdateInput();
	ApplyGravity();
	UpdatePosition(stage);
	UpdateJump();
	UpdateAttack(weapons);
	UpdateAnim();
}

void Player::UpdatePosition(Stage& stage) {
	prevY = y;
	x += vx;
	y += vy;

	onGround = false;

	// 左右ラップ
	if (x < 0.0f) x = 1280.0f;
	if (x > 1280.0f) x = 0.0f;

	float outY = 0.0f;
	if (stage.CheckLanding(x, y, prevY, PLAYER_HIT_W, 20.0f, outY)) {
		y = outY;
		vy = 0.0f;
		onGround = true;
		jumpCount = 2;
	}
}

void Player::ApplyGravity() {
	vy += GRAVITY;
}

void Player::Draw(Weapon* weapons) {
	// 点滅処理
	if (isBlinking) {
		int interval = (blinkTimer > 60) ? 10 : 5;
		if (blinkTimer % interval < interval / 2) return;
	}
	if (isBlinking) {
		DrawCircleAA(x, y - PLAYER_HIT_CY, 150.0f, 64, GetColor(255, 0, 0), FALSE);
	}

	float drawX = x - 48.0f;
	float drawY = y - 128.0f;
	if (facingRight) {
		DrawExtendGraphF(drawX, drawY, drawX + 96, drawY + 128, playerImage[animFrame], TRUE);
	}
	else {
		DrawExtendGraphF(drawX + 96, drawY, drawX, drawY + 128, playerImage[animFrame], TRUE);
	}
	// 武器の追従描画
	if (holdingWeaponIndex != -1) {
		weaponDrawX = facingRight ? x + 60.0f : x - 60.0f;
		weaponDrawY = y - 64.0f;

		if (animFrame == 1) {
			weaponDrawY -= 60.0f;
		}
		float weaponAngle = 0.0f;

		if (isReadyThrow) {
			weaponDrawX = facingRight ? x - 20.0f : x + 20.0f;
			weaponDrawY = y - 110.0f;
			weaponAngle = facingRight ? -2.5f : 2.5f;
		}
		else if (attacking && WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].isRanged) {
			if (attackTimer > 7) {
				weaponDrawY = y - 110.0f;
				weaponAngle = facingRight ? -0.8f : 0.8f;
			}
			else {
				weaponDrawY = y - 20.0f;
				weaponAngle = facingRight ? 0.8f : -0.8f;
			}
		}

		DrawRotaGraphF(weaponDrawX, weaponDrawY, 1.5, weaponAngle,
			weapons[holdingWeaponIndex].weaponImage, TRUE, !facingRight);
	}

#ifdef _DEBUG
	// プレイヤーの当たり判定（緑）
	DrawBoxAA(
		x - PLAYER_HIT_W / 2, y - PLAYER_HIT_CY - PLAYER_HIT_H / 2,
		x + PLAYER_HIT_W / 2, y - PLAYER_HIT_CY + PLAYER_HIT_H / 2,
		GetColor(0, 255, 0), FALSE
	);
	// 攻撃判定（赤）
	if (attacking && attackTimer < 7) {
		if (holdingWeaponIndex != -1 && WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].isRanged) {
			float atkW = WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].hitW;
			float atkH = WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].hitH;
			DrawBoxAA(weaponDrawX - atkW / 2, weaponDrawY - atkH / 2,
				weaponDrawX + atkW / 2, weaponDrawY + atkH / 2,
				GetColor(255, 0, 0), FALSE);
		}
		else {
			// 素手の判定
			float atkX = facingRight ? x + 50.0f : x - 50.0f;
			float atkY = y - 50.0f;
			DrawBoxAA(atkX - 20.0f, atkY - 40.0f, atkX + 20.0f, atkY + 40.0f, GetColor(255, 0, 0), FALSE);
		}
	}

#endif
}

bool Player::CheckAttackHit(Player& other, Weapon* weapons) {
	if (!attacking) return false;
	if (attackTimer >= 7) return false;

	float atkX, atkY, atkW, atkH;

	if (holdingWeaponIndex != -1 && WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].isRanged) {
		atkX = weaponDrawX;
		atkY = weaponDrawY;
		atkW = WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].hitW;
		atkH = WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].hitH;
	}
	else {
		// 素手の判定：今まで通り
		atkX = facingRight ? x + 50.0f : x - 50.0f;
		atkY = y - 50.0f;
		atkW = 40.0f;
		atkH = 80.0f;
	}

	if (CheckHit(atkX, atkY, atkW, atkH,
		other.x, other.y - PLAYER_HIT_CY,
		PLAYER_HIT_W, PLAYER_HIT_H)) {
		attacking = false;
		winCount++;
		return true;
	}
	return false;
}

bool Player::CheckHit(float ax, float ay, float aw, float ah,
	float bx, float by, float bw, float bh) {
	// ax,ay と bx,by は「中心座標」として扱う
	return fabsf(ax - bx) < (aw + bw) / 2 &&
		fabsf(ay - by) < (ah + bh) / 2;
}