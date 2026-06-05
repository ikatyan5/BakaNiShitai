#pragma once

enum WeaponType {
    WEAPON_KAMA,
    WEAPON_KONBOU,
	WEAPON_KEN,
    WEAPON_GUN,
    WEAPON_HOUKI,
    WEAPON_HAMMER,
    WEAPON_STICK,
    WEAPON_TYPE_MAX // •K‚¸ÅŒã‚É’u‚­
};

struct WeaponData {
    float hitW;     // “–‚½‚è”»’è‚Ì•
    float hitH;     // “–‚½‚è”»’è‚Ì‚‚³
    float scale;    // Šg‘å—¦
	bool isRanged;  // ‹ßÚ•Ší‚©‰“‹——£•Ší‚© true = U‚è‰º‚ë‚µ false = “Š‚°‚é
};

const WeaponData WEAPON_DATA[] = {
    { 60.0f, 100.0f, 1.5f, false },     // WEAPON_KAMA
    { 60.0f, 90.0f, 1.5f, false },      // WEAPON_KONBOU
    { 70.0f,  70.0f, 1.5f, true  },     // WEAPON_KEN
    { 100.0f,  80.0f, 1.5f, true  },    // WEAPON_GUN
    { 60.0f, 100.0f, 1.5f, true},       // WEAPON_HOUKI
    { 60.0f, 100.0f, 1.5f, true},       // WEAPON_HAMMER
    { 60.0f, 100.0f, 1.5f, true},       // WEAPON_STICK
    // ‚±‚ê‚©‚ç•Ší‚ª‘‚¦‚½‚ç‚±‚±‚É’Ç‰Á‚·‚é‚¾‚¯I
};