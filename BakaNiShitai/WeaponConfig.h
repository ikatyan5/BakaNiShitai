#pragma once

enum WeaponType {
    WEAPON_KAMA,
    WEAPON_KONBOU,
    WEAPON_KEN,
    WEAPON_GUN,
    WEAPON_HOUKI,
    WEAPON_HAMMER,
    WEAPON_STICK,
    WEAPON_BOOMERANG,
    WEAPON_MEMENTO_MORI,
    WEAPON_PIKOHAN,
    WEAPON_TENSAI_TSUE,
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
    int chargeFrames;  // 構えのフレーム数
    int attackFrames;  // 攻撃判定が出てる時間
};

const WeaponData WEAPON_DATA[] = {
    //  hitW    hitH   scale  isRanged  parry  throwSpeed  hitDur  parryW  parryH  charge  atkFrames
      { 60.0f, 100.0f, 1.5f,  true,     2,     8.0f,       7,      60.0f,  100.0f, 10,     6  }, // KAMA
      { 60.0f,  90.0f, 1.5f,  true,     3,     10.0f,      7,      60.0f,   90.0f,  8,     6  }, // KONBOU
      { 70.0f,  70.0f, 1.5f,  true,     2,     13.0f,      7,      70.0f,   70.0f, 10,     6  }, // KEN
      { 100.0f, 80.0f, 1.5f,  true,     1,     16.0f,      7,     100.0f,   80.0f,  5,     4  }, // GUN
      { 60.0f, 100.0f, 1.5f,  true,     4,     10.0f,      7,      80.0f,  120.0f,  7,     6  }, // HOUKI
      { 60.0f, 100.0f, 1.5f,  true,     1,     7.0f,       14,     60.0f,  100.0f, 12,     9  }, // HAMMER
      { 60.0f, 100.0f, 1.5f,  true,     1,     10.0f,      7,      60.0f,  100.0f,  8,     6  }, // STICK
      { 60.0f,  60.0f, 1.5f,  true,     1,     16.0f,      7,      60.0f,   60.0f,  6,     5  }, // BOOMERANG
      { 20.0f,  20.0f, 1.5f,  true,     0,     10.0f,      7,      20.0f,   60.0f, 20,     4  }, // MEMENTO_MORI
      { 100.0f, 80.0f, 1.5f,  true,     0,     10.0f,      7,      20.0f,   60.0f,  7,     4  }, // PIKOHAN
      { 60.0f, 100.0f, 1.5f,  true,     1,     10.0f,      7,      60.0f,  100.0f,  8,     6  }, // TENSAI_TSUE
      // これから武器が増えたらここに追加する！
};