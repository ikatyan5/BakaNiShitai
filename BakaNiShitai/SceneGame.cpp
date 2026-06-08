#include "SceneGame.h"
#include "Config.h"
#include "UIConfig.h"
#include "DebugConfig.h"
#include <cmath>

void SceneGame::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    state = STATE_PLAYING;
    JUDGE = false;
    HIT_TIMER = 0;
    RESULT_TIMER = 0;
    weaponSpawnTimer = 0;
    nextScene = -1;

    matchTime = DEFAULT_TIME;
    timeTimer = matchTime * 60;

    mementoMoriTimer = 0;
    mementoMoriShooterID = 0;
    mementoMoriWinnerID = 0;
    mementoMoriPending = false;

    currentTex = MakeScreen(1280, 920, TRUE);
    prevTex = MakeScreen(1280, 920, TRUE);
    blurMode = 0;
    blurTimer = 0;

    p1Glowing = false;
    p2Glowing = false;

    itemManager.Init(*imgMgr);
    orbManager.Init(*imgMgr);
    meteorManager.Init();
    restrictionManager.Init();

#ifdef _DEBUG
    restrictionManager.SelectRandom(); // デバッグ時は最初から制限をかける
#endif
    stage.Init(1);
    for (int i = 0; i < WEAPON_MAX; i++) {
        weapons[i].Init(WEAPON_KAMA, *imgMgr);
    }
    InitPlayers(false);
}

void SceneGame::InitPlayers(bool keepWinCount) {
    bool p1Right = rand() % 2 == 0;
    float leftX = 200.0f;
    float rightX = 1080.0f;

    int p1Win = keepWinCount ? player1.winCount : 0;
    int p2Win = keepWinCount ? player2.winCount : 0;

    player1.Init(p1Right ? rightX : leftX, 360.0f, 1, p1Right, *imgMgr, p1Win);
    player2.Init(p1Right ? leftX : rightX, 360.0f, 2, !p1Right, *imgMgr, p2Win);

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

void SceneGame::ResetGame(bool keepWinCount) {
    isDraw = false;
    p1HpIndex = 0;
    p2HpIndex = 0;
    p1Glowing = false;
    p2Glowing = false;
    timeTimer = matchTime * 60;
    itemManager.Init(*imgMgr);
    orbManager.Init(*imgMgr);
    meteorManager.Init();
    restrictionManager.SelectRandom();
    weaponSpawnTimer = 0;
    mementoMoriTimer = 0;
    mementoMoriShooterID = 0;
    mementoMoriWinnerID = 0;
    mementoMoriPending = false;
    for (int i = 0; i < WEAPON_MAX; i++) {
        weapons[i].Init(WEAPON_KAMA, *imgMgr);
    }
    InitPlayers(keepWinCount);
    if (restrictionManager.IsActive(REST_THROW_NO_DAMAGE)) {
        player1.moveSpeed = 5.0f * 1.2f;
        player2.moveSpeed = 5.0f * 1.2f;
    }
}

void SceneGame::CheckParry(Player& attacker, int ownerID) {
    if (!attacker.attacking) return;

    bool isMeleeNoRest = restrictionManager.IsActive(REST_MELEE_NO_DAMAGE);
    bool isBareHand = attacker.holdingWeaponIndex == -1;

    // 通常は素手ではたき落とし不可
    if (isBareHand && !isMeleeNoRest) return;

    // 弾き猶予フレーム（制限中は延長）
    int parryLimit = isMeleeNoRest ? 10 : 7; // 制限中は猶予を増やす
    if (attacker.attackTimer >= parryLimit) return;

    int chargeFrames = (attacker.holdingWeaponIndex == -1)
        ? BARE_HAND_CHARGE_FRAMES
        : WEAPON_DATA[weapons[attacker.holdingWeaponIndex].weaponType].chargeFrames;

    bool isParryFrame = (attacker.attackTimer == chargeFrames - 1);

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
                weapons[i].vx = -weapons[i].vx * (isMeleeNoRest ? 1.5f : 1.0f);
            }
            else {
                if (!isBareHand) {
                    Weapon& held = weapons[attacker.holdingWeaponIndex];
                    weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                    held.parryRemain--;
                    if (held.parryRemain <= 0) {
                        held.weaponState = Weapon::WEAPON_INACTIVE;
                        attacker.holdingWeaponIndex = -1;
                    }
                }
                else {
                    // 素手ではたき落とし成功
                    weapons[i].vx = -weapons[i].vx * 1.5f;
                }
            }
            parried = true;
            break;
        }
    }

    // 素手ではたき落とし失敗したらEND
    if (isMeleeNoRest && isBareHand && anyThrownInRange && !parried) {
        EnterHitState(ownerID == 1, true);
    }
}

void SceneGame::CheckWeaponHit(Player& target, Player& attacker, bool judgeValue, int targetID) {
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
    // メテオ中はピコハン投げ当たりでスタン
    if (restrictionManager.IsActive(REST_METEOR)) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
            if (weapons[i].weaponType != WEAPON_PIKOHAN) continue;
            if (weapons[i].CheckHit(
                target.x, target.y - PLAYER_HIT_CY,
                PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
                weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                target.EnterStun();
            }
        }
        return;
    }
    // 通常処理
    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
        if (weapons[i].CheckHit(
            target.x, target.y - PLAYER_HIT_CY,
            PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
            weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
            EnterHitState(judgeValue, true);
            return;
        }
    }
}

void SceneGame::ThrowWeapon(Player& player, int ownerID) {
    if (!player.wantThrow) return;
    if (player.holdingWeaponIndex != -1) {
        int idx = player.holdingWeaponIndex;
        if (weapons[idx].weaponType == WEAPON_STICK) {
            orbManager.Launch(player.x, player.y - 50.0f, player.facingRight, ownerID);
            weapons[idx].weaponState = Weapon::WEAPON_INACTIVE;
            player.holdingWeaponIndex = -1;
        }
        else {
            // ピコハン投げたらリスポーンタイマーセット
            if (weapons[idx].weaponType == WEAPON_PIKOHAN) {
                player.pikohanRespawnTimer = 180;
            }
            weapons[idx].Throw(player.x, player.y - 50.0f, player.facingRight, ownerID,
                (WeaponType)weapons[idx].weaponType, *imgMgr);
            player.holdingWeaponIndex = -1;
        }
    }
    player.wantThrow = false;
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

void SceneGame::SpawnWeapon() {
    // メテオ中は武器スポーンしない
    if (restrictionManager.IsActive(REST_METEOR)) return;
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
                        type = WEAPON_STICK;
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
}

void SceneGame::CheckMementoMori(Player& attacker, Player& target, bool judgeValue) {
    if (attacker.holdingWeaponIndex == -1) return;
    Weapon& held = weapons[attacker.holdingWeaponIndex];
    if (held.weaponType != WEAPON_MEMENTO_MORI) return;
    if (!attacker.attacking) return;
    if (mementoMoriPending)return;

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

    // 48pxのタイルを横に並べて1280px埋める
    for (int x = 0; x < 1280; x += 48) {
        DrawExtendGraphF(x, drawY, x + 48, drawY + 20, imgMgr->mementoMoriEffect, TRUE);
    }
}

void SceneGame::Update() {
    if (state == STATE_PLAYING) {
        // タイマーカウントダウン
        if (timeTimer > 0) timeTimer--;
        else {
            // 時間切れ！両者END
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

        // 爆発中はプレイヤーの更新を止める
        if (!itemManager.isExploding && !mementoMoriPending) {
            player1.Update(stage, weapons, restrictionManager);
            player2.Update(stage, weapons, restrictionManager);
        }
        itemManager.Update(player1, player2, restrictionManager);
        orbManager.Update(player1, player2);
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
            if (player1.CheckAttackHit(player2, weapons)) EnterHitState(false, true);
            if (player2.CheckAttackHit(player1, weapons)) EnterHitState(true, true);
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
                state = STATE_PLAYING;
            }
        }
    }
    else if (state == STATE_GAMEEND) {
        // Rキーでリスタート
        if (CheckHitKey(KEY_INPUT_R)) {
            ResetGame(false);
            state = STATE_PLAYING;
            nextScene = -1;
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
        if (restrictionManager.IsActive(REST_METEOR)) {
            meteorManager.Draw(*imgMgr);
        }
        itemManager.Draw();
        DrawUI();
        SetDrawScreen(DX_SCREEN_BACK);

        if (restrictionManager.IsActive(REST_SCREEN_FLIP)) {
            DrawExtendGraphF(0, 920, 1280, 0, currentTex, TRUE);
        }
        else {
            DrawGraph(0, 0, currentTex, TRUE);
        }

        if (blurMode != 0) {
            int alpha = (blurMode == 1) ? 100 : 180;
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
            DrawGraph(0, 0, prevTex, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

        // 重ねた後にコピー
        GetDrawScreenGraph(0, 0, 1280, 920, prevTex);

#ifdef _DEBUG
        const TCHAR* restrictionNames[] = {
            _T("NONE"),
            _T("GRAVITY_ZERO"),
            _T("HOVER_JUMP"),
            _T("THROW_NO_DAMAGE"),
            _T("MELEE_NO_DAMAGE"),
            _T("STICK_ONLY"),
            _T("BOOMERANG_ONLY"),
            _T("SETSUNA"),
            _T("GRAVITY_CONTROL"),
            _T("SCREEN_FLIP"),
            _T("MASH_MOVE"),
            _T("METEOR"),
            _T("ONIIGOKKO"),
            _T("JUMP_LIMIT"),
            _T("SCREEN_BLUR"),
            _T("WINDOW_MOVE"),
        };
        for (int i = 0; i < restrictionManager.activeCount; i++) {
            DrawString(10, 10 + i * 20, restrictionNames[restrictionManager.active[i]], GetColor(255, 255, 0));
        }
        DrawFormatString(10, 200, GetColor(255, 0, 0), _T("blurMode: %d"), blurMode);
#endif
    }
    else if (state == STATE_HIT) {
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        orbManager.Draw();
        itemManager.Draw();
        if (!isDraw) {
            if (!JUDGE) {
                DrawString(540, 300, _T("HIT"), GetColor(255, 0, 0));
            }
            else {
                DrawString(540, 300, _T("HIT"), GetColor(0, 0, 255));
            }
        }
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
        if (isDraw) {
            DrawString(540, 300, _T("ちんたらすんな！"), GetColor(255, 255, 0));
        }
        else if (!JUDGE) {
            DrawString(540, 300, _T("Player 1 Wins!"), GetColor(255, 0, 0));
        }
        else {
            DrawString(540, 300, _T("Player 2 Wins!"), GetColor(0, 0, 255));
        }
        DrawUI();
    }
    else if (state == STATE_GAMEEND) {
        if (!JUDGE) {
            DrawString(540, 350, _T("Player 1 Wins!"), GetColor(255, 0, 0));
        }
        else {
            DrawString(540, 350, _T("Player 2 Wins!"), GetColor(0, 0, 255));
        }
        DrawString(540, 300, _T("Game Over"), GetColor(255, 255, 255));
        DrawString(540, 250, _T("Return to R"), GetColor(255, 255, 255));
    }
}

void SceneGame::DrawUI()
{
    // Player1 HPバー
    int p1HpImg = p1Glowing ? imgMgr->p3Hp[p1HpIndex] : imgMgr->p1Hp[p1HpIndex];
    DrawExtendGraphF(50.0f, 20.0f, 50.0f + UI_HP_W, 20.0f + UI_HP_H, p1HpImg, TRUE);

    // Player2 HPバー
    int p2HpImg = p2Glowing ? imgMgr->p3Hp[p2HpIndex] : imgMgr->p2Hp[p2HpIndex];
    DrawExtendGraphF(1280.0f - 50.0f - UI_HP_W, 20.0f, 1280.0f - 50.0f, 20.0f + UI_HP_H, p2HpImg, TRUE);

    // Player1スコア
    for (int i = 0; i < WINNING_SCORE; i++) {
        int idx = (WINNING_SCORE - 1 - i < player1.winCount) ? 0 : 1;
        int scoreImg = p1Glowing ? imgMgr->p3Score[idx] : imgMgr->p1Score[idx];
        DrawExtendGraphF(UI_P1_SCORE_X + i * UI_SCORE_W, UI_SCORE_Y,
            UI_P1_SCORE_X + i * UI_SCORE_W + UI_SCORE_W, UI_SCORE_Y + UI_SCORE_H,
            scoreImg, TRUE);
    }

    // Player2スコア
    for (int i = 0; i < WINNING_SCORE; i++) {
        int idx = (i < player2.winCount) ? 0 : 1;
        int scoreImg = p2Glowing ? imgMgr->p3Score[idx] : imgMgr->p2Score[idx];
        float startX = UI_P2_SCORE_X - WINNING_SCORE * UI_SCORE_W + i * UI_SCORE_W;
        DrawExtendGraphF(startX, UI_SCORE_Y, startX + UI_SCORE_W, UI_SCORE_Y + UI_SCORE_H,
            scoreImg, TRUE);
    }

    int sec = timeTimer / 60;
    int tens = sec / 10;
    int ones = sec % 10;
    DrawExtendGraphF(UI_TIME_X, UI_TIME_Y, UI_TIME_X + UI_NUMBER_SIZE, UI_TIME_Y + UI_NUMBER_SIZE, imgMgr->numbers[tens], TRUE);
    DrawExtendGraphF(638.0f, UI_TIME_Y, 638.0f + UI_NUMBER_SIZE, UI_TIME_Y + UI_NUMBER_SIZE, imgMgr->numbers[ones], TRUE);
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
    state = STATE_HIT;
}

int SceneGame::GetNextScene() {
    return nextScene;
}