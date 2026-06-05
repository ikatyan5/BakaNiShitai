#include "SceneGame.h"
#include "Config.h"
#include "UIConfig.h"
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

	InitPlayers(false);

    for (int i = 0; i < WEAPON_MAX; i++) {
        weapons[i].Init(WEAPON_KAMA, *imgMgr);
    }
    itemManager.Init(*imgMgr);
    orbManager.Init(*imgMgr);
    stage.Init(1);
}

void SceneGame::InitPlayers(bool keepWinCount) {
    bool p1Right = rand() % 2 == 0;
    float leftX = 200.0f;
    float rightX = 1080.0f;

    int p1Win = keepWinCount ? player1.winCount : 0;
    int p2Win = keepWinCount ? player2.winCount : 0;

    player1.Init(p1Right ? rightX : leftX, 360.0f, 1, p1Right, *imgMgr, p1Win);
    player2.Init(p1Right ? leftX : rightX, 360.0f, 2, !p1Right, *imgMgr, p2Win);
}

void SceneGame::CheckParry(Player& attacker, int ownerID) {
    if (!attacker.attacking) return;
    if (attacker.attackTimer >= 7) return;
    bool isFirstFrame = (attacker.attackTimer == PARRY_FRAME);
    if (attacker.holdingWeaponIndex == -1) return;

    Weapon& held = weapons[attacker.holdingWeaponIndex];
    if (held.parryRemain <= 0) return;

    float atkX = attacker.facingRight ? attacker.x + 50.0f : attacker.x - 50.0f;
    float atkY = attacker.y - 50.0f;

    orbManager.CheckParry(atkX, atkY, 40.0f, 80.0f, ownerID);

    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].ownerID == ownerID) continue;
        if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
        if (weapons[i].CheckParry(atkX, atkY, 40.0f, 80.0f)) {
            if (isFirstFrame) {
                weapons[i].vx = -weapons[i].vx;
            }
            else {
                weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                held.parryRemain--;
                if (held.parryRemain <= 0) {
                    held.weaponState = Weapon::WEAPON_INACTIVE;
                    attacker.holdingWeaponIndex = -1;
                }
            }
            break;
        }
    }
}

void SceneGame::CheckWeaponHit(Player& target, Player& attacker, bool judgeValue, int targetID) {
    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
        if (weapons[i].CheckHit(
            target.x, target.y - PLAYER_HIT_CY,
            PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
            weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
            attacker.winCount++;
            EnterHitState(judgeValue);
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
    weaponSpawnTimer++;
    if (weaponSpawnTimer >= WEAPON_SPAWN_INTERVAL) {
        weaponSpawnTimer = 0;
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                WeaponType type = (WeaponType)(rand() % WEAPON_TYPE_MAX);
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

        // 爆発中はプレイヤーの更新を止める
        if (!itemManager.isExploding) {
            player1.Update(stage, weapons);
            player2.Update(stage, weapons);
        }
        itemManager.Update(player1, player2);
        orbManager.Update(player1, player2);
        // 爆発ヒット判定
        if (itemManager.hitOccurred) {
            itemManager.hitOccurred = false;
            EnterHitState(itemManager.hitWinnerID == 2, true);
        }

        if (orbManager.hitOccurred) {
            orbManager.hitOccurred = false;
            EnterHitState(orbManager.hitWinnerID == 2, false);
        }

        // はたき落とし判定
        CheckParry(player1, 1);
        CheckParry(player2, 2);

        // 投げ武器ヒット判定
        CheckWeaponHit(player1, player2, true, 1);
        CheckWeaponHit(player2, player1, false, 2);

        // 近接攻撃ヒット判定
        if (player1.CheckAttackHit(player2, weapons)) {
            EnterHitState(false);
        }
        if (player2.CheckAttackHit(player1, weapons)) {
            EnterHitState(true);
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
                InitPlayers(true);
                for (int i = 0; i < WEAPON_MAX; i++) {
                    weapons[i].Init(WEAPON_KAMA, *imgMgr);
                }
                isDraw = false;
                p1HpIndex = 0;
                p2HpIndex = 0;
                timeTimer = matchTime * 60;
                itemManager.Init(*imgMgr);
                orbManager.Init(*imgMgr);
                weaponSpawnTimer = 0;
                state = STATE_PLAYING;
            }
        }
    }
    else if (state == STATE_GAMEEND) {
        // Rキーでリスタート
        if (CheckHitKey(KEY_INPUT_R)) {
            InitPlayers(false);
            for (int i = 0; i < WEAPON_MAX; i++) {
                weapons[i].Init(WEAPON_KAMA, *imgMgr);
            }
            isDraw = false;
            p1HpIndex = 0;
            p2HpIndex = 0;
            timeTimer = matchTime * 60;
            itemManager.Init(*imgMgr);
            orbManager.Init(*imgMgr);
            weaponSpawnTimer = 0;
            state = STATE_PLAYING;
            nextScene = -1;
        }
    }
}

void SceneGame::Draw() {
    if (state == STATE_PLAYING) {
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        player1.Draw(weapons);
        player2.Draw(weapons);
        orbManager.Draw();
        itemManager.Draw();
        DrawUI();
    }
    else if (state == STATE_HIT) {
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        player1.Draw(weapons);
        player2.Draw(weapons);
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
        player1.Draw(weapons);
        player2.Draw(weapons);
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
    // Player1（左側）
    DrawExtendGraphF(50.0f, 20.0f, 50.0f + UI_HP_W, 20.0f + UI_HP_H, imgMgr->p1Hp[p1HpIndex], TRUE);
    // Player2（右側） 
    DrawExtendGraphF(1280.0f - 50.0f - UI_HP_W, 20.0f, 1280.0f - 50.0f, 20.0f + UI_HP_H, imgMgr->p2Hp[p2HpIndex], TRUE);

    // Player1スコア（HPバーの下あたり）
    for (int i = 0; i < WINNING_SCORE; i++) {
        int idx = (WINNING_SCORE - 1 - i < player1.winCount) ? 0 : 1;
        DrawExtendGraphF(UI_P1_SCORE_X + i * UI_SCORE_W, UI_SCORE_Y,
            UI_P1_SCORE_X + i * UI_SCORE_W + UI_SCORE_W, UI_SCORE_Y + UI_SCORE_H,
            imgMgr->p1Score[idx], TRUE);
    }
    // Player2スコア（右側）
    for (int i = 0; i < WINNING_SCORE; i++) {
        int idx = (i < player2.winCount) ? 0 : 1;
        float startX = UI_P2_SCORE_X - WINNING_SCORE * UI_SCORE_W + i * UI_SCORE_W;
        DrawExtendGraphF(startX, UI_SCORE_Y, startX + UI_SCORE_W, UI_SCORE_Y + UI_SCORE_H,
            imgMgr->p2Score[idx], TRUE);
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