// DebugConfig.h
#pragma once

#ifdef _DEBUG

#include "ItemManager.h"
#include "WeaponConfig.h"
#include "RestrictionConfig.h"

// アイテム強制スポーン
const bool DBG_FORCE_ITEM = false;
const ItemType DBG_ITEM_TYPE = ITEM_POTION_RED;

// 武器強制スポーン
const bool DBG_FORCE_WEAPON = false;
const WeaponType DBG_WEAPON_TYPE = WEAPON_HAMMER;

// 制限強制適用
const bool DBG_FORCE_RESTRICTION = true;
const RestrictionType DBG_RESTRICTION_TYPE = REST_MASH_MOVE;
#endif