#include "Player.h"
#include "Config.h"
#include <cmath>

void Player::Init(float startX, float startY, int id, bool facingR, ImageManager& imgMgr, int keepWinCount) {
	x = startX;
	y = startY;
	vx = vy = 0.0f;
	moveSpeed = 5.0f;
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
	pikohanRespawnTimer = 0;
	prevAttackKey = false;
	prevThrowKey = false;
	prevLeftKey = false;
	prevRightKey = false;
	isReadyThrow = false;
	isGlowing = false;
	canAttack = true;
	mashCount = 0;
	mashDecay = 0;
	airTime = 0;
	speedDownTimer = 0;
	stanTimer = 0;
	isStunned = false;
	hasShield = false;
	freezeAnim = false;
	for (int i = 0; i < 7; i++) {
		playerImage[i] = (id == 1) ? imgMgr.player1[i] : imgMgr.player2[i];
		playerGlowImage[i] = imgMgr.player3[i];
	}
}

// 左右移動とキー入力処理
void Player::UpdateInput(const RestrictionManager& restrictions) {
	if (isStunned) { vx = 0; return; }
	if (attacking){ vx = 0; return; }
	if (restrictions.IsActive(REST_JUMP_LIMIT) && onGround) { vx = 0; return; }

	// 刹那の見切り：向き変えのみ可、移動禁止
	if (restrictions.IsActive(REST_SETSUNA)) {
		vx = 0;
		if (PlayerID == 1) {
			if (CheckHitKey(KEY_INPUT_A)) facingRight = false;
			if (CheckHitKey(KEY_INPUT_D)) facingRight = true;
		}
		else {
			if (CheckHitKey(KEY_INPUT_LEFT)) facingRight = false;
			if (CheckHitKey(KEY_INPUT_RIGHT)) facingRight = true;
		}
		return;
	}

	bool mashMove = restrictions.IsActive(REST_MASH_MOVE);

	if (mashMove) {
		// 連打カウントの減衰
		if (mashDecay > 0) mashDecay--;
		else if (mashCount > 0) mashCount--;

		// vxを毎フレーム減衰させる（摩擦）
		vx *= 0.95f;

		if (PlayerID == 1) {
			bool leftKey = CheckHitKey(KEY_INPUT_A);
			bool rightKey = CheckHitKey(KEY_INPUT_D);
			if (leftKey && !prevLeftKey) {
				mashCount = min(mashCount + 1, 5);
				mashDecay = 8;
				vx -= (moveSpeed + mashCount * 1.5f);
				facingRight = false;
			}
			if (rightKey && !prevRightKey) {
				mashCount = min(mashCount + 1, 5);
				mashDecay = 8;
				vx += (moveSpeed + mashCount * 1.5f);
				facingRight = true;
			}
			prevLeftKey = leftKey;
			prevRightKey = rightKey;
		}
		else if (PlayerID == 2) {
			bool leftKey = CheckHitKey(KEY_INPUT_LEFT);
			bool rightKey = CheckHitKey(KEY_INPUT_RIGHT);
			if (leftKey && !prevLeftKey) {
				mashCount = min(mashCount + 1, 5);
				mashDecay = 8;
				vx -= (moveSpeed + mashCount * 1.5f);
				facingRight = false;
			}
			if (rightKey && !prevRightKey) {
				mashCount = min(mashCount + 1, 5);
				mashDecay = 8;
				vx += (moveSpeed + mashCount * 1.5f);
				facingRight = true;
			}
			prevLeftKey = leftKey;
			prevRightKey = rightKey;
		}
	}
	else {
		vx = 0;
		if (PlayerID == 1) {
			if (CheckHitKey(KEY_INPUT_A)) { vx = isBlinking ? -(moveSpeed + 8.0f) : -moveSpeed; facingRight = false; }
			if (CheckHitKey(KEY_INPUT_D)) { vx = isBlinking ? (moveSpeed + 8.0f) : moveSpeed; facingRight = true; }
			if (restrictions.IsActive(REST_GRAVITY_ZERO)) {
				vy = 0;
				if (CheckHitKey(KEY_INPUT_W)) vy = -moveSpeed - 8.0f;
				if (CheckHitKey(KEY_INPUT_S)) vy = moveSpeed + 8.0f;
			}
		}
		else if (PlayerID == 2) {
			if (CheckHitKey(KEY_INPUT_LEFT)) { vx = isBlinking ? -(moveSpeed + 8.0f) : -moveSpeed; facingRight = false; }
			if (CheckHitKey(KEY_INPUT_RIGHT)) { vx = isBlinking ? (moveSpeed + 8.0f) : moveSpeed; facingRight = true; }
			if (restrictions.IsActive(REST_GRAVITY_ZERO)) {
				vy = 0;
				if (CheckHitKey(KEY_INPUT_UP)) vy = -moveSpeed - 8.0f;
				if (CheckHitKey(KEY_INPUT_DOWN)) vy = moveSpeed + 8.0f;
			}
		}
	}
}

// ジャンプの処理
void Player::UpdateJump(const RestrictionManager& restrictions) {
	if (restrictions.IsActive(REST_SETSUNA)) return;
	if (isStunned) { vy = 0; return; }
	bool jumpKey = false;
	if (PlayerID == 1) jumpKey = CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_SPACE);
	if (PlayerID == 2) jumpKey = CheckHitKey(KEY_INPUT_UP);

	if (restrictions.IsActive(REST_HOVER_JUMP)) {
		// ホバリング：ジャンプ回数無制限・ジャンプ力低め
		if (jumpKey && !prevJumpKey) {
			vy = jumpPower * 0.9f;
		}
		// 空中にいる時間で落下加速
		if (!onGround) {
			airTime++;
			vy += airTime * 0.01f;
		}
		else {
			airTime = 0;
		}
	}
	else {
		// 通常ジャンプ
		if (jumpKey && !prevJumpKey && jumpCount > 0) {
			vy = jumpPower;
			jumpCount--;
		}
	}
	prevJumpKey = jumpKey;
}

// 攻撃モーション全体のフレーム数を取得する関数
int Player::GetAttackFrames(Weapon* weapons) {
	if (holdingWeaponIndex == -1)
		return BARE_HAND_CHARGE_FRAMES + BARE_HAND_ATTACK_FRAMES;
	WeaponType type = (WeaponType)weapons[holdingWeaponIndex].weaponType;
	return WEAPON_DATA[type].chargeFrames + WEAPON_DATA[type].attackFrames;
}

// 攻撃の処理
void Player::UpdateAttack(Weapon* weapons, const RestrictionManager& restrictions) {
	if (isStunned) return;
	if (!canAttack) return;
	// メテオ中は素手攻撃不可
	if (restrictions.IsActive(REST_METEOR) && holdingWeaponIndex == -1) return;
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
			isReadyThrow = false;
			attackTimer = 15;
		}
		else if (attackKey && !prevAttackKey && attackTimer == 0) {
			attacking = true;
			attackTimer = GetAttackFrames(weapons);
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
			isReadyThrow = false;
			attackTimer = 15;
		}
		else if (attackKey && !prevAttackKey && attackTimer == 0) {
			attacking = true;
			attackTimer = GetAttackFrames(weapons);
		}
		prevAttackKey = attackKey;
		prevThrowKey = throwKey;
	}
}

void Player::UpdateAnim() {
	if (freezeAnim) return;
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
	// 減速タイマー
	if (speedDownTimer > 0) {
		speedDownTimer--;
		if (speedDownTimer <= 0) {
			moveSpeed = 5.0f; // 元に戻す
		}
	}
	// スタンタイマー
	if (stanTimer > 0) {
		stanTimer--;
		if (stanTimer <= 0) {
			isStunned = false;
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

void Player::Update(Stage& stage, Weapon* weapons, const RestrictionManager& restrictions) {
	UpdateInput(restrictions);
	ApplyGravity(restrictions);
	UpdatePosition(stage);
	UpdateJump(restrictions);
	UpdateAttack(weapons, restrictions);
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

void Player::ApplyGravity(const RestrictionManager& restrictions) {
	if (restrictions.IsActive(REST_SETSUNA)) return;
	if (restrictions.IsActive(REST_GRAVITY_ZERO)) return; // 重力ゼロなら何もしない

	vy += GRAVITY;
	if (restrictions.IsActive(REST_GRAVITY_CONTROL)|| isBlinking && !onGround) {
		bool downKey = (PlayerID == 1)
			? CheckHitKey(KEY_INPUT_S)
			: CheckHitKey(KEY_INPUT_DOWN);
		if (downKey) vy += GRAVITY * 4.0f;
	}
}

void Player::Draw(Weapon* weapons, ImageManager& imgMgr) {
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
	int* drawImage = isGlowing ? playerGlowImage : playerImage;
	if (facingRight) {
		DrawExtendGraphF(drawX, drawY, drawX + 96, drawY + 128, drawImage[animFrame], TRUE);
	}
	else {
		DrawExtendGraphF(drawX + 96, drawY, drawX, drawY + 128, drawImage[animFrame], TRUE);
	}
	// スタンエフェクト
	if (isStunned) {
		int stanFrame = (stanTimer / 8) % 2; // 8フレームごとに切り替え
		DrawExtendGraphF(
			x - 32.0f, y - PLAYER_HIT_CY - 120.0f,
			x + 32.0f, y - PLAYER_HIT_CY - 56.0f,
			imgMgr.stan[stanFrame], TRUE
		);
	}

	if (hasShield) {
		DrawExtendGraphF(
			x - 48.0f, y - PLAYER_HIT_CY - 64.0f,
			x + 48.0f, y - PLAYER_HIT_CY + 64.0f,
			imgMgr.shield, TRUE
		);
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
				// メメントモリだけY軸を上げる
				if (weapons[holdingWeaponIndex].weaponType == WEAPON_MEMENTO_MORI) {
					weaponDrawY = y - 43.0f;
				}
				else {
					weaponDrawY = y - 20.0f;
				}
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
	int debugCharge = 7;
	int debugAtkFrames = 8;
	if (holdingWeaponIndex != -1) {
		WeaponType type = (WeaponType)weapons[holdingWeaponIndex].weaponType;
		debugCharge = WEAPON_DATA[type].chargeFrames;
		debugAtkFrames = WEAPON_DATA[type].attackFrames;
	}
	if (attacking && attackTimer < debugCharge && attackTimer >= debugCharge - debugAtkFrames) {
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

void Player::EnterStun() {
	stanTimer = 120; // 2秒
	isStunned = true;
}

bool Player::CheckAttackHit(Player& other, Weapon* weapons) {
	if (!attacking) return false;

	int charge = BARE_HAND_CHARGE_FRAMES;
	int atkFrames = BARE_HAND_ATTACK_FRAMES;
	if (holdingWeaponIndex != -1) {
		WeaponType type = (WeaponType)weapons[holdingWeaponIndex].weaponType;
		charge = WEAPON_DATA[type].chargeFrames;
		atkFrames = WEAPON_DATA[type].attackFrames;
	}
	// chargeFrames以上 または attackFramesより少ない残り時間なら判定なし
	if (attackTimer >= charge) return false;
	if (attackTimer < charge - atkFrames) return false;

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