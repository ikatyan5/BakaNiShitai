#include "Player.h"
#include "Config.h"
#include <cmath>
#include <algorithm>

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
	justJumped = false;
	justBareAttacked = false;
	isDashing = false;
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
	dashAttack = false;
	useGamepad = false;
	reverseTimer = 0;
	tensaiAnimTimer = 0;
	knockbackCount = 0;
	knockbackDecayTimer = 0;
	knockbackVx = 0.0f;
	knockbackTimer = 0;
	isKnockedBack = false;
	padID = DX_INPUT_PAD1;

	for (int i = 0; i < 7; i++) {
		playerImage[i] = (id == 1) ? imgMgr.player1[i] : imgMgr.player2[i];
		playerGlowImage[i] = imgMgr.player3[i];
	}
}

// 左右移動とキー入力処理
void Player::UpdateInput(const RestrictionManager& restrictions, Weapon* weapons){
	// ノックバック中は通常入力を無効化
	if (isKnockedBack) {
		knockbackTimer--;
		knockbackVx *= 0.85f;

		bool leftKey = false, rightKey = false;
		if (useGamepad) {
			int pad = GetJoypadInputState(padID);
			int stickX = 0, stickY = 0;
			GetJoypadAnalogInput(&stickX, &stickY, padID);
			leftKey = (pad & PAD_INPUT_LEFT) || stickX < -500;
			rightKey = (pad & PAD_INPUT_RIGHT) || stickX > 500;
		}
		else if (PlayerID == 1) {
			leftKey = CheckHitKey(KEY_INPUT_A);
			rightKey = CheckHitKey(KEY_INPUT_D);
		}
		else {
			leftKey = CheckHitKey(KEY_INPUT_LEFT);
			rightKey = CheckHitKey(KEY_INPUT_RIGHT);
		}

		if (knockbackVx < 0 && rightKey) knockbackVx *= 0.88f;
		if (knockbackVx > 0 && leftKey)  knockbackVx *= 0.88f;

		vx = knockbackVx;
		if (knockbackTimer <= 0) isKnockedBack = false;
		return;
	}

	if (isDashing) return;

	if (isStunned) { vx = 0; return; }
	if (attacking) {
		if (dashAttack) {
			int charge = (holdingWeaponIndex == -1)
				? BARE_HAND_CHARGE_FRAMES
				: WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].chargeFrames;
			if (attackTimer < charge) {
				vx = 0;
				dashAttack = false;
			}
		}
		else {
			vx = 0;
		}
		if (restrictions.IsActive(REST_GRAVITY_ZERO)) vy = 0;
		return;
	}
	if (restrictions.IsActive(REST_JUMP_LIMIT) && onGround) { vx = 0; return; }

	// 刹那の見切り：向き変えのみ可、移動禁止
	if (restrictions.IsActive(REST_SETSUNA)) {
		vx = 0;
		if (useGamepad) {
			int pad = GetJoypadInputState(padID);
			int stickX = 0, stickY = 0;
			GetJoypadAnalogInput(&stickX, &stickY, padID);
			if ((pad & PAD_INPUT_LEFT) || stickX < -500)  facingRight = false;
			if ((pad & PAD_INPUT_RIGHT) || stickX > 500) facingRight = true;
		}
		else if (PlayerID == 1) {
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
		vx *= 0.95f;

		bool leftKey, rightKey;
		if (useGamepad) {
			int pad = GetJoypadInputState(padID);
			int stickX = 0, stickY = 0;
			GetJoypadAnalogInput(&stickX, &stickY, padID);
			leftKey = (pad & PAD_INPUT_LEFT) || stickX < -500;
			rightKey = (pad & PAD_INPUT_RIGHT) || stickX > 500;
		}
		else if (PlayerID == 1) {
			leftKey = CheckHitKey(KEY_INPUT_A);
			rightKey = CheckHitKey(KEY_INPUT_D);
		}
		else {
			leftKey = CheckHitKey(KEY_INPUT_LEFT);
			rightKey = CheckHitKey(KEY_INPUT_RIGHT);
		}

		if (reverseTimer > 0) std::swap(leftKey, rightKey);

		if (leftKey && !prevLeftKey) {
			float randomSpeed = (float)(rand() % 20 + 1);
			vx -= randomSpeed;
			facingRight = false;
		}
		if (rightKey && !prevRightKey) {
			float randomSpeed = (float)(rand() % 20 + 1);
			vx += randomSpeed;
			facingRight = true;
		}
		prevLeftKey = leftKey;
		prevRightKey = rightKey;
	}
	else {
		vx = 0;
		bool leftKey, rightKey, upKey, downKey;

		if (useGamepad) {
			int pad = GetJoypadInputState(padID);
			int stickX = 0, stickY = 0;
			GetJoypadAnalogInput(&stickX, &stickY, padID);
			leftKey = (pad & PAD_INPUT_LEFT) || stickX < -500;
			rightKey = (pad & PAD_INPUT_RIGHT) || stickX > 500;
			upKey = (pad & PAD_INPUT_UP) || stickY < -500;
			downKey = (pad & PAD_INPUT_DOWN) || stickY > 500;
		}
		else if (PlayerID == 1) {
			leftKey = CheckHitKey(KEY_INPUT_A);
			rightKey = CheckHitKey(KEY_INPUT_D);
			upKey = CheckHitKey(KEY_INPUT_W);
			downKey = CheckHitKey(KEY_INPUT_S);
		}
		else {
			leftKey = CheckHitKey(KEY_INPUT_LEFT);
			rightKey = CheckHitKey(KEY_INPUT_RIGHT);
			upKey = CheckHitKey(KEY_INPUT_UP);
			downKey = CheckHitKey(KEY_INPUT_DOWN);
		}

		if (leftKey) { vx = isBlinking ? -(moveSpeed + 8.0f) : -moveSpeed; facingRight = false; }
		if (rightKey) { vx = isBlinking ? (moveSpeed + 8.0f) : moveSpeed; facingRight = true; }

		if (restrictions.IsActive(REST_GRAVITY_ZERO)) {
			vy = 0;
			if (upKey)   vy = -moveSpeed - 6.0f;
			if (downKey) vy = moveSpeed + 6.0f;
			if (leftKey)  vx = isBlinking ? -(moveSpeed + 8.0f) : -(moveSpeed + 8.0f);
			if (rightKey) vx = isBlinking ? (moveSpeed + 8.0f) : (moveSpeed + 8.0f);
		}
	}
}

// ジャンプの処理
void Player::UpdateJump(const RestrictionManager& restrictions) {
	if (restrictions.IsActive(REST_SETSUNA)) return;
	if (isStunned) { vy = 0; return; }
	bool jumpKey = false;
	if (useGamepad) {
		int pad = GetJoypadInputState(padID);
		jumpKey = ((pad & PAD_INPUT_1) || (pad & PAD_INPUT_2));
	}
	else if (PlayerID == 1) {
		jumpKey = CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_SPACE);
	}
	else {
		jumpKey = CheckHitKey(KEY_INPUT_UP);
	}

	if (restrictions.IsActive(REST_HOVER_JUMP)) {
		// ホバリング：ジャンプ回数無制限・ジャンプ力低め
		if (jumpKey && !prevJumpKey) {
			vy = jumpPower * 0.9f;
			justJumped = true;
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
			justJumped = true;
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

void Player::AddKnockbackCount() {
	knockbackCount = 1; // 0か1の2段階のみ
}

void Player::ApplyKnockback(float kbVx, float kbVy) {
	knockbackVx = kbVx;
	vy = kbVy;  // 上方向の初速をvyに直接セット
	knockbackTimer = 60;
	isKnockedBack = true;
	knockbackCount = 0;
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

	if (useGamepad) {
		int pad = GetJoypadInputState(padID);
		bool modKey = ((pad & PAD_INPUT_5) || (pad & PAD_INPUT_6));
		bool attackKey = ((pad & PAD_INPUT_3) || (pad & PAD_INPUT_4));
		bool throwKey = modKey && attackKey;
		isReadyThrow = modKey && holdingWeaponIndex != -1;
		if (throwKey && !prevThrowKey && attackTimer == 0 && holdingWeaponIndex != -1) {
			wantThrow = true;
			isReadyThrow = false;
			attackTimer = 15;
		}
		else if (attackKey && !prevAttackKey && attackTimer == 0) {
			attacking = true;
			attackTimer = GetAttackFrames(weapons);
			if (holdingWeaponIndex == -1) justBareAttacked = true; // 素手攻撃の音用
			if (restrictions.IsActive(REST_THROW_NO_DAMAGE)) {
				bool movingKey = (vx != 0);
				dashAttack = movingKey;
			}
		}
		prevAttackKey = attackKey;
		prevThrowKey = throwKey;
	}
	else if (PlayerID == 1) {
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
			if (holdingWeaponIndex == -1) justBareAttacked = true; // 素手攻撃の音用
			if (restrictions.IsActive(REST_THROW_NO_DAMAGE)) {
				bool movingKey = (vx != 0);
				dashAttack = movingKey;
			}
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
			if (holdingWeaponIndex == -1) justBareAttacked = true; // 素手攻撃の音用
			if (restrictions.IsActive(REST_THROW_NO_DAMAGE)) {
				bool movingKey = (vx != 0);
				dashAttack = movingKey;
			}
		}
		prevAttackKey = attackKey;
		prevThrowKey = throwKey;
	}
}

void Player::UpdateAnim(Weapon* weapons) {
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
	// 操作反転タイマー
	if (reverseTimer > 0) reverseTimer--;
	// スタンタイマー
	if (stanTimer > 0) {
		stanTimer--;
		if (stanTimer <= 0) {
			isStunned = false;
		}
	}
	// ノックバック減衰タイマー
	if (knockbackDecayTimer > 0) {
		knockbackDecayTimer--;
		if (knockbackDecayTimer <= 0 && knockbackCount > 0) {
			knockbackCount--;
			if (knockbackCount > 0) knockbackDecayTimer = 300;
		}
	}
	animTimer++;
	tensaiAnimTimer++;

	if (isReadyThrow) {
		animFrame = 5;
	}
	else if (attacking) {
		int charge = (holdingWeaponIndex == -1)
			? BARE_HAND_CHARGE_FRAMES
			: WEAPON_DATA[weapons[holdingWeaponIndex].weaponType].chargeFrames;

		if (animTimer < charge) animFrame = 3;  // 構え中
		else animFrame = 4;                      // 攻撃中
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
	UpdateInput(restrictions, weapons);
	ApplyGravity(restrictions);
	UpdatePosition(stage);
	UpdateJump(restrictions);
	UpdateAttack(weapons, restrictions);
	UpdateAnim(weapons);
}

void Player::UpdatePosition(Stage& stage) {
	prevY = y;
	x += vx;
	y += vy;

	onGround = false;

	if (y < 50.0f) {
		y = 50.0f;
		vy = 0.0f;
	}

	// 左右ラップ（ノックバック中は無効）
	if (!isKnockedBack) {
		if (x < 0.0f) x = 1280.0f;
		if (x > 1280.0f) x = 0.0f;
	}

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
	if (restrictions.IsActive(REST_GRAVITY_ZERO)) return;

	static const float GRAVITY_LEVELS[] = {
		GRAVITY * 0.1f,
		GRAVITY * 0.4f,
		GRAVITY * 1.0f,
		GRAVITY * 2.0f,
		GRAVITY * 4.0f,
	};

	float currentGravity = restrictions.IsActive(REST_GRAVITY_INSANE)
		? GRAVITY_LEVELS[gravityInsaneLevel]
		: GRAVITY;

	vy += currentGravity;

	// 急降下
	bool downKey = false;
	if (useGamepad) {
		int pad = GetJoypadInputState(padID);
		int stickX = 0, stickY = 0;
		GetJoypadAnalogInput(&stickX, &stickY, padID);
		downKey = (pad & PAD_INPUT_DOWN) || stickY > 500;
	}
	else if (PlayerID == 1) {
		downKey = CheckHitKey(KEY_INPUT_S);
	}
	else {
		downKey = CheckHitKey(KEY_INPUT_DOWN);
	}
	if (downKey && !onGround) vy += GRAVITY * 4.0f;
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
	// 左右反転エフェクト
	if (reverseTimer > 0) {
		int gyakuFrame = (reverseTimer / 8) % 2;
		DrawExtendGraphF(
			x - 32.0f, y - PLAYER_HIT_CY - 120.0f,
			x + 32.0f, y - PLAYER_HIT_CY - 56.0f,
			imgMgr.gyaku[gyakuFrame], TRUE
		);
	}
	// テンサイエフェクト
	if (holdingWeaponIndex != -1 &&
		weapons[holdingWeaponIndex].weaponType == WEAPON_TENSAI_TSUE) {
		int tensaiFrame = (tensaiAnimTimer / 8) % 2;
		DrawExtendGraphF(
			x - 32.0f, y - PLAYER_HIT_CY - 140.0f,
			x + 32.0f, y - PLAYER_HIT_CY - 76.0f,
			imgMgr.tensai[tensaiFrame], TRUE
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

	TCHAR kbBuf[8];
	wsprintf(kbBuf, _T("x%d"), knockbackCount);
	DrawString((int)(x - 10), (int)(y - PLAYER_HIT_CY - 160), kbBuf, GetColor(255, 50, 50));

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

void Player::DrawDecoy(float dx, float dy, bool faceRight, ImageManager& imgMgr) {
	// 本体の Draw から「体の絵」部分だけを抜き出したもの。
	// 画像(playerImage/playerGlowImage)とアニメ枚数(animFrame)は本体に追従するので、
	// 黄ポーション中は本体ごと分身も黄色く光る。向きだけ引数で個別に渡す。
	float drawX = dx - 48.0f;
	float drawY = dy - 128.0f;
	int* drawImage = isGlowing ? playerGlowImage : playerImage;
	if (faceRight) {
		DrawExtendGraphF(drawX, drawY, drawX + 96, drawY + 128, drawImage[animFrame], TRUE);
	}
	else {
		DrawExtendGraphF(drawX + 96, drawY, drawX, drawY + 128, drawImage[animFrame], TRUE);
	}
}

void Player::SwapImageWith(Player& other) {
	for (int i = 0; i < 7; i++) {
		std::swap(playerImage[i], other.playerImage[i]);
		std::swap(playerGlowImage[i], other.playerGlowImage[i]);
	}
}

// ピコハン近接
void Player::EnterStun() {
	stanTimer = 120; // 2秒
	isStunned = true;
}

// ピコハン投げた時
void Player::EnterStunShort() {
	stanTimer = 60; // 1秒
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