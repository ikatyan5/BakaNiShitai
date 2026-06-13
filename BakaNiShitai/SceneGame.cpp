#include "SceneGame.h"
#include "Config.h"
#include "UIConfig.h"
#include "DebugConfig.h"
#include <cmath>

static const TCHAR* RESTRICTION_NAMES[] = {
    _T("なにもなし！"),
    _T("重力がなくなった！"),
    _T("ジャンプ回数が無制限に！"),
    _T("武器を投げてもダメージがないぞ！"),
    _T("近接ダメージがゼロだ！"),
    _T("杖ばっか降ってくるぞ！"),
    _T("ブーメランばっか降ってくるぞ！"),
    _T("！マークが出たら攻撃だ！"),
    _T("重力がおかしくなったぞ！"),
    _T("画面がひっくり返るぞ！"),
    _T("横移動は連打しろ！"),
    _T("隕石が降ってくるぞ 相手をスタンさせよう！"),
    _T("強いやつから逃げ切れ 触れられたら負けだぞ！"),
    _T("地上で移動できないぞ！"),
    _T("なんか画面おかしくね？"),
};

static void DrawRotatedUI(float cx, float cy, float w, float h, float angle, int img) {
    float hw = w / 2.0f;
    float hh = h / 2.0f;
    float c = cosf(angle);
    float s = sinf(angle);
    // 四隅（中心基準）を回転
    // 左上, 右上, 右下, 左下
    float xs[4] = { -hw,  hw,  hw, -hw };
    float ys[4] = { -hh, -hh,  hh,  hh };
    float px[4], py[4];
    for (int i = 0; i < 4; i++) {
        px[i] = cx + xs[i] * c - ys[i] * s;
        py[i] = cy + xs[i] * s + ys[i] * c;
    }
    // DrawModiGraphFは左上→右上→右下→左下の順
    DrawModiGraphF(px[0], py[0], px[1], py[1], px[2], py[2], px[3], py[3], img, TRUE);
}

void SceneGame::Init(ImageManager& imgMgr_, GameSettings& settings) {
    imgMgr = &imgMgr_;
    state = STATE_COUNTDOWN;
    countdownTimer = 180; // 3秒
    JUDGE = false;
    HIT_TIMER = 0;
    RESULT_TIMER = 0;
    weaponSpawnTimer = 0;
    nextScene = SCENE_NONE;

    player1.useGamepad = settings.p1UseGamepad;
    player1.padID = settings.p1PadID;
    player2.useGamepad = settings.p2UseGamepad;
    player2.padID = settings.p2PadID;

    matchTime = DEFAULT_TIME;
    timeTimer = matchTime * 60;

    mementoMoriTimer = 0;
    mementoMoriShooterID = 0;
    mementoMoriWinnerID = 0;
    mementoMoriPending = false;

    wallEndLeft = false;
    wallEndRight = false;
    wallEndTimer = 180 + rand() % 300;

    currentTex = MakeScreen(1280, 920, TRUE);
    prevTex = MakeScreen(1280, 920, TRUE);
    blurMode = 0;
    blurTimer = 0;

    p1Glowing = false;
    p2Glowing = false;

    setsunaPhase = SETSUNA_SLIDE;
    setsunaPhaseTimer = 0;
    setsunaP1UIX = 1280.0f;
    setsunaP2UIX = -1280.0f;

    setsunaSignVisible = false;
    this->settings = &settings;
    itemManager.Init(*imgMgr);
    orbManager.Init(*imgMgr);
    meteorManager.Init();
    adManager.Init(*imgMgr);
    restrictionManager.Init();

#ifdef _DEBUG
    restrictionManager.SelectRandom(); // デバッグ時は最初から制限をかける
#endif
    stage.Init(1);
    for (int i = 0; i < WEAPON_MAX; i++) {
        weapons[i].Init(WEAPON_KAMA, *imgMgr);
    }
    InitFallingUI();
    InitPlayers(false);
    gravityInsaneLevel = 2;
    gravityInsaneTimer = 120 + rand() % 60;
    player1.gravityInsaneLevel = gravityInsaneLevel;
    player2.gravityInsaneLevel = gravityInsaneLevel;
}

void SceneGame::InitPlayers(bool keepWinCount) {
    bool p1Right = rand() % 2 == 0;
    float leftX = 200.0f;
    float rightX = 1080.0f;

    int p1Win = keepWinCount ? player1.winCount : 0;
    int p2Win = keepWinCount ? player2.winCount : 0;
    if (restrictionManager.IsActive(REST_SETSUNA)) {
        player1.Init(p1Right ? rightX : leftX, 530.0f, 1, p1Right, *imgMgr, p1Win);
        player2.Init(p1Right ? leftX : rightX, 530.0f, 2, !p1Right, *imgMgr, p2Win);
        
        player1.useGamepad = settings->p1UseGamepad;
        player1.padID = settings->p1PadID;
        player2.useGamepad = settings->p2UseGamepad;
        player2.padID = settings->p2PadID;
    }

    else {
        player1.Init(p1Right ? rightX : leftX, 360.0f, 1, p1Right, *imgMgr, p1Win);
        player2.Init(p1Right ? leftX : rightX, 360.0f, 2, !p1Right, *imgMgr, p2Win);
    
        player1.useGamepad = settings->p1UseGamepad;
        player1.padID = settings->p1PadID;
        player2.useGamepad = settings->p2UseGamepad;
        player2.padID = settings->p2PadID;
    }

    // 刹那の見切り：両プレイヤーにメメントモリを持たせる
    if (restrictionManager.IsActive(REST_SETSUNA)) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                weapons[i].Init(WEAPON_MEMENTO_MORI, *imgMgr);
                weapons[i].weaponState = Weapon::WEAPON_HELD;
                player1.holdingWeaponIndex = i;
                break;
            }
        }
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                weapons[i].Init(WEAPON_MEMENTO_MORI, *imgMgr);
                weapons[i].weaponState = Weapon::WEAPON_HELD;
                player2.holdingWeaponIndex = i;
                break;
            }
        }
    }

    // 弱い側にピコハンを持たせる
    if (restrictionManager.IsActive(REST_HYPETSUYOI)) {
        // 負けてる側優先、同点ならランダム
        if (player1.winCount < player2.winCount) hyperPlayerID = 1;
        else if (player2.winCount < player1.winCount) hyperPlayerID = 2;
        else hyperPlayerID = (rand() % 2) + 1;

        itemManager.hyperPlayerID = hyperPlayerID;

        Player& hyperPlayer = (hyperPlayerID == 1) ? player1 : player2;
        hyperPlayer.canAttack = false;
        hyperPlayer.moveSpeed = 5.0f * 1.3f;

        // 弱い側にピコハンを持たせる
        int weakID = (hyperPlayerID == 1) ? 2 : 1;
        Player& weakPlayer = (weakID == 1) ? player1 : player2;
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                weapons[i].Init(WEAPON_PIKOHAN, *imgMgr);
                weapons[i].weaponState = Weapon::WEAPON_HELD;
                weakPlayer.holdingWeaponIndex = i;
                break;
            }
        }
    }

    // メテオ中は最初からピコハンを持たせる
    if (restrictionManager.IsActive(REST_METEOR)) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                weapons[i].Init(WEAPON_PIKOHAN, *imgMgr);
                weapons[i].weaponState = Weapon::WEAPON_HELD;
                player1.holdingWeaponIndex = i;
                break;
            }
        }
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                weapons[i].Init(WEAPON_PIKOHAN, *imgMgr);
                weapons[i].weaponState = Weapon::WEAPON_HELD;
                player2.holdingWeaponIndex = i;
                break;
            }
        }
    }
}

void SceneGame::InitFallingUI() {
    // HP P1：左上(50,20)+サイズ(400,180) → 中心
    hpUI[0].baseX = 50.0f + UI_HP_W / 2.0f;
    hpUI[0].baseY = 20.0f + UI_HP_H / 2.0f;
    // HP P2：右端基準
    hpUI[1].baseX = 1280.0f - 50.0f - UI_HP_W / 2.0f;
    hpUI[1].baseY = 20.0f + UI_HP_H / 2.0f;

    // スコア P1：3個の塊の中心
    float p1ScoreW = WINNING_SCORE * UI_SCORE_W;
    scoreUI[0].baseX = UI_P1_SCORE_X + p1ScoreW / 2.0f;
    scoreUI[0].baseY = UI_SCORE_Y + UI_SCORE_H / 2.0f;
    // スコア P2
    float p2StartX = UI_P2_SCORE_X - WINNING_SCORE * UI_SCORE_W;
    scoreUI[1].baseX = p2StartX + p1ScoreW / 2.0f;
    scoreUI[1].baseY = UI_SCORE_Y + UI_SCORE_H / 2.0f;

    // 時間UI：十の位(530)と一の位(638)の塊の中心
    timeUI.baseX = (UI_TIME_X + (638.0f + UI_NUMBER_SIZE)) / 2.0f;
    timeUI.baseY = UI_TIME_Y + UI_NUMBER_SIZE / 2.0f;

    // 4つとも状態リセット
    FallingUI* all[5] = { &hpUI[0], &hpUI[1], &scoreUI[0], &scoreUI[1], &timeUI };
    for (int i = 0; i < 5; i++) {
        all[i]->x = all[i]->baseX;
        all[i]->y = all[i]->baseY;
        all[i]->vy = 0.0f;
        all[i]->angle = 0.0f;
        all[i]->angle2 = 0.0f;
        all[i]->count = 0;
        all[i]->falling = false;
        all[i]->landed = false;
    }
    uiShakeTimer = 0;
}

void SceneGame::ResetGame(bool keepWinCount) {
    isDraw = false;
    p1HpIndex = 0;
    p2HpIndex = 0;
    state = STATE_COUNTDOWN;
    countdownTimer = 180; // 3秒
    p1Glowing = false;
    p2Glowing = false;
    timeTimer = matchTime * 60;
    itemManager.Init(*imgMgr);
    orbManager.Init(*imgMgr);
    meteorManager.Init();
    adManager.Init(*imgMgr);
    restrictionManager.SelectRandom();
    wallEndLeft = false;
    wallEndRight = false;
    wallEndTimer = 180 + rand() % 300;
    hyperPlayerID = 0;
    itemManager.hyperPlayerID = 0;
    setsunaPhase = SETSUNA_SLIDE;
    setsunaPhaseTimer = 0;
    setsunaP1UIX = 1280.0f;
    setsunaP2UIX = -1280.0f;
    gravityInsaneLevel = 2;
    gravityInsaneTimer = 120 + rand() % 60;
    player1.gravityInsaneLevel = gravityInsaneLevel;
    player2.gravityInsaneLevel = gravityInsaneLevel;
    // スワップ状態をリセット
    if (restrictionManager.IsActive(REST_SCREEN_FLIP)) {
        if (flipPattern == 1 || flipPattern == 2) {
            player1.SwapImageWith(player2);
        }
    }
    flipPattern = rand() % 3;
    flipTimer = 420 + rand() % 180;
    // 新パターンがスワップ込みなら適用
    if (flipPattern == 1 || flipPattern == 2) {
        player1.SwapImageWith(player2);
    }
    weaponSpawnTimer = 0;
    mementoMoriTimer = 0;
    mementoMoriShooterID = 0;
    mementoMoriWinnerID = 0;
    mementoMoriPending = false;
    setsunaSignVisible = false;
    for (int i = 0; i < WEAPON_MAX; i++) {
        weapons[i].Init(WEAPON_KAMA, *imgMgr);
    }
    if (restrictionManager.IsActive(REST_THROW_NO_DAMAGE)) {
        player1.moveSpeed = 5.0f * 1.2f;
        player2.moveSpeed = 5.0f * 1.2f;
    }
    InitFallingUI();
    InitPlayers(keepWinCount);
}

void SceneGame::CheckParry(Player& attacker, int ownerID) {
    if (!attacker.attacking) return;
    bool isMeleeNoRest = restrictionManager.IsActive(REST_MELEE_NO_DAMAGE);
    bool isBareHand = attacker.holdingWeaponIndex == -1;
    if (isBareHand && !isMeleeNoRest) return;

    int chargeFrames = (attacker.holdingWeaponIndex == -1)
        ? BARE_HAND_CHARGE_FRAMES
        : WEAPON_DATA[weapons[attacker.holdingWeaponIndex].weaponType].chargeFrames;
    int attackFrames = (attacker.holdingWeaponIndex == -1)
        ? BARE_HAND_ATTACK_FRAMES
        : WEAPON_DATA[weapons[attacker.holdingWeaponIndex].weaponType].attackFrames;

    // 攻撃判定が出てる区間かどうか（カウントダウン方式）
    if (attacker.attackTimer >= chargeFrames) return;           // まだ構え中
    if (attacker.attackTimer < chargeFrames - attackFrames) return; // 判定終わり

    bool isParryFrame = (attacker.attackTimer == chargeFrames - 1); // 判定が出た最初の1フレーム

    float atkX = attacker.facingRight ? attacker.x + 50.0f : attacker.x - 50.0f;
    float atkY = attacker.y - 50.0f;

    if (!isBareHand) {
        Weapon& held = weapons[attacker.holdingWeaponIndex];
        if (held.parryRemain <= 0) return;
        orbManager.CheckParry(atkX, atkY, 40.0f, 80.0f, ownerID);
    }

    bool parried = false;
    bool anyThrownInRange = false;
    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
        bool inRange = weapons[i].CheckParry(atkX, atkY, 40.0f, 80.0f);
        if (inRange) anyThrownInRange = true;
        if (inRange) {
            if (isParryFrame) {
                // 弾き返し
                weapons[i].vx = -weapons[i].vx * (isMeleeNoRest ? 1.5f : 1.0f);
                return;
            }
            else {
                if (!isBareHand) {
                    // はたき落とし
                    Weapon& held = weapons[attacker.holdingWeaponIndex];
                    weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                    held.parryRemain--;
                    if (held.parryRemain <= 0) {
                        held.weaponState = Weapon::WEAPON_INACTIVE;
                        attacker.holdingWeaponIndex = -1;
                    }
                }
                else {
                    weapons[i].vx = -weapons[i].vx * 1.5f;
                }
            }
            parried = true;
            break;
        }
    }

    if (isMeleeNoRest && isBareHand && anyThrownInRange && !parried) {
        EnterHitState(ownerID == 1, true);
    }
}

void SceneGame::CheckWeaponHit(Player& target, Player& attacker, bool judgeValue, int targetID) {
    // ハイパー強い側はピコハン投げ当たりでスタン
    if (restrictionManager.IsActive(REST_HYPETSUYOI) && targetID == hyperPlayerID) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
            if (weapons[i].weaponType != WEAPON_PIKOHAN) continue;
            if (weapons[i].CheckHit(
                target.x, target.y - PLAYER_HIT_CY,
                PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
                weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                target.EnterStunShort();
                attacker.pikohanRespawnTimer = 180;
            }
        }
        return;
    }
    // メテオ中はピコハン投げ当たりでスタン
    if (restrictionManager.IsActive(REST_METEOR)) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
            if (weapons[i].weaponType != WEAPON_PIKOHAN) continue;
            if (weapons[i].CheckHit(
                target.x, target.y - PLAYER_HIT_CY,
                PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
                weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                target.EnterStunShort();
            }
        }
        return;
    }
    // 投げものダメージなしの場合
    if (restrictionManager.IsActive(REST_THROW_NO_DAMAGE)) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
            if (weapons[i].CheckHit(
                target.x, target.y - PLAYER_HIT_CY,
                PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
                weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                if (target.holdingWeaponIndex != -1) {
                    // 武器を持ってたら消す
                    weapons[target.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
                    target.holdingWeaponIndex = -1;
                }
                else {
                    // 武器を持ってなかったら移動速度ダウン 0.5秒間
                    target.moveSpeed = 5.0f * 0.6f;
                    target.speedDownTimer = 30;
                }
            }
        }
        return;
    }
    // 通常処理
    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].CheckHit(
            target.x, target.y - PLAYER_HIT_CY,
            PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
            weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
            if (target.hasShield) {
                target.hasShield = false;
                return;
            }
            EnterHitState(judgeValue, true);
            return;
        }
    }
}

void SceneGame::CheckHyperTouch() {
    if (!restrictionManager.IsActive(REST_HYPETSUYOI)) return;
    if (hyperPlayerID == 0) return;

    Player& hyperPlayer = (hyperPlayerID == 1) ? player1 : player2;
    Player& weakPlayer = (hyperPlayerID == 1) ? player2 : player1;
    float dx = hyperPlayer.x - weakPlayer.x;
    float dy = (hyperPlayer.y - PLAYER_HIT_CY) - (weakPlayer.y - PLAYER_HIT_CY);
    if (fabsf(dx) < PLAYER_HIT_W &&
        fabsf(dy) < PLAYER_HIT_H) {
        EnterHitState(hyperPlayerID == 2, true);
    }
}

void SceneGame::ThrowWeapon(Player& player, int ownerID) {
    if (!player.wantThrow) return;
    if (player.holdingWeaponIndex != -1) {
        int idx = player.holdingWeaponIndex;
        // ピコハン投げたらリスポーンタイマーセット
        if (weapons[idx].weaponType == WEAPON_PIKOHAN) {
            player.pikohanRespawnTimer = 180;
        }
        weapons[idx].Throw(player.x, player.y - 90.0f, player.facingRight, ownerID,
        (WeaponType)weapons[idx].weaponType, *imgMgr);
        player.holdingWeaponIndex = -1;

        // REST_GRAVITY_INSANEだけ縦方向に力を加える
        if (restrictionManager.IsActive(REST_GRAVITY_INSANE)) {
            // レベル別の重力値
            const float gravTable[5] = { 0.1f, 0.4f, 1.0f, 2.0f, 4.0f };
            float g = gravTable[gravityInsaneLevel];
            weapons[idx].throwGravity = g;

            if (gravityInsaneLevel == 0)      weapons[idx].vy = -12.0f; // 上にビュン（横短め）
            else if (gravityInsaneLevel == 1) weapons[idx].vy = -10.0f; // 上にビュン
            else if (gravityInsaneLevel == 2) weapons[idx].vy = 0.0f;   // 横まっすぐ
            else if (gravityInsaneLevel == 3) weapons[idx].vy = 3.0f;   // すぐ落ちる
            else if (gravityInsaneLevel == 4) weapons[idx].vy = 10.0f;  // ほぼ飛ばない
        }

    }
    player.wantThrow = false;
}

void SceneGame::CheckStickOrb(Player& player, int ownerID) {
    if (player.holdingWeaponIndex == -1) return;
    Weapon& held = weapons[player.holdingWeaponIndex];
    if (held.weaponType != WEAPON_STICK) return;
    if (!player.attacking) return;
    if (held.orbFired) return;

    int charge = WEAPON_DATA[WEAPON_STICK].chargeFrames;
    if (player.attackTimer != charge) return;

    orbManager.Launch(player.x, player.y - 50.0f, player.facingRight, ownerID);
    held.orbFired = true;
}

void SceneGame::CheckTensaiTsue(Player& player) {
    if (player.holdingWeaponIndex == -1) return;
    Weapon& held = weapons[player.holdingWeaponIndex];
    if (held.weaponType != WEAPON_TENSAI_TSUE) return;
    if (!player.attacking) return;
    if (held.tensaiFired) return;

    int charge = WEAPON_DATA[WEAPON_TENSAI_TSUE].chargeFrames;
    if (player.attackTimer != charge) return;

    meteorManager.SpawnTensai();
    held.tensaiFired = true;
}

void SceneGame::PickupWeapon(Player& player) {
    if (player.holdingWeaponIndex != -1) return;
    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].weaponState != Weapon::WEAPON_FALLING) continue;
        float dx = player.x - weapons[i].x;
        float dist = fabsf(dx);
        if (dist < 80.0f && fabsf(player.y - weapons[i].y) < 150.0f) {
            weapons[i].weaponState = Weapon::WEAPON_HELD;
            player.holdingWeaponIndex = i;
            break;
        }
    }
}

void SceneGame::SpawnWeapon()
{
    // メテオ中・ハイパー強い中は武器スポーンしない
    if (restrictionManager.IsActive(REST_METEOR)) return;
    if (restrictionManager.IsActive(REST_HYPETSUYOI)) return;
    if (restrictionManager.IsActive(REST_SETSUNA)) return;
    weaponSpawnTimer++;
    int spawnInterval = WEAPON_SPAWN_INTERVAL;
    if (restrictionManager.IsActive(REST_MELEE_NO_DAMAGE)) {
        spawnInterval = WEAPON_SPAWN_INTERVAL * 1.1; // 1.1倍に延長
    }
    if (weaponSpawnTimer >= spawnInterval) {
        weaponSpawnTimer = 0;
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
#ifdef _DEBUG
                WeaponType type = DBG_FORCE_WEAPON ? DBG_WEAPON_TYPE : (WeaponType)(rand() % WEAPON_TYPE_MAX);
                if (!DBG_FORCE_WEAPON) {
#else
                WeaponType type = (WeaponType)(rand() % WEAPON_TYPE_MAX);
                {
#endif

                    if (restrictionManager.IsActive(REST_STICK_ONLY)) {
                        type = (rand() % 10 == 0) ? WEAPON_TENSAI_TSUE : WEAPON_STICK;
                    }
                    else if (restrictionManager.IsActive(REST_MASH_MOVE)) {
                        type = WEAPON_BOOMERANG;
                    }
                    else if (restrictionManager.IsActive(REST_BOOMERANG_ONLY)) {
                        type = WEAPON_BOOMERANG;
                    }
                    else if (restrictionManager.IsActive(REST_THROW_NO_DAMAGE)) {
                        while (type == WEAPON_STICK) {
                            type = (WeaponType)(rand() % WEAPON_TYPE_MAX);
                        }
                    }
                    else {
                        // 通常時はピコハンを出さない
                        while (type == WEAPON_PIKOHAN) {
                            type = (WeaponType)(rand() % WEAPON_TYPE_MAX);
                        }
                    }

                }
                weapons[i].Init(type, *imgMgr);
                weapons[i].weaponState = Weapon::WEAPON_FALLING;
                weapons[i].x = (float)(rand() % 1100 + 90);
                weapons[i].y = 0.0f;
                weapons[i].angle = 0.0f;
                break;
                }
            }
        }
}

void SceneGame::CheckMementoMori(Player& attacker, Player& target, bool judgeValue) {
    if (attacker.holdingWeaponIndex == -1) return;
    Weapon& held = weapons[attacker.holdingWeaponIndex];
    if (held.weaponType != WEAPON_MEMENTO_MORI) return;
    if (!attacker.attacking) return;
    if (mementoMoriPending) return;

    int atkFrames = WEAPON_DATA[WEAPON_MEMENTO_MORI].attackFrames;
    if (attacker.attackTimer != atkFrames) return;

    float hitH = WEAPON_DATA[WEAPON_MEMENTO_MORI].hitH;
    bool hit = fabsf((target.y - PLAYER_HIT_CY) - (attacker.y - PLAYER_HIT_CY)) < (hitH + PLAYER_HIT_H) / 2;

    mementoMoriPending = true;
    mementoMoriShooterID = attacker.PlayerID;
    mementoMoriTimer = 30;
    mementoMoriWinnerID = hit ? (judgeValue ? 2 : 1) : (judgeValue ? 1 : 2);
}

void SceneGame::DrawMementoMori(Player& attacker) {
    int atkFrames = WEAPON_DATA[WEAPON_MEMENTO_MORI].attackFrames;
    bool isAttacking = attacker.attacking && attacker.attackTimer <= atkFrames
        && attacker.holdingWeaponIndex != -1
        && weapons[attacker.holdingWeaponIndex].weaponType == WEAPON_MEMENTO_MORI;
    bool isPending = mementoMoriPending && mementoMoriShooterID == attacker.PlayerID;

    if (!isAttacking && !isPending) return;

    float hitH = WEAPON_DATA[WEAPON_MEMENTO_MORI].hitH;
    float cy = attacker.y - PLAYER_HIT_CY;
    float drawY = cy - hitH / 2;

    for (int x = 0; x < 1280; x += 48) {
        DrawExtendGraphF(x, drawY, x + 48, drawY + 20, imgMgr->mementoMoriEffect, TRUE);
    }
}

void SceneGame::Update() {
    if (state == STATE_PLAYING) {
        if (timeTimer > 0) timeTimer--;
        else {
            if (restrictionManager.IsActive(REST_HYPETSUYOI) && hyperPlayerID != 0) {
                // ハイパー側が時間切れで自爆→弱い側の勝ち
                EnterHitState(hyperPlayerID == 1, true);
            }
            else {
                // 通常の引き分け処理
                isDraw = true;
                player1.animFrame = 6;
                player2.animFrame = 6;
                p1HpIndex = 1;
                p2HpIndex = 1;
                HIT_TIMER = 60;
                RESULT_TIMER = 120;
                timeTimer = matchTime * 60;
                state = STATE_HIT;
                JUDGE = false;
            }
        }

        if (restrictionManager.IsActive(REST_SCREEN_BLUR)) {
            if (blurTimer > 0) blurTimer--;
            else {
                blurMode = rand() % 3;
                blurTimer = 120 + rand() % 181;
            }
        }
        else {
            blurMode = 0;
            blurTimer = 0;
        }

        if (restrictionManager.IsActive(REST_METEOR)) {
            meteorManager.Update(player1, player2);
            if (meteorManager.hitOccurred) {
                meteorManager.hitOccurred = false;
                EnterHitState(meteorManager.hitWinnerID == 2, true);
            }
        }
        else if (meteorManager.HasActiveMeteor() || meteorManager.tensaiSpawnCount > 0) {
            meteorManager.Update(player1, player2, true);
            if (meteorManager.hitOccurred) {
                meteorManager.hitOccurred = false;
                EnterHitState(meteorManager.hitWinnerID == 2, true);
            }
        }

        if (restrictionManager.IsActive(REST_SETSUNA)) {
            UpdateSetsuna();
        }

        if (restrictionManager.IsActive(REST_GRAVITY_INSANE)) {
            uiShakeTimer++;

            bool enteredHeavy = false; // 今フレーム重いレベルに入ったか

            if (gravityInsaneTimer > 0) gravityInsaneTimer--;
            else {
                int newLevel;
                do {
                    newLevel = rand() % 5;
                } while (newLevel == gravityInsaneLevel);
                gravityInsaneLevel = newLevel;
                gravityInsaneTimer = 120 + rand() % 60;
                player1.gravityInsaneLevel = gravityInsaneLevel;
                player2.gravityInsaneLevel = gravityInsaneLevel;

                if (gravityInsaneLevel == 3 || gravityInsaneLevel == 4) {
                    enteredHeavy = true; // フラグ立てるだけ
                }
            }

            UpdateFallingUI(enteredHeavy); // UI落下処理を一括で呼ぶ
        }

        // 爆発中はプレイヤーの更新を止める
        if (!itemManager.isExploding && !mementoMoriPending) {
            player1.Update(stage, weapons, restrictionManager);
            player2.Update(stage, weapons, restrictionManager);
        }

        if (restrictionManager.IsActive(REST_MASH_MOVE)) {
            // タイマー更新
            if (wallEndTimer > 0) wallEndTimer--;
            else {
                // ランダムに左右どちらかを切り替え
                int roll = rand() % 3;
                wallEndLeft = (roll == 0 || roll == 2);
                wallEndRight = (roll == 1 || roll == 2);
                wallEndTimer = 180 + rand() % 240; // 3～7秒
            }

            // ビーム当たり判定
            auto checkWallEnd = [&](Player& player, int winnerID) {
                if (wallEndLeft && player.x < 80.0f)   EnterHitState(winnerID == 2, true);
                if (wallEndRight && player.x > 1250.0f) EnterHitState(winnerID == 2, true);
                };
            checkWallEnd(player1, 2);
            checkWallEnd(player2, 1);
        }

        if (!restrictionManager.IsActive(REST_SETSUNA)) {
            itemManager.Update(player1, player2, restrictionManager);
        }
        orbManager.Update(player1, player2);
        if (restrictionManager.IsActive(REST_SCREEN_BLUR)) {
            adManager.Update();
        }

        if (restrictionManager.IsActive(REST_SCREEN_FLIP)) {
            if (flipTimer > 0) flipTimer--;
            else {
                if (flipPattern == 1 || flipPattern == 2) {
                    player1.SwapImageWith(player2);
                }
                int newPattern;
                do {
                    newPattern = rand() % 3;
                } while (newPattern == flipPattern);
                flipPattern = newPattern;

                flipTimer = 420 + rand() % 180;
                if (flipPattern == 1 || flipPattern == 2) {
                    player1.SwapImageWith(player2);
                }
            }
        }

        // 爆発ヒット判定
        if (itemManager.hitOccurred) {
            itemManager.hitOccurred = false;
            EnterHitState(itemManager.hitWinnerID == 2, true);
        }

        if (orbManager.hitOccurred) {
            orbManager.hitOccurred = false;
            EnterHitState(orbManager.hitWinnerID == 2, true);
        }

        p1Glowing = player1.isGlowing;
        p2Glowing = player2.isGlowing;

        // ハイパー強いモード
        if (restrictionManager.IsActive(REST_HYPETSUYOI) && hyperPlayerID != 0) {
            // 接触判定
            CheckHyperTouch();
        }

        // メメントモリ判定
        CheckMementoMori(player1, player2, false);
        CheckMementoMori(player2, player1, true);

        // メメントモリ余韻タイマー
        if (mementoMoriPending) {
            mementoMoriTimer--;
            if (mementoMoriTimer <= 0) {
                mementoMoriPending = false;
                EnterHitState(mementoMoriWinnerID == 2, true);
            }
        }

        // ピコハンリスポーン
        auto checkPikohanRespawn = [&](Player& player) {
            if (player.pikohanRespawnTimer <= 0) return;
            player.pikohanRespawnTimer--;
            if (player.pikohanRespawnTimer == 0) {
                for (int i = 0; i < WEAPON_MAX; i++) {
                    if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                        weapons[i].Init(WEAPON_PIKOHAN, *imgMgr);
                        weapons[i].weaponState = Weapon::WEAPON_HELD;
                        player.holdingWeaponIndex = i;
                        break;
                    }
                }
            }
            };
        checkPikohanRespawn(player1);
        checkPikohanRespawn(player2);

        // オーブ判定
        CheckStickOrb(player1, 1);
        CheckStickOrb(player2, 2);

        // テンサイのメテオ判定
        CheckTensaiTsue(player1);
        CheckTensaiTsue(player2);

        // はたき落とし判定
        CheckParry(player1, 1);
        CheckParry(player2, 2);

        // 投げ武器ヒット判定
        CheckWeaponHit(player1, player2, true, 1);
        CheckWeaponHit(player2, player1, false, 2);

        // 近接攻撃ヒット判定
        if (restrictionManager.IsActive(REST_MELEE_NO_DAMAGE)) {
            // 武器持ちで当たったら速度ダウンのみ
            if (player1.CheckAttackHit(player2, weapons)) {
                if (player1.holdingWeaponIndex != -1) {
                    player2.moveSpeed = 5.0f * 0.6f;
                    player2.speedDownTimer = 180;
                }
            }
            if (player2.CheckAttackHit(player1, weapons)) {
                if (player2.holdingWeaponIndex != -1) {
                    player1.moveSpeed = 5.0f * 0.6f;
                    player1.speedDownTimer = 180;
                }
            }
        }
        else if (restrictionManager.IsActive(REST_METEOR)) {
            if (player1.CheckAttackHit(player2, weapons)) {
                if (player1.holdingWeaponIndex != -1 &&
                    weapons[player1.holdingWeaponIndex].weaponType == WEAPON_PIKOHAN) {
                    player2.EnterStun();
                    // ピコハン消してリスポーンタイマーセット
                    weapons[player1.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
                    player1.holdingWeaponIndex = -1;
                    player1.pikohanRespawnTimer = 180;
                }
            }
            if (player2.CheckAttackHit(player1, weapons)) {
                if (player2.holdingWeaponIndex != -1 &&
                    weapons[player2.holdingWeaponIndex].weaponType == WEAPON_PIKOHAN) {
                    player1.EnterStun();
                    // ピコハン消してリスポーンタイマーセット
                    weapons[player2.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
                    player2.holdingWeaponIndex = -1;
                    player2.pikohanRespawnTimer = 180;
                }
            }
        }
        else {
            if (player1.CheckAttackHit(player2, weapons)) {
                if (restrictionManager.IsActive(REST_SETSUNA)) {
                    // 刹那中は無視
                }
                else if (player1.holdingWeaponIndex != -1 &&
                    weapons[player1.holdingWeaponIndex].weaponType == WEAPON_MEMENTO_MORI) {
                    // メメントモリはCheckMementoMoriで処理
                }
                else if (restrictionManager.IsActive(REST_HYPETSUYOI) && hyperPlayerID == 2) {
                    if (player1.holdingWeaponIndex != -1 &&
                        weapons[player1.holdingWeaponIndex].weaponType == WEAPON_PIKOHAN) {
                        player2.EnterStun();
                        weapons[player1.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
                        player1.holdingWeaponIndex = -1;
                        player1.pikohanRespawnTimer = 180;
                    }
                }
                else {
                    EnterHitState(false, true);
                }
            }
            if (player2.CheckAttackHit(player1, weapons)) {
                if (restrictionManager.IsActive(REST_SETSUNA)) {
                    // 刹那中は無視
                }
                else if (player2.holdingWeaponIndex != -1 &&
                    weapons[player2.holdingWeaponIndex].weaponType == WEAPON_MEMENTO_MORI) {
                    // メメントモリはCheckMementoMoriで処理
                }
                else if (restrictionManager.IsActive(REST_HYPETSUYOI) && hyperPlayerID == 1) {
                    if (player2.holdingWeaponIndex != -1 &&
                        weapons[player2.holdingWeaponIndex].weaponType == WEAPON_PIKOHAN) {
                        player1.EnterStun();
                        weapons[player2.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
                        player2.holdingWeaponIndex = -1;
                        player2.pikohanRespawnTimer = 180;
                    }
                }
                else {
                    EnterHitState(true, true);
                }
            }
        }

        ThrowWeapon(player1, 1);
        ThrowWeapon(player2, 2);

        // 武器スポーン
        SpawnWeapon();

        PickupWeapon(player1);
        PickupWeapon(player2);

        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Update();
        }
    }
    else if (state == STATE_HIT) {
        // ヒット演出タイマー
        if (HIT_TIMER > 0) HIT_TIMER--;
        else state = STATE_RESULT;
    }
    else if (state == STATE_RESULT) {
        // リザルト表示タイマー
        if (RESULT_TIMER > 0) RESULT_TIMER--;
        else {
            if (player1.winCount >= WINNING_SCORE || player2.winCount >= WINNING_SCORE) {
                state = STATE_GAMEEND;
            }
            else {
                // ラウンド間リセット
                ResetGame(true);
            }
        }
    }
    else if (state == STATE_GAMEEND) {

        if (CheckHitKey(KEY_INPUT_R)) {
            nextScene = SCENE_MENU;
        }
    }
    else if (state == STATE_COUNTDOWN) {
        countdownTimer--;
        if (countdownTimer <= 0) {
            state = STATE_PLAYING;
        }
}
}

void SceneGame::UpdateFallingUI(bool enteredHeavy) {
    FallingUI* targets[5] = { &hpUI[0], &hpUI[1], &scoreUI[0], &scoreUI[1], &timeUI };

    // 重いレベルに入った瞬間だけカウント+1して落下抽選
    if (enteredHeavy) {
        for (int i = 0; i < 5; i++) {
            FallingUI& ui = *targets[i];
            if (ui.falling || ui.landed) continue;
            ui.count += (gravityInsaneLevel == 4) ? 2 : 1;
            int chance = 10 * ui.count;
            if (rand() % 100 < chance) {
                ui.falling = true;
                ui.vy = 0.0f;
            }
        }
    }

    // 落下中UIの移動（毎フレーム）
    for (int i = 0; i < 5; i++) {
        FallingUI& ui = *targets[i];
        if (!ui.falling) continue;
        ui.vy += GRAVITY;
        ui.y += ui.vy;
        if (ui.y >= GROUND_Y) {
            ui.y = GROUND_Y;
            ui.falling = false;
            ui.landed = true;
            ui.angle = ((rand() % 61) - 30) / 100.0f;
            ui.angle2 = ((rand() % 61) - 30) / 100.0f;
        }
    }

    // 落下中UIのプレイヤー当たり判定（落下中のみ、両プレイヤーに当たる）
    struct UIHitBox { FallingUI* ui; float w; float h; };
    UIHitBox boxes[5] = {
        { &hpUI[0],    UI_HP_W, UI_HP_H },
        { &hpUI[1],    UI_HP_W, UI_HP_H },
        { &scoreUI[0], WINNING_SCORE * UI_SCORE_W, UI_SCORE_H },
        { &scoreUI[1], WINNING_SCORE * UI_SCORE_W, UI_SCORE_H },
        { &timeUI,     236.0f, UI_NUMBER_SIZE },
    };
    auto hitPlayer = [&](Player& p) {
        for (int i = 0; i < 5; i++) {
            FallingUI& ui = *boxes[i].ui;
            if (!ui.falling) continue;
            float pw = PLAYER_HIT_W, ph = PLAYER_HIT_H;
            if (fabsf(ui.x - p.x) < (boxes[i].w + pw) / 2 &&
                fabsf(ui.y - (p.y - PLAYER_HIT_CY)) < (boxes[i].h + ph) / 2) {
                p.EnterStun();
            }
        }
        };
    hitPlayer(player1);
    hitPlayer(player2);
}

void SceneGame::UpdateSetsuna() {
    const float SLIDE_SPEED = 1280.0f / 60.0f; // 1秒でスライドイン

    if (setsunaPhase == SETSUNA_SLIDE) {
        player1.canAttack = false;
        player2.canAttack = false;
        setsunaP1UIX -= SLIDE_SPEED;
        setsunaP2UIX += SLIDE_SPEED;
        if (setsunaP1UIX <= 0.0f) {
            setsunaP1UIX = 0.0f;
            setsunaP2UIX = 0.0f;
            setsunaPhase = SETSUNA_READY;
            setsunaPhaseTimer = 60; // Ready表示1秒
        }
    }
    else if (setsunaPhase == SETSUNA_READY) {
        player1.canAttack = false;
        player2.canAttack = false;
        setsunaPhaseTimer--;
        if (setsunaPhaseTimer <= 0) {
            setsunaPhase = SETSUNA_WAIT;
            setsunaPhaseTimer = 180 + rand() % 420; // 3～10秒
        }
    }
    else if (setsunaPhase == SETSUNA_WAIT) {
        player1.canAttack = false;
        player2.canAttack = false;
        // フライングチェック
        bool p1Attack = CheckHitKey(KEY_INPUT_F);
        bool p2Attack = GetMouseInput() & MOUSE_INPUT_LEFT;
        if (p1Attack) { EnterHitState(true, true); }
        else if (p2Attack) { EnterHitState(false, true); }

        setsunaPhaseTimer--;
        if (setsunaPhaseTimer <= 0) {
            setsunaPhase = SETSUNA_ACTIVE;
            player1.canAttack = true;
            player2.canAttack = true;
            setsunaSignVisible = true;
        }
    }
}

void SceneGame::Draw() {
    ClearDrawScreen();
    if (state == STATE_PLAYING) {
        SetDrawScreen(currentTex);
        ClearDrawScreen();
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        DrawMementoMori(player1);
        DrawMementoMori(player2);
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        orbManager.Draw();
        if (restrictionManager.IsActive(REST_METEOR) || meteorManager.HasActiveMeteor()) {
            meteorManager.Draw(*imgMgr);
        }
        itemManager.Draw();
        DrawUI();
        adManager.Draw();
        if (restrictionManager.IsActive(REST_MASH_MOVE)) {
            unsigned int wallColor = GetColor(255, 0, 0);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
            if (wallEndLeft)  DrawBoxAA(0.0f, 0.0f, 50.0f, 920.0f, wallColor, TRUE);
            if (wallEndRight) DrawBoxAA(1230.0f, 0.0f, 1280.0f, 920.0f, wallColor, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        SetDrawScreen(DX_SCREEN_BACK);

        if (restrictionManager.IsActive(REST_SCREEN_FLIP)) {
            bool flipUD = (flipPattern == 0 || flipPattern == 1); // 上下反転
            bool flipLR = (flipPattern == 0 || flipPattern == 2); // 左右反転

            float left = flipLR ? 1280.0f : 0.0f;
            float right = flipLR ? 0.0f : 1280.0f;
            float top = flipUD ? 920.0f : 0.0f;
            float bottom = flipUD ? 0.0f : 920.0f;
            DrawExtendGraphF(left, top, right, bottom, currentTex, TRUE);
        }
        else {
            DrawGraph(0, 0, currentTex, TRUE);
        }

        if (blurMode != 0) {
            int alpha = (blurMode == 1) ? 180 : 250;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
            DrawGraph(0, 0, prevTex, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        // 重ねた後にコピー
        GetDrawScreenGraph(0, 0, 1280, 920, prevTex);

#ifdef _DEBUG
        for (int i = 0; i < restrictionManager.activeCount; i++) {
            DrawString(10, 10 + i * 20, RESTRICTION_NAMES[restrictionManager.active[i]], GetColor(255, 255, 0));
        }
#endif
    }
    else if (state == STATE_HIT) {
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        DrawMementoMori(player1);
        DrawMementoMori(player2);
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        orbManager.Draw();
        itemManager.Draw();
        DrawUI();
    }
    else if (state == STATE_RESULT) {
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        orbManager.Draw();
        itemManager.Draw();
        DrawUI();

        SetFontSize(72);
        const TCHAR* resultText = isDraw ? _T("ちんたらすんな！") :
            !JUDGE ? _T("赤の勝ち！") :
            _T("青の勝ち！");
        unsigned int resultColor = isDraw ? GetColor(255, 255, 0) :
            !JUDGE ? GetColor(255, 50, 50) :
            GetColor(50, 50, 255);
        int textW = GetDrawStringWidth(resultText, lstrlen(resultText));
        DrawString((1280 - textW) / 2, 380, resultText, resultColor);
        SetFontSize(16);

    }
    else if (state == STATE_GAMEEND) {

        SetFontSize(48);
        const TCHAR* winText = !JUDGE ? _T("赤の勝ち！") : _T("青の勝ち！");
        unsigned int winColor = !JUDGE ? GetColor(255, 50, 50) : GetColor(50, 50, 255);
        int winW = GetDrawStringWidth(winText, lstrlen(winText));
        DrawString((1280 - winW) / 2, 320, winText, winColor);

        SetFontSize(72);
        const TCHAR* endText = _T("ゲーム終了！");
        int endW = GetDrawStringWidth(endText, lstrlen(endText));
        DrawString((1280 - endW) / 2, 420, endText, GetColor(255, 255, 255));

        SetFontSize(24);
        const TCHAR* retText = _T("Rキーでメニューに戻る");
        int retW = GetDrawStringWidth(retText, lstrlen(retText));
        DrawString((1280 - retW) / 2, 560, retText, GetColor(0, 0, 0));

        SetFontSize(16);
    }
    else if (state == STATE_COUNTDOWN) {
        stage.Draw();
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        DrawUI();

        // 今回の制限（小さめ）
        SetFontSize(24);
        int titleW = GetDrawStringWidth(_T("今回の制限は！"), 7);
        DrawString((1280 - titleW) / 2, 330, _T("今回の制限は！"), GetColor(50, 50, 50));

        // 制限名を大きく表示
        SetFontSize(48);
        for (int i = 0; i < restrictionManager.activeCount; i++) {
            const TCHAR* text = RESTRICTION_NAMES[restrictionManager.active[i]];
            int textW = GetDrawStringWidth(text, lstrlen(text));
            DrawString((1280 - textW) / 2, 390 + i * 60, text, GetColor(255, 50, 50));
        }

        // カウントダウン数字
        SetFontSize(72);
        int sec = (countdownTimer / 60) + 1;
        TCHAR buf[8];
        wsprintf(buf, _T("%d"), sec);
        int numW = GetDrawStringWidth(buf, lstrlen(buf));
        DrawString((1280 - numW) / 2, 700, buf, GetColor(50, 50, 50));

        SetFontSize(16);
    }
}

void SceneGame::DrawUI()
{
    // 揺れ量を計算（重力レベル3,4のとき）
    bool uiShake = restrictionManager.IsActive(REST_GRAVITY_INSANE) &&
        (gravityInsaneLevel == 3 || gravityInsaneLevel == 4);
    float shakeX = uiShake ? sinf(uiShakeTimer * 0.8f) * 6.0f : 0.0f;

    // Player1 HPバー
    {
        int p1HpImg = p1Glowing ? imgMgr->p3Hp[p1HpIndex] : imgMgr->p1Hp[p1HpIndex];
        FallingUI& ui = hpUI[0];
        if (ui.falling || ui.landed) {
            DrawRotatedUI(ui.x, ui.y, UI_HP_W, UI_HP_H, ui.angle, p1HpImg);
        }
        else {
            // 通常：揺れだけ足して今まで通り
            DrawExtendGraphF(50.0f + shakeX, 20.0f, 50.0f + shakeX + UI_HP_W, 20.0f + UI_HP_H, p1HpImg, TRUE);
        }
    }

    // Player2 HPバー
    {
        int p2HpImg = p2Glowing ? imgMgr->p3Hp[p2HpIndex] : imgMgr->p2Hp[p2HpIndex];
        FallingUI& ui = hpUI[1];
        if (ui.falling || ui.landed) {
            DrawRotatedUI(ui.x, ui.y, UI_HP_W, UI_HP_H, ui.angle, p2HpImg);
        }
        else {
            DrawExtendGraphF(1280.0f - 50.0f - UI_HP_W + shakeX, 20.0f, 1280.0f - 50.0f + shakeX, 20.0f + UI_HP_H, p2HpImg, TRUE);
        }
    }


    // Player1スコア
    {
        FallingUI& ui = scoreUI[0];
        float groupW = WINNING_SCORE * UI_SCORE_W;
        if (ui.falling || ui.landed) {
            // 塊の中心(ui.x,ui.y)基準で3個並べる。角度は今回は無視して横並びのまま落とす簡易版
            for (int i = 0; i < WINNING_SCORE; i++) {
                int idx = (WINNING_SCORE - 1 - i < player1.winCount) ? 0 : 1;
                int scoreImg = p1Glowing ? imgMgr->p3Score[idx] : imgMgr->p1Score[idx];
                float cx = ui.x - groupW / 2 + i * UI_SCORE_W + UI_SCORE_W / 2;
                DrawRotatedUI(cx, ui.y, UI_SCORE_W, UI_SCORE_H, ui.angle, scoreImg);
            }
        }
        else {
            for (int i = 0; i < WINNING_SCORE; i++) {
                int idx = (WINNING_SCORE - 1 - i < player1.winCount) ? 0 : 1;
                int scoreImg = p1Glowing ? imgMgr->p3Score[idx] : imgMgr->p1Score[idx];
                DrawExtendGraphF(UI_P1_SCORE_X + i * UI_SCORE_W + shakeX, UI_SCORE_Y,
                    UI_P1_SCORE_X + i * UI_SCORE_W + UI_SCORE_W + shakeX, UI_SCORE_Y + UI_SCORE_H,
                    scoreImg, TRUE);
            }
        }
    }

    // Player2スコア
    {
        FallingUI& ui = scoreUI[1];
        float groupW = WINNING_SCORE * UI_SCORE_W;
        float p2StartX = UI_P2_SCORE_X - WINNING_SCORE * UI_SCORE_W;
        if (ui.falling || ui.landed) {
            for (int i = 0; i < WINNING_SCORE; i++) {
                int idx = (i < player2.winCount) ? 0 : 1;
                int scoreImg = p2Glowing ? imgMgr->p3Score[idx] : imgMgr->p2Score[idx];
                float cx = ui.x - groupW / 2 + i * UI_SCORE_W + UI_SCORE_W / 2;
                DrawRotatedUI(cx, ui.y, UI_SCORE_W, UI_SCORE_H, ui.angle, scoreImg);
            }
        }
        else {
            for (int i = 0; i < WINNING_SCORE; i++) {
                int idx = (i < player2.winCount) ? 0 : 1;
                int scoreImg = p2Glowing ? imgMgr->p3Score[idx] : imgMgr->p2Score[idx];
                float startX = p2StartX + i * UI_SCORE_W + shakeX;
                DrawExtendGraphF(startX, UI_SCORE_Y, startX + UI_SCORE_W, UI_SCORE_Y + UI_SCORE_H,
                    scoreImg, TRUE);
            }
        }
    }

    int sec = timeTimer / 60;
    int tens = sec / 10;
    int ones = sec % 10;
    {
        FallingUI& ui = timeUI;
        if (ui.falling || ui.landed) {
            // 塊の中心から左右に半桁ずつ振り分け
            float half = UI_NUMBER_SIZE / 2.0f;
            float tensX = ui.x - half;  // 十の位の中心
            float onesX = ui.x + half;  // 一の位の中心
            DrawRotatedUI(tensX, ui.y, UI_NUMBER_SIZE, UI_NUMBER_SIZE, ui.angle, imgMgr->numbers[tens]);
            DrawRotatedUI(onesX, ui.y, UI_NUMBER_SIZE, UI_NUMBER_SIZE, ui.angle2, imgMgr->numbers[ones]);
        }
        else {
            DrawExtendGraphF(UI_TIME_X + shakeX, UI_TIME_Y, UI_TIME_X + UI_NUMBER_SIZE + shakeX, UI_TIME_Y + UI_NUMBER_SIZE, imgMgr->numbers[tens], TRUE);
            DrawExtendGraphF(638.0f + shakeX, UI_TIME_Y, 638.0f + UI_NUMBER_SIZE + shakeX, UI_TIME_Y + UI_NUMBER_SIZE, imgMgr->numbers[ones], TRUE);
        }
    }

    if (setsunaSignVisible) {
        DrawExtendGraphF(563.0f, 347.0f, 717.0f, 501.0f, imgMgr->surpMark, TRUE);
    }

    if (restrictionManager.IsActive(REST_SETSUNA)) {
        // 上部UI（P1）
        DrawExtendGraphF(
            setsunaP1UIX, 0.0f,
            setsunaP1UIX + 1280.0f, 300.0f,
            imgMgr->setsunaP1[0], TRUE
        );
        // 下部UI（P2）
        DrawExtendGraphF(
            setsunaP2UIX, 620.0f,
            setsunaP2UIX + 1280.0f, 920.0f,
            imgMgr->setsunaP2[0], TRUE
        );
    }
}

void SceneGame::EnterHitState(bool judgeValue, bool addScore) {
    JUDGE = judgeValue;
    if (JUDGE) {
        p1HpIndex = 1;
        player1.animFrame = 6;
        if (addScore) player2.winCount++;
    }
    else {
        p2HpIndex = 1;
        player2.animFrame = 6;
        if (addScore) player1.winCount++;
    }
    HIT_TIMER = 60;
    RESULT_TIMER = 120;
    timeTimer = matchTime * 60;
    player1.freezeAnim = false;
    player2.freezeAnim = false;
    state = STATE_HIT;
}

SceneID SceneGame::GetNextScene() {
    return nextScene;
}