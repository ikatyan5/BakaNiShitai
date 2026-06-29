// DebugConfig.h
#pragma once

// デバッグ専用の強制フラグ（Debug時のみ有効）
// アイテム/武器/妨害をテスト用に固定で出す

#ifdef _DEBUG

#include "ItemManager.h"
#include "WeaponConfig.h"
#include "RestrictionConfig.h"

// アイテム強制スポーン
const bool DBG_FORCE_ITEM = false;
const ItemType DBG_ITEM_TYPE = ITEM_POTION_RED;

// 武器強制スポーン
const bool DBG_FORCE_WEAPON = false;
const WeaponType DBG_WEAPON_TYPE = WEAPON_STICK;

// 制限強制適用
const bool DBG_FORCE_RESTRICTION = false;
const RestrictionType DBG_RESTRICTION_TYPE = REST_BLACKOUT;
#endif