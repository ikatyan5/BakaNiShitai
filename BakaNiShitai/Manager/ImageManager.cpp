#include "ImageManager.h"

void ImageManager::Load() {
    // プレイヤー画像
    int src1 = LoadGraph("Image/Player/Player1.png");
    int src2 = LoadGraph("Image/Player/Player2.png");
    int src3 = LoadGraph("Image/Player/Player3.png");
    for (int i = 0; i < 7; i++) {
        player1[i] = DerivationGraph(i * 48, 0, 48, 64, src1);
        player2[i] = DerivationGraph(i * 48, 0, 48, 64, src2);
        player3[i] = DerivationGraph(i * 48, 0, 48, 64, src3);
    }

    // UI画像
    int srcP1Hp = LoadGraph("Image/UI/Player1HP.png");
    int srcP2Hp = LoadGraph("Image/UI/Player2HP.png");
    int srcP3Hp = LoadGraph("Image/UI/Player3HP.png");
    p1Hp[0] = DerivationGraph(0, 0, 128, 64, srcP1Hp); // 満タン
    p1Hp[1] = DerivationGraph(128, 0, 128, 64, srcP1Hp); // ゼロ
    p2Hp[0] = DerivationGraph(0, 0, 128, 64, srcP2Hp); // 満タン
    p2Hp[1] = DerivationGraph(128, 0, 128, 64, srcP2Hp); // ゼロ
    p3Hp[0] = DerivationGraph(0, 0, 128, 64, srcP3Hp); // 満タン
    p3Hp[1] = DerivationGraph(128, 0, 128, 64, srcP3Hp); // ゼロ

    int srcP1Score = LoadGraph("Image/UI/Player1Score.png");
    int srcP2Score = LoadGraph("Image/UI/Player2Score.png");
    int srcP3Score = LoadGraph("Image/UI/Player3Score.png");
    p1Score[0] = DerivationGraph(0, 0, 32, 32, srcP1Score); // 色付き
    p1Score[1] = DerivationGraph(32, 0, 32, 32, srcP1Score); // 枠のみ
    p2Score[0] = DerivationGraph(0, 0, 32, 32, srcP2Score); // 色付き
    p2Score[1] = DerivationGraph(32, 0, 32, 32, srcP2Score); // 枠のみ
    p3Score[0] = DerivationGraph(0, 0, 32, 32, srcP3Score); // 色付き
    p3Score[1] = DerivationGraph(32, 0, 32, 32, srcP3Score); // 枠のみ

    surpMark = LoadGraph("Image/UI/surprise_mark.png");
    titleLogo = LoadGraph("Image/UI/Title.png");
    zukan = LoadGraph("Image/UI/zukan.png");
    zukan_restriction = LoadGraph("Image/UI/zukan_restriction.png");

    int srcKeyboard = LoadGraph("Image/UI/keyboard.png");
    keyboard[0] = DerivationGraph(0, 0, 600, 200, srcKeyboard); // 非選択1
    keyboard[1] = DerivationGraph(600, 0, 600, 200, srcKeyboard); // 非選択2
    keyboard[2] = DerivationGraph(1200, 0, 600, 200, srcKeyboard); // 選択中1
    keyboard[3] = DerivationGraph(1800, 0, 600, 200, srcKeyboard); // 選択中2

    int srcController = LoadGraph("Image/UI/controller.png");
    controller[0] = DerivationGraph(0, 0, 600, 200, srcController);
    controller[1] = DerivationGraph(600, 0, 600, 200, srcController);
    controller[2] = DerivationGraph(1200, 0, 600, 200, srcController);
    controller[3] = DerivationGraph(1800, 0, 600, 200, srcController);

    int srcblackboard = LoadGraph("Image/UI/blackboard.png");
    blackboard[0] = DerivationGraph(0, 0, 640, 460, srcblackboard);
    blackboard[1] = DerivationGraph(640, 0, 640, 460, srcblackboard);
    int srcBbSetting = LoadGraph("Image/UI/blackboard_setting.png");
    blackboardSetting[0] = DerivationGraph(0, 0, 640, 460, srcBbSetting);
    blackboardSetting[1] = DerivationGraph(640, 0, 640, 460, srcBbSetting);
    int srcBbGame = LoadGraph("Image/UI/blackboard_game.png");
    blackboardGame[0] = DerivationGraph(0, 0, 640, 460, srcBbGame);
    blackboardGame[1] = DerivationGraph(640, 0, 640, 460, srcBbGame);

    int srcPressAnyKey = LoadGraph("Image/UI/nankaose.png");
    pressAnyKey[0] = DerivationGraph(0, 0, 600, 200, srcPressAnyKey);
    pressAnyKey[1] = DerivationGraph(600, 0, 600, 200, srcPressAnyKey);
    int srcTatakau = LoadGraph("Image/UI/tatakau.png");
    menuTatakau[0] = DerivationGraph(0, 0, 500, 200, srcTatakau); // 非選択1
    menuTatakau[1] = DerivationGraph(500, 0, 500, 200, srcTatakau); // 非選択2
    menuTatakau[2] = DerivationGraph(1000, 0, 500, 200, srcTatakau); // 選択中1
    menuTatakau[3] = DerivationGraph(1500, 0, 500, 200, srcTatakau); // 選択中2
    int srcTutorial = LoadGraph("Image/UI/tutorial.png");
    menuTutorial[0] = DerivationGraph(0, 0, 500, 200, srcTutorial); // 非選択1
    menuTutorial[1] = DerivationGraph(500, 0, 500, 200, srcTutorial); // 非選択2
    menuTutorial[2] = DerivationGraph(1000, 0, 500, 200, srcTutorial); // 選択中1
    menuTutorial[3] = DerivationGraph(1500, 0, 500, 200, srcTutorial); // 選択中2
    int srcSetting = LoadGraph("Image/UI/setting.png");
    menuSetting[0] = DerivationGraph(0, 0, 300, 200, srcSetting); // 非選択1
    menuSetting[1] = DerivationGraph(300, 0, 300, 200, srcSetting); // 非選択2
    menuSetting[2] = DerivationGraph(600, 0, 300, 200, srcSetting); // 選択中1
    menuSetting[3] = DerivationGraph(900, 0, 300, 200, srcSetting); // 選択中2
    int srcNum = LoadGraph("Image/UI/number.png");
    for (int i = 0; i < 10; i++) {
        numbers[i] = DerivationGraph(i * 32, 0, 32, 32, srcNum);
    }
    int srcAnimTatakau = LoadGraph("Image/UI/menuAnim_tatakau.png");
    menuAnimTatakau[0] = DerivationGraph(0, 0, 400, 400, srcAnimTatakau);
    menuAnimTatakau[1] = DerivationGraph(400, 0, 400, 400, srcAnimTatakau);

    int srcAnimTutorial = LoadGraph("Image/UI/menuAnim_tutorial.png");
    menuAnimTutorial[0] = DerivationGraph(0, 0, 400, 400, srcAnimTutorial);
    menuAnimTutorial[1] = DerivationGraph(400, 0, 400, 400, srcAnimTutorial);

    int srcAnimSetting = LoadGraph("Image/UI/menuAnim_setting.png");
    menuAnimSetting[0] = DerivationGraph(0, 0, 400, 400, srcAnimSetting);
    menuAnimSetting[1] = DerivationGraph(400, 0, 400, 400, srcAnimSetting);

    int srcSetsunaP1 = LoadGraph("Image/UI/setsunaPlayer1.png");
    int srcSetsunaP2 = LoadGraph("Image/UI/setsunaPlayer2.png");
    setsunaP1[0] = DerivationGraph(0, 0, 640, 200, srcSetsunaP1); // 通常
    setsunaP1[1] = DerivationGraph(640, 0, 640, 200, srcSetsunaP1);
    setsunaP2[0] = DerivationGraph(0, 0, 640, 200, srcSetsunaP2);
    setsunaP2[1] = DerivationGraph(640, 0, 640, 200, srcSetsunaP2);

    // 武器画像
    weaponImages[WEAPON_KAMA] = LoadGraph("Image/Weapon/kama.png");
    weaponImages[WEAPON_KONBOU] = LoadGraph("Image/Weapon/konbou.png");
	weaponImages[WEAPON_KEN] = LoadGraph("Image/Weapon/ken.png");
	weaponImages[WEAPON_GUN] = LoadGraph("Image/Weapon/gun.png");
    weaponImages[WEAPON_HOUKI] = LoadGraph("Image/Weapon/houki.png");
    weaponImages[WEAPON_HAMMER] = LoadGraph("Image/Weapon/hammer.png");
    weaponImages[WEAPON_STICK] = LoadGraph("Image/Weapon/stick.png");
    weaponImages[WEAPON_BOOMERANG] = LoadGraph("Image/Weapon/boomerang.png");
    weaponImages[WEAPON_MEMENTO_MORI] = LoadGraph("Image/Weapon/memento_mori.png");
    weaponImages[WEAPON_PIKOHAN] = LoadGraph("Image/Weapon/pikohan.png");
    weaponImages[WEAPON_TENSAI_TSUE] = weaponImages[WEAPON_STICK]; // 杖と同じ画像を使い回し

    int srcOrb = LoadGraph("Image/Weapon/orb.png");
    for (int i = 0; i < 3; i++) {
        orbs[i] = DerivationGraph(32 * i, 0, 32, 32, srcOrb);
    }
    meteo = LoadGraph("Image/Weapon/meteo.png");

    // エフェクト画像
    mementoMoriEffect = LoadGraph("Image/Effect/memento_mori_Effect.png");
    shield = LoadGraph("Image/Effect/shield.png");
    int srcStan = LoadGraph("Image/Effect/stan.png");
    for (int i = 0; i < 2; i++) {
        stan[i] = DerivationGraph(32 * i, 0, 32, 32, srcStan);
    }
    int srcGyaku = LoadGraph("Image/Effect/gyaku.png");
    for (int i = 0; i < 2; i++) {
        gyaku[i] = DerivationGraph(32 * i, 0, 32, 32, srcGyaku);
    }
    int srcTensai = LoadGraph("Image/Effect/tensai.png");
    for (int i = 0; i < 2; i++) {
        tensai[i] = DerivationGraph(32 * i, 0, 32, 32, srcTensai);
    }

    // アイテム画像
    potionBlue = LoadGraph("Image/Item/potionBlue.png");
	potionRed = LoadGraph("Image/Item/potionRed.png");
    potionPurple = LoadGraph("Image/Item/potionPurple.png");
    potionYellow = LoadGraph("Image/Item/potionYellow.png");
    hankachi = LoadGraph("Image/Item/hankachi.png");
	bomb = LoadGraph("Image/Item/bomb.png");
    banana = LoadGraph("Image/Item/banana.png");
    kinoko = LoadGraph("Image/Item/poisonkinoko.png");

    // 広告
    adImages[0] = LoadGraph("Image/UI/AD1.png");
    adImages[1] = LoadGraph("Image/UI/AD2.png");
    adImages[2] = LoadGraph("Image/UI/AD3.png");
}