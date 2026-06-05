#pragma once

enum WeaponType {
    WEAPON_KAMA,
    WEAPON_KONBOU,
	WEAPON_KEN,
    WEAPON_GUN,
    WEAPON_HOUKI,
    WEAPON_HAMMER,
    WEAPON_STICK,
    WEAPON_TYPE_MAX // 必ず最後に置く
};

struct WeaponData {
    float hitW;
    float hitH;
    float scale;
    bool isRanged;
    int parryCount;    // はたき落とせる回数
    float throwSpeed;  // 投げたときの速度
    float hitDuration; // 攻撃判定を出す時間（ハンマー用）
    float parryW;      // はたき落とし判定の幅（ほうき用）
    float parryH;      // はたき落とし判定の高さ
};

const WeaponData WEAPON_DATA[] = {
    //  hitW    hitH    scale  isRanged  parry  throwSpeed  hitDur  parryW  parryH
      { 60.0f, 100.0f, 1.5f,  true,     2,     8.0f,       7,      60.0f,  100.0f }, // KAMA 遅め
      { 60.0f,  90.0f, 1.5f,  true,     3,     10.0f,      7,      60.0f,   90.0f }, // KONBOU 標準
      { 70.0f,  70.0f, 1.5f,  true,     2,     13.0f,      7,      70.0f,   70.0f }, // KEN 少し速い
      { 100.0f, 80.0f, 1.5f,  true,     1,     16.0f,      7,     100.0f,   80.0f }, // GUN 速い
      { 60.0f, 100.0f, 1.5f,  true,     4,     10.0f,      7,      80.0f,  120.0f }, // HOUKI はたき広め
      { 60.0f, 100.0f, 1.5f,  true,     1,     7.0f,       14,     60.0f,  100.0f }, // HAMMER 遅い硬直長
      { 60.0f, 100.0f, 1.5f,  true,     1,     10.0f,      7,      60.0f,  100.0f }, // STICK
      // これから武器が増えたらここに追加するだけ！
};