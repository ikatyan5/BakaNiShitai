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

void SceneGame::Update() {
    if (state == STATE_PLAYING) {
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
        if (!itemManager.isExploding) {
            player1.Update(stage, weapons);
            player2.Update(stage, weapons);
        }
        itemManager.Update(player1, player2);


        if (itemManager.hitOccurred) {
            itemManager.hitOccurred = false;
            EnterHitState(itemManager.hitWinnerID == 2);
        }
        auto checkWeaponHit = [&](Player& target, Player& attacker, bool judgeValue, int targetID) {
            for (int i = 0; i < WEAPON_MAX; i++) {
                if (weapons[i].CheckHit(
                    target.x, target.y - PLAYER_HIT_CY,
                    PLAYER_HIT_W, PLAYER_HIT_H)
                    && weapons[i].ownerID != targetID) {
                    weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                    attacker.winCount++;
                    EnterHitState(judgeValue);
                    return;
                }
            }
        };

        checkWeaponHit(player1, player2, true, 1);
        checkWeaponHit(player2, player1, false, 2);

        if (player1.CheckAttackHit(player2, weapons)) {
            EnterHitState(false);
        }
        if (player2.CheckAttackHit(player1, weapons)) {
            EnterHitState(true);
        }

        auto throwWeapon = [&](Player& player, int ownerID) {
            if (player.wantThrow) {
                if (player.holdingWeaponIndex != -1) {
                    int idx = player.holdingWeaponIndex;
                    weapons[idx].Throw(player.x, player.y - 50.0f, player.facingRight, ownerID,
                        (WeaponType)weapons[idx].weaponType, *imgMgr);
                    player.holdingWeaponIndex = -1;
                }
                player.wantThrow = false;
            }
            };

        throwWeapon(player1, 1);
        throwWeapon(player2, 2);

        weaponSpawnTimer++;
        if (weaponSpawnTimer >= WEAPON_SPAWN_INTERVAL) {
            weaponSpawnTimer = 0;
            for (int i = 0; i < WEAPON_MAX; i++) {
                if (weapons[i].weaponState == Weapon::WEAPON_INACTIVE) {
                    weapons[i].weaponState = Weapon::WEAPON_FALLING;
                    weapons[i].x = (float)(rand() % 1100 + 90);
                    weapons[i].y = 0.0f;
                    weapons[i].angle = 0.0f;
                    WeaponType type = (WeaponType)(rand() % WEAPON_TYPE_MAX);
                    weapons[i].weaponType = type;
                    weapons[i].weaponImage = imgMgr->weaponImages[type];
                    break;
                }
            }
        }

        auto pickupWeapon = [&](Player& player) {
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
            };

        pickupWeapon(player1);
        pickupWeapon(player2);

        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Update();
        }
    }
    else if (state == STATE_HIT) {
        if (HIT_TIMER > 0) HIT_TIMER--;
        else state = STATE_RESULT;
    }
    else if (state == STATE_RESULT) {
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
                weaponSpawnTimer = 0;
                state = STATE_PLAYING;
            }
        }
    }
    else if (state == STATE_GAMEEND) {
        if (CheckHitKey(KEY_INPUT_R)) {
            InitPlayers(false);
            for (int i = 0; i < WEAPON_MAX; i++) {
                weapons[i].Init(WEAPON_KAMA, *imgMgr);
            }
            isDraw = false;
            p1HpIndex = 0;
            p2HpIndex = 0;
            itemManager.Init(*imgMgr);
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

void SceneGame::EnterHitState(bool judgeValue) {
    JUDGE = judgeValue;
    if (JUDGE) {
        p1HpIndex = 1;
        player1.animFrame = 6;
    }
    else {
        p2HpIndex = 1;
        player2.animFrame = 6;
    }
    HIT_TIMER = 60;
    RESULT_TIMER = 120;
    state = STATE_HIT;
}

int SceneGame::GetNextScene() {
    return nextScene;
}