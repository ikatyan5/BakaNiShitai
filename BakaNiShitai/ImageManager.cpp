#include "ImageManager.h"

void ImageManager::Load() {
    // プレイヤー画像
    int src1 = LoadGraph(_T("Image/Player/Player1.png"));
    int src2 = LoadGraph(_T("Image/Player/Player2.png"));
    int src3 = LoadGraph(_T("Image/Player/Player3.png"));
    for (int i = 0; i < 7; i++) {
        player1[i] = DerivationGraph(i * 48, 0, 48, 64, src1);
        player2[i] = DerivationGraph(i * 48, 0, 48, 64, src2);
        player3[i] = DerivationGraph(i * 48, 0, 48, 64, src3);
    }

    // UI画像
    int srcP1Hp = LoadGraph(_T("Image/UI/Player1HP.png"));
    int srcP2Hp = LoadGraph(_T("Image/UI/Player2HP.png"));
    int srcP3Hp = LoadGraph(_T("Image/UI/Player3HP.png"));
    p1Hp[0] = DerivationGraph(0, 0, 128, 64, srcP1Hp); // 満タン
    p1Hp[1] = DerivationGraph(128, 0, 128, 64, srcP1Hp); // ゼロ
    p2Hp[0] = DerivationGraph(0, 0, 128, 64, srcP2Hp); // 満タン
    p2Hp[1] = DerivationGraph(128, 0, 128, 64, srcP2Hp); // ゼロ
    p3Hp[0] = DerivationGraph(0, 0, 128, 64, srcP3Hp); // 満タン
    p3Hp[1] = DerivationGraph(128, 0, 128, 64, srcP3Hp); // ゼロ

    int srcP1Score = LoadGraph(_T("Image/UI/Player1Score.png"));
    int srcP2Score = LoadGraph(_T("Image/UI/Player2Score.png"));
    int srcP3Score = LoadGraph(_T("Image/UI/Player3Score.png"));
    p1Score[0] = DerivationGraph(0, 0, 32, 32, srcP1Score); // 色付き
    p1Score[1] = DerivationGraph(32, 0, 32, 32, srcP1Score); // 枠のみ
    p2Score[0] = DerivationGraph(0, 0, 32, 32, srcP2Score); // 色付き
    p2Score[1] = DerivationGraph(32, 0, 32, 32, srcP2Score); // 枠のみ
    p3Score[0] = DerivationGraph(0, 0, 32, 32, srcP3Score); // 色付き
    p3Score[1] = DerivationGraph(32, 0, 32, 32, srcP3Score); // 枠のみ

    int srcNum = LoadGraph(_T("Image/UI/number.png"));
    for (int i = 0; i < 10; i++) {
        numbers[i] = DerivationGraph(i * 32, 0, 32, 32, srcNum);
    }

    // 武器画像
    weaponImages[WEAPON_KAMA] = LoadGraph(_T("Image/Weapon/kama.png"));
    weaponImages[WEAPON_KONBOU] = LoadGraph(_T("Image/Weapon/konbou.png"));
	weaponImages[WEAPON_KEN] = LoadGraph(_T("Image/Weapon/ken.png"));
	weaponImages[WEAPON_GUN] = LoadGraph(_T("Image/Weapon/gun.png"));
    weaponImages[WEAPON_HOUKI] = LoadGraph(_T("Image/Weapon/houki.png"));
    weaponImages[WEAPON_HAMMER] = LoadGraph(_T("Image/Weapon/hammer.png"));
    weaponImages[WEAPON_STICK] = LoadGraph(_T("Image/Weapon/stick.png"));
    weaponImages[WEAPON_BOOMERANG] = LoadGraph(_T("Image/Weapon/boomerang.png"));

    int srcOrb = LoadGraph(_T("Image/Weapon/orb.png"));
    for (int i = 0; i < 3; i++) {
        orbs[i] = DerivationGraph(32 * i, 0, 32, 32, srcOrb);
    }

    potionBlue = LoadGraph(_T("Image/Item/potionBlue.png"));
	potionRed = LoadGraph(_T("Image/Item/potionRed.png"));
    potionPurple = LoadGraph(_T("Image/Item/potionPurple.png"));
    potionYellow = LoadGraph(_T("Image/Item/potionYellow.png"));
    hankachi = LoadGraph(_T("Image/Item/hankachi.png"));
	bomb = LoadGraph(_T("Image/Item/bomb.png"));
}