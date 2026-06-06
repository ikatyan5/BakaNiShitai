// DebugConfig.h
#pragma once

#ifdef _DEBUG

#include "ItemManager.h"
#include "WeaponConfig.h"

// アイテム強制スポーン
const bool DBG_FORCE_ITEM = false;
const ItemType DBG_ITEM_TYPE = ITEM_POTION_YELLOW;

// 武器強制スポーン
const bool DBG_FORCE_WEAPON = true;
const WeaponType DBG_WEAPON_TYPE = WEAPON_BOOMERANG;

#endif