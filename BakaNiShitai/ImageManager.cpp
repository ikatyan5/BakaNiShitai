#include "ImageManager.h"

void ImageManager::Load() {
    // プレイヤー画像
    int src1 = LoadGraph(_T("Image/Player/Player1.png"));
    int src2 = LoadGraph(_T("Image/Player/Player2.png"));
    for (int i = 0; i < 7; i++) {
        player1[i] = DerivationGraph(i * 48, 0, 48, 64, src1);
        player2[i] = DerivationGraph(i * 48, 0, 48, 64, src2);
    }

    // UI画像
    int srcP1Hp = LoadGraph(_T("Image/UI/Player1HP.png"));
    int srcP2Hp = LoadGraph(_T("Image/UI/Player2HP.png"));
    p1Hp[0] = DerivationGraph(0, 0, 128, 64, srcP1Hp); // 満タン
    p1Hp[1] = DerivationGraph(128, 0, 128, 64, srcP1Hp); // ゼロ
    p2Hp[0] = DerivationGraph(0, 0, 128, 64, srcP2Hp); // 満タン
    p2Hp[1] = DerivationGraph(128, 0, 128, 64, srcP2Hp); // ゼロ

    int srcP1Score = LoadGraph(_T("Image/UI/Player1Score.png"));
    int srcP2Score = LoadGraph(_T("Image/UI/Player2Score.png"));
    p1Score[0] = DerivationGraph(0, 0, 32, 32, srcP1Score); // 色付き
    p1Score[1] = DerivationGraph(32, 0, 32, 32, srcP1Score); // 枠のみ
    p2Score[0] = DerivationGraph(0, 0, 32, 32, srcP2Score); // 色付き
    p2Score[1] = DerivationGraph(32, 0, 32, 32, srcP2Score); // 枠のみ

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

    int srcOrb = LoadGraph(_T("Image/Weapon/orb.png"));
    for (int i = 0; i < 3; i++) {
        orbs[i] = DerivationGraph(32 * i, 0, 32, 32, srcOrb);
    }

    potionBlue = LoadGraph(_T("Image/Item/potionBlue.png"));
	potionRed = LoadGraph(_T("Image/Item/potionRed.png"));
    potionPurple = LoadGraph(_T("Image/Item/potionPurple.png"));
	bomb = LoadGraph(_T("Image/Item/bomb.png"));
}