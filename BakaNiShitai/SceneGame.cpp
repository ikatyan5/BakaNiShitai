#include "SceneGame.h"
#include "Config.h"
#include "UIConfig.h"
#include "DebugConfig.h"
#include <cmath>

// 妨害の説明文は各 Restriction 派生クラスの Name() が持つ（Restriction.cpp 参照）。
// 表示は restrictionManager.ActiveName() 経由で取得する。

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

void SceneGame::Init(ImageManager& imgMgr_, GameSettings& settings, SoundManager& sndMgr_) {
    imgMgr = &imgMgr_;
    sound = &sndMgr_;
    // ゲームに入った瞬間にメニュー系BGMを止めて、3曲からランダムで1曲流す。
    // 試合中はラウンドを跨いでも同じ曲。メニューに戻った時に止まる。
    sound->PlayGameBgmRandom();
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

    hyperDashing = false;
    hyperDashCooldown = 0;
    hyperDashDistance = 0.0f;

    mementoMoriTimer = 0;
    mementoMoriShooterID = 0;
    mementoMoriWinnerID = 0;
    mementoMoriPending = false;

    currentTex = MakeScreen(SCREEN_W, SCREEN_H, TRUE);
    prevTex = MakeScreen(SCREEN_W, SCREEN_H, TRUE);
    blurMode = 0;
    blurTimer = 0;

    p1Glowing = false;
    p2Glowing = false;

    setsunaPhase = SETSUNA_SLIDE;
    setsunaPhaseTimer = 0;
    setsunaP1UIX = SCREEN_W;
    setsunaP2UIX = -SCREEN_W;

    setsunaSignVisible = false;
    setsunaRedoPending = false;
    flyExplodeActive = false;
    flyExplodeTimer = 0;
    this->settings = &settings;
    itemManager.Init(*imgMgr, *sound);
    orbManager.Init(*imgMgr);
    meteorManager.Init(*sound);
    adManager.Init(*imgMgr);
    restrictionManager.Init();

#ifdef _DEBUG
    restrictionManager.SelectRandom(); // デバッグ時は最初から制限をかける
    // 最初のラウンドが刹那の見切りならBGMを止めておく
    if (restrictionManager.IsActive(REST_SETSUNA)) {
        sound->StopGameBgm();
    }
#endif
    stage.Init(1);
    for (int i = 0; i < WEAPON_MAX; i++) {
        weapons[i].Init(WEAPON_KAMA, *imgMgr);
    }
    InitFallingUI();
    InitPlayers(false);
    if (restrictionManager.Active()) restrictionManager.Active()->OnRoundStart(*this); // 妨害のラウンド準備（デバッグ強制時のみ有効）
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

    // 近接無双のシールド付与は MeleeMusouRestriction::OnRoundStart へ移設した。

    // 弱い側にピコハンを持たせる
    if (restrictionManager.IsActive(REST_HYPETSUYOI)) {
        // 負けてる側優先、同点ならランダム
        if (player1.winCount < player2.winCount) hyperPlayerID = 1;
        else if (player2.winCount < player1.winCount) hyperPlayerID = 2;
        else hyperPlayerID = (rand() % 2) + 1;

        itemManager.hyperPlayerID = hyperPlayerID;

        Player& hyperPlayer = (hyperPlayerID == 1) ? player1 : player2;
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
    hpUI[1].baseX = SCREEN_W - 50.0f - UI_HP_W / 2.0f;
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

void SceneGame::ResetGame(bool keepWinCount, bool keepRestriction) {
    isDraw = false;
    blurMode = 0;  // ぼやけ以外のラウンドでは合成がかからないよう毎ラウンド0に戻す
    blurTimer = 0;
    flyExplodeActive = false;
    flyExplodeTimer = 0;
    p1HpIndex = 0;
    p2HpIndex = 0;
    state = STATE_COUNTDOWN;
    countdownTimer = 180; // 3秒
    p1Glowing = false;
    p2Glowing = false;
    timeTimer = matchTime * 60;
    hyperDashing = false;
    hyperDashCooldown = 0;
    hyperDashDistance = 0.0f;

    animTimer = 0;
    animFrame = 0;

    itemManager.Init(*imgMgr, *sound);
    orbManager.Init(*imgMgr);
    meteorManager.Init(*sound);
    adManager.Init(*imgMgr);
    // keepRestriction中は新しい制限を選ばない（刹那のやり直し用）
    if (!keepRestriction) restrictionManager.SelectRandom();
    // 刹那の見切りのラウンドはBGMを止める。別の制限になったら再開する。
    if (restrictionManager.IsActive(REST_SETSUNA)) {
        sound->StopGameBgm();
    } else {
        sound->ResumeGameBgm();
    }
    hyperPlayerID = 0;
    itemManager.hyperPlayerID = 0;
    setsunaPhase = SETSUNA_SLIDE;
    setsunaPhaseTimer = 0;
    setsunaP1UIX = SCREEN_W;
    setsunaP2UIX = -SCREEN_W;
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
    InitFallingUI();
    InitPlayers(keepWinCount);
    if (restrictionManager.Active()) restrictionManager.Active()->OnRoundStart(*this); // 妨害のラウンド準備
}


// 入れ替え＋分身の実装は SwapRestriction（Restriction.cpp）へ移設した。

void SceneGame::CheckParry(Player& attacker, int ownerID) {
    if (!attacker.attacking) return;
    bool isBareHand = attacker.holdingWeaponIndex == -1;
    if (isBareHand) return; // 弾けるのは武器持ちだけ

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

    for (int i = 0; i < WEAPON_MAX; i++) {
        if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
        bool inRange = weapons[i].CheckParry(atkX, atkY, 40.0f, 80.0f);
        if (inRange) {
            if (isParryFrame) {
                // 弾き返し
                PlaySoundMem(sound->parry, DX_PLAYTYPE_BACK);
                weapons[i].vx = -weapons[i].vx;
                return;
            }
            else {
                // はたき落とし（ここに来るのは武器持ちのみ）
                PlaySoundMem(sound->deflect, DX_PLAYTYPE_BACK);
                Weapon& held = weapons[attacker.holdingWeaponIndex];
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
    // 近接無双：投げはシールドで1回だけ無効化。シールドが無ければ小さく押すだけ（場外には届かない）。
    if (restrictionManager.IsActive(REST_MELEE_MUSOU)) {
        for (int i = 0; i < WEAPON_MAX; i++) {
            if (weapons[i].weaponState != Weapon::WEAPON_THROWN) continue;
            if (weapons[i].CheckHit(
                target.x, target.y - PLAYER_HIT_CY,
                PLAYER_HIT_W, PLAYER_HIT_H, targetID)) {
                weapons[i].weaponState = Weapon::WEAPON_INACTIVE;
                if (target.hasShield) {
                    target.hasShield = false; // 投げを1回だけ無効化してシールド消滅
                    return;
                }
                float dirVx = (weapons[i].vx >= 0.0f) ? 25.0f : -25.0f; // ▼投げの押し（弱め）
                target.ApplyKnockback(dirVx, -6.0f);
                return;
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
        PlaySoundMem(sound->weaponThrow, DX_PLAYTYPE_BACK);
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

        // REST_ACCEL（加速＋反射）：投げた武器に跳ね返り属性を付ける。
        // 上下ランダム初速で水平一直線をやめ、壁でランダムに散る。
        // 武器の投げ速度自体は加速させない（超高速反射は理不尽なので、加速はプレイヤーだけ）。
        if (restrictionManager.IsActive(REST_ACCEL)) {
            weapons[idx].bouncing = true;
            weapons[idx].bounceCount = 4;     // 反射回数の寿命
            weapons[idx].throwGravity = 0.0f; // 重力なしでまっすぐ→壁でランダムに散る
            // 武器の投げ速度はプレイヤーより控えめに加速させる。
            // プレイヤーの倍率(最大3.0)の増加分を1/4に圧縮＝武器は最大1.5倍まで。
            float weaponMult = 1.0f + (player.accelMult - 1.0f) * 0.25f;
            weapons[idx].vx *= weaponMult;
            float kick = fabsf(weapons[idx].vx) * 0.8f;
            weapons[idx].vy = (((rand() % 1000) / 1000.0f) - 0.5f) * 2.0f * kick; // ±kick
        }

    }
    player.wantThrow = false;
}

void SceneGame::CheckMeleeHit(Player& attacker, Player& target, bool judgeValue) {
    if (!attacker.CheckAttackHit(target, weapons)) return;

    if (restrictionManager.IsActive(REST_MELEE_MUSOU)) {
        // 近接無双：武器近接は場外まで吹っ飛ばす即死級、素手はいやがらせ程度。
        // シールドは投げ専用なので近接は素通り（必ず吹っ飛ぶ）。
        bool hasWeapon = attacker.holdingWeaponIndex != -1;
        float kbVx = hasWeapon ? 200.0f : 12.0f; // ▼数値で吹っ飛び調整（×6.7が大体の総距離）
        float kbVy = hasWeapon ? -10.0f : -4.0f;
        float dirVx = (attacker.x < target.x) ? kbVx : -kbVx;
        target.ApplyKnockback(dirVx, kbVy);
        return;
    }

    if (restrictionManager.IsActive(REST_METEOR)) {
        if (attacker.holdingWeaponIndex != -1 &&
            weapons[attacker.holdingWeaponIndex].weaponType == WEAPON_PIKOHAN) {
            target.EnterStun();
            weapons[attacker.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
            attacker.holdingWeaponIndex = -1;
            attacker.pikohanRespawnTimer = 180;
        }
        return;
    }

    if (restrictionManager.IsActive(REST_SETSUNA)) return;

    if (attacker.holdingWeaponIndex != -1 &&
        weapons[attacker.holdingWeaponIndex].weaponType == WEAPON_MEMENTO_MORI) return;

    if (restrictionManager.IsActive(REST_HYPETSUYOI) && hyperPlayerID == target.PlayerID) {
        if (attacker.holdingWeaponIndex != -1 &&
            weapons[attacker.holdingWeaponIndex].weaponType == WEAPON_PIKOHAN) {
            target.EnterStun();
            weapons[attacker.holdingWeaponIndex].weaponState = Weapon::WEAPON_INACTIVE;
            attacker.holdingWeaponIndex = -1;
            attacker.pikohanRespawnTimer = 180;
        }
        return;
    }

    EnterHitState(judgeValue, true);
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

    PlaySoundMem(sound->tensai, DX_PLAYTYPE_BACK); // 天才の杖を振った
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
            PlaySoundMem(sound->weaponPickup, DX_PLAYTYPE_BACK);
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
    if (restrictionManager.IsActive(REST_MELEE_MUSOU)) {
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
                    else if (restrictionManager.IsActive(REST_BOOMERANG_ONLY)) {
                        type = WEAPON_BOOMERANG;
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

    PlaySoundMem(sound->mementoMori, DX_PLAYTYPE_BACK); // メメントモリ発射
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

    for (int x = 0; x < SCREEN_W; x += 48) {
        DrawExtendGraphF(x, drawY, x + 48, drawY + 20, imgMgr->mementoMoriEffect, TRUE);
    }
}

// フライング（！前の早撃ち）をやらかした側に出す爆発演出
void SceneGame::DrawFlyExplosion() {
    if (!flyExplodeActive) return;
    DrawRotaGraphF(flyExplodeX, flyExplodeY, 9.0, 0.0, imgMgr->bomb, TRUE);
    DrawCircleAA(flyExplodeX, flyExplodeY, 150.0f, 64, GetColor(0, 255, 255), FALSE);
}

void SceneGame::Update() {
    // フライング爆発演出のタイマー（状態に関係なく進める）
    if (flyExplodeTimer > 0) {
        flyExplodeTimer--;
        if (flyExplodeTimer == 0) flyExplodeActive = false;
    }

    if (state == STATE_PLAYING) {
        // P1/P2を配列で扱い、左右対称な処理をループで一度だけ書く
        Player* players[2] = { &player1, &player2 };

        animTimer++;
        if (animTimer >= 10) {
            animTimer = 0;
            animFrame = (animFrame + 1) % 2;
        }
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

        UpdateMeteor();

        // 前処理が必要な妨害（刹那・重力ランダム）の毎フレーム駆動を委譲（プレイヤー更新の前）
        if (restrictionManager.Active()) restrictionManager.Active()->UpdateBeforePlayers(*this);

        // ハイパー突進は HyperTsuyoiRestriction::UpdateBeforePlayers（委譲）が呼ぶ。

        // 爆発中はプレイヤーの更新を止める
        if (!itemManager.isExploding && !mementoMoriPending) {
            for (Player* p : players) {
                p->Update(stage, weapons, restrictionManager);

                // ジャンプ・素手攻撃を出した瞬間に音を鳴らしてフラグを戻す
                if (p->justJumped)       { PlaySoundMem(sound->jump,   DX_PLAYTYPE_BACK); p->justJumped = false; }
                if (p->justBareAttacked) { PlaySoundMem(sound->attack, DX_PLAYTYPE_BACK); p->justBareAttacked = false; }
            }
        }

        // 各妨害の毎フレーム挙動を委譲する（Strategy）。移植済みの妨害だけがここで動く。
        if (restrictionManager.Active()) restrictionManager.Active()->UpdatePlaying(*this);

        // ノックバック画面外チェック（近接無双は場外で負け）
        if (restrictionManager.IsActive(REST_MELEE_MUSOU)) {
            for (int i = 0; i < 2; i++) {
                if (players[i]->isKnockedBack && (players[i]->x < 0.0f || players[i]->x > SCREEN_W))
                    EnterHitState(i == 0, true); // i==0=P1が場外
            }
        }

        if (!restrictionManager.IsActive(REST_SETSUNA)) {
            itemManager.Update(player1, player2, restrictionManager);
        }
        orbManager.Update(player1, player2);

        // ぼやけの駆動と広告更新は ScreenBlurRestriction::UpdateBeforePlayers（委譲）が呼ぶ。
        // 画面反転の毎フレーム駆動は ScreenFlipRestriction::UpdatePlaying（委譲）が呼ぶ。

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

        // ハイパーの接触判定は HyperTsuyoiRestriction::UpdatePlaying（委譲）が呼ぶ。

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
        CheckMeleeHit(player1, player2, false);
        CheckMeleeHit(player2, player1, true);

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
        else {
            state = STATE_RESULT;
            // リザルトの「〇の勝ち！」表示と同時に鳴らす（引き分けは鳴らさない）
            if (!isDraw) PlaySoundMem(sound->win, DX_PLAYTYPE_BACK);
        }
    }
    else if (state == STATE_RESULT) {
        // リザルト表示タイマー
        if (RESULT_TIMER > 0) RESULT_TIMER--;
        else {
            if (setsunaRedoPending) {
                // 刹那の両者遅すぎ→同じ刹那をもう一回（スコアそのまま・別の制限に進まない）
                setsunaRedoPending = false;
                ResetGame(true, true);
            }
            else if (player1.winCount >= WINNING_SCORE || player2.winCount >= WINNING_SCORE) {
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
        int beforeSec = (countdownTimer / 60) + 1;
        countdownTimer--;
        int afterSec = (countdownTimer / 60) + 1;
        // 3・2・1と数字が変わる瞬間に鳴らす（0は鳴らさない）
        if (afterSec != beforeSec && afterSec >= 1 && afterSec <= 3) {
            PlaySoundMem(sound->countdown, DX_PLAYTYPE_BACK);
        }
        if (countdownTimer <= 0) {
            state = STATE_PLAYING;
            if (restrictionManager.IsActive(REST_SETSUNA)) {
                // ！が出るまでの余興音。一度だけ流して、鳴り終わったら無音
                PlaySoundMem(sound->setsuna, DX_PLAYTYPE_BACK);
            }
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
    const float SLIDE_SPEED = SCREEN_W / 60.0f; // 1秒でスライドイン

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
        // フライングチェック（！が出る前に攻撃したら負け＋やらかし爆発）
        bool p1Attack = CheckHitKey(KEY_INPUT_F);
        bool p2Attack = GetMouseInput() & MOUSE_INPUT_LEFT;
        if (p1Attack) {
            flyExplodeActive = true; flyExplodeX = player1.x; flyExplodeY = player1.y - 60.0f; flyExplodeTimer = 45;
            PlaySoundMem(sound->explosion, DX_PLAYTYPE_BACK);
            EnterHitState(true, true);
        }
        else if (p2Attack) {
            flyExplodeActive = true; flyExplodeX = player2.x; flyExplodeY = player2.y - 60.0f; flyExplodeTimer = 45;
            PlaySoundMem(sound->explosion, DX_PLAYTYPE_BACK);
            EnterHitState(false, true);
        }

        setsunaPhaseTimer--;
        if (setsunaPhaseTimer <= 0) {
            setsunaPhase = SETSUNA_ACTIVE;
            player1.canAttack = true;
            player2.canAttack = true;
            setsunaSignVisible = true;
            setsunaPhaseTimer = 180; // ！が出てからの反応猶予3秒
            PlaySoundMem(sound->setsunaSign, DX_PLAYTYPE_BACK); // ！が出た合図
        }
    }
    else if (setsunaPhase == SETSUNA_ACTIVE) {
        // ！が出てから3秒、どちらも決め手を出さなければ「両者遅すぎ」で引き分けやり直し
        if (setsunaPhaseTimer > 0) setsunaPhaseTimer--;
        if (setsunaPhaseTimer <= 0 && !player1.attacking && !player2.attacking) {
            isDraw = true;
            setsunaRedoPending = true;
            player1.animFrame = 6;
            player2.animFrame = 6;
            p1HpIndex = 1;
            p2HpIndex = 1;
            HIT_TIMER = 60;
            RESULT_TIMER = 120;
            state = STATE_HIT;
            JUDGE = false;
        }
    }
}

void SceneGame::UpdateScreenBlur() {
    if (blurTimer > 0) blurTimer--;
    else {
        blurMode = rand() % 3;
        blurTimer = 120 + rand() % 181;
    }
    adManager.Update();
}

void SceneGame::UpdateHyperDash() {
    if (hyperPlayerID == 0) return;
    Player& hp = (hyperPlayerID == 1) ? player1 : player2;

    if (hyperDashCooldown > 0) hyperDashCooldown--;

    // 構え→攻撃の遷移瞬間に突進発動
    if (!hyperDashing && hyperDashCooldown == 0 && hp.attacking) {
        if (hp.attackTimer == BARE_HAND_CHARGE_FRAMES - 1) {
            hyperDashing = true;
            hyperDashDistance = 0.0f;
        }
    }

    // ダッシュ中の処理
    const float DASH_SPEED = 35.0f;
    if (hyperDashing) {
        hp.vx = hp.facingRight ? DASH_SPEED : -DASH_SPEED;
        hp.vy = 0.0f;          // 重力無視で高さキープ
        hp.isDashing = true;
        hyperDashDistance += DASH_SPEED;
        if (hyperDashDistance >= SCREEN_W) {
            hyperDashing = false;
            hp.isDashing = false;
            hyperDashCooldown = 420;
        }
    }
    else {
        hp.isDashing = false;
    }
}

void SceneGame::UpdateGravityInsane() {
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

void SceneGame::UpdateScreenFlip() {
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

void SceneGame::UpdateMeteor() {
    bool isRestMeteor = restrictionManager.IsActive(REST_METEOR);
    bool hasTensai = meteorManager.HasActiveMeteor() || meteorManager.tensaiSpawnCount > 0;
    if (!isRestMeteor && !hasTensai) return;

    meteorManager.Update(player1, player2, !isRestMeteor);
    if (meteorManager.hitOccurred) {
        meteorManager.hitOccurred = false;
        EnterHitState(meteorManager.hitWinnerID == 2, true);
    }
}

// 連打移動の壁判定は MashMoveRestriction（Restriction.cpp）へ移設した。

void SceneGame::Draw() {
    ClearDrawScreen();
    if (state == STATE_PLAYING) {
        SetDrawScreen(currentTex);
        ClearDrawScreen();
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboardGame[animFrame], TRUE);
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        DrawMementoMori(player1);
        DrawMementoMori(player2);
        if (restrictionManager.Active()) restrictionManager.Active()->Draw(*this); // 妨害ごとの演出（分身など）
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        orbManager.Draw();
        if (restrictionManager.IsActive(REST_METEOR) || meteorManager.HasActiveMeteor()) {
            meteorManager.Draw(*imgMgr);
        }
        itemManager.Draw();
        DrawUI();
        adManager.Draw();
        if (restrictionManager.Active()) restrictionManager.Active()->DrawForeground(*this); // 妨害の前景演出（連打の壁など）
        SetDrawScreen(DX_SCREEN_BACK);

        if (restrictionManager.IsActive(REST_SCREEN_FLIP)) {
            bool flipUD = (flipPattern == 0 || flipPattern == 1); // 上下反転
            bool flipLR = (flipPattern == 0 || flipPattern == 2); // 左右反転

            float left = flipLR ? SCREEN_W : 0.0f;
            float right = flipLR ? 0.0f : SCREEN_W;
            float top = flipUD ? SCREEN_H : 0.0f;
            float bottom = flipUD ? 0.0f : SCREEN_H;
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
        GetDrawScreenGraph(0, 0, SCREEN_W, SCREEN_H, prevTex);

#ifdef _DEBUG
        for (int i = 0; i < restrictionManager.activeCount; i++) {
            DrawString(10, 10 + i * 20, restrictionManager.ActiveName(), GetColor(255, 255, 0));
        }
#endif
    }
    else if (state == STATE_HIT) {
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboardGame[animFrame], TRUE);
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        DrawMementoMori(player1);
        DrawMementoMori(player2);
        if (restrictionManager.Active()) restrictionManager.Active()->Draw(*this); // 妨害ごとの演出（分身など）
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        if (restrictionManager.IsActive(REST_METEOR) || meteorManager.HasActiveMeteor()) {
            meteorManager.Draw(*imgMgr);
        }
        orbManager.Draw();
        itemManager.Draw();
        DrawFlyExplosion();
        DrawUI();
    }
    else if (state == STATE_RESULT) {
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboardGame[animFrame], TRUE);
        stage.Draw();
        for (int i = 0; i < WEAPON_MAX; i++) {
            weapons[i].Draw();
        }
        if (restrictionManager.Active()) restrictionManager.Active()->Draw(*this); // 妨害ごとの演出（分身など）
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        if (restrictionManager.IsActive(REST_METEOR) || meteorManager.HasActiveMeteor()) {
            meteorManager.Draw(*imgMgr);
        }
        orbManager.Draw();
        itemManager.Draw();
        DrawUI();

        SetFontSize(72);
        const TCHAR* resultText =
            (isDraw && setsunaRedoPending) ? _T("画面見てないのか？") :
            isDraw ? _T("ちんたらすんな！") :
            !JUDGE ? _T("赤の勝ち！") :
            _T("青の勝ち！");
        unsigned int resultColor = isDraw ? GetColor(255, 255, 0) :
            !JUDGE ? GetColor(255, 50, 50) :
            GetColor(50, 50, 255);
        int textW = GetDrawStringWidth(resultText, lstrlen(resultText));
        DrawString((SCREEN_W - textW) / 2, 380, resultText, resultColor);
        SetFontSize(16);

    }
    else if (state == STATE_GAMEEND) {
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboardGame[animFrame], TRUE);
        SetFontSize(48);
        const TCHAR* winText = !JUDGE ? _T("赤の勝ち！") : _T("青の勝ち！");
        unsigned int winColor = !JUDGE ? GetColor(255, 50, 50) : GetColor(50, 50, 255);
        int winW = GetDrawStringWidth(winText, lstrlen(winText));
        DrawString((SCREEN_W - winW) / 2, 320, winText, winColor);

        SetFontSize(72);
        const TCHAR* endText = _T("ゲーム終了！");
        int endW = GetDrawStringWidth(endText, lstrlen(endText));
        DrawString((SCREEN_W - endW) / 2, 420, endText, GetColor(255, 255, 255));

        SetFontSize(24);
        const TCHAR* retText = _T("Rキーでメニューに戻る");
        int retW = GetDrawStringWidth(retText, lstrlen(retText));
        DrawString((SCREEN_W - retW) / 2, 560, retText, GetColor(0, 0, 0));

        SetFontSize(16);
    }
    else if (state == STATE_COUNTDOWN) {
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboardGame[animFrame], TRUE);
        stage.Draw();
        if (restrictionManager.Active()) restrictionManager.Active()->Draw(*this); // 妨害ごとの演出（分身など）
        player1.Draw(weapons, *imgMgr);
        player2.Draw(weapons, *imgMgr);
        DrawUI();

        // 今回の制限（小さめ）
        SetFontSize(24);
        int titleW = GetDrawStringWidth(_T("今回の制限は！"), 7);
        DrawString((SCREEN_W - titleW) / 2, 330, _T("今回の制限は！"), GetColor(50, 50, 50));

        // 制限名を大きく表示
        SetFontSize(48);
        for (int i = 0; i < restrictionManager.activeCount; i++) {
            const TCHAR* text = restrictionManager.ActiveName();
            int textW = GetDrawStringWidth(text, lstrlen(text));
            DrawString((SCREEN_W - textW) / 2, 390 + i * 60, text, GetColor(255, 50, 50));
        }

        // カウントダウン数字
        SetFontSize(72);
        int sec = (countdownTimer / 60) + 1;
        TCHAR buf[8];
        wsprintf(buf, _T("%d"), sec);
        int numW = GetDrawStringWidth(buf, lstrlen(buf));
        DrawString((SCREEN_W - numW) / 2, 700, buf, GetColor(50, 50, 50));

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
            DrawExtendGraphF(SCREEN_W - 50.0f - UI_HP_W + shakeX, 20.0f, SCREEN_W - 50.0f + shakeX, 20.0f + UI_HP_H, p2HpImg, TRUE);
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
        // 決着後は負けた側だけ「やられ絵」に差し替える
        int p1Idx = 0;
        int p2Idx = 0;
        if ((state == STATE_HIT || state == STATE_RESULT) && !isDraw) {
            if (JUDGE) p1Idx = 1; // JUDGE==true は P1 が負け
            else       p2Idx = 1; // JUDGE==false は P2 が負け
        }
        // 上部UI（P1）
        DrawExtendGraphF(
            setsunaP1UIX, 0.0f,
            setsunaP1UIX + SCREEN_W, 300.0f,
            imgMgr->setsunaP1[p1Idx], TRUE
        );
        // 下部UI（P2）
        DrawExtendGraphF(
            setsunaP2UIX, 620.0f,
            setsunaP2UIX + SCREEN_W, SCREEN_H,
            imgMgr->setsunaP2[p2Idx], TRUE
        );
    }
}

void SceneGame::EnterHitState(bool judgeValue, bool addScore) {
    PlaySoundMem(sound->hit, DX_PLAYTYPE_BACK); // 決着の一撃
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