#include "ItemManager.h"
#include "ItemPotionBlue.h"
#include "ItemPotionRed.h"
#include "ItemPotionPurple.h"
#include "ItemPotionYellow.h"
#include "ItemHankachi.h"
#include "ItemBanana.h"
#include "ItemKinoko.h"
#include "Player.h"
#include "Config.h"
#include "DebugConfig.h"
#include <cmath>

void ItemManager::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    itemSpawnTimer = 0;
    hitOccurred = false;
    hitWinnerID = 0;
    hyperPlayerID = 0;
    for (int i = 0; i < ITEM_MAX; i++) {
        items[i] = nullptr;
    }
	bombImage = imgMgr->bomb;
    isExploding = false;
}

void ItemManager::Update(Player& player1, Player& player2, const RestrictionManager& restrictions) {
    // 爆発チェック
    CheckExplode(player1);
    CheckExplode(player2);

    itemSpawnTimer++;
    if (itemSpawnTimer >= ITEM_SPAWN_INTERVAL) {
        itemSpawnTimer = 0;
        SpawnItem(restrictions);
    }

    isExploding = false;
    for (int i = 0; i < ITEM_MAX; i++) {
        if (items[i] == nullptr) continue;
        items[i]->Update();

        // 爆発ヒット判定
        ItemPotionRed* red = dynamic_cast<ItemPotionRed*>(items[i]);
        if (red && red->exploding) {
            isExploding = true;

        }

        if (red) {
            bool hitPlayer1 = red->CheckExplodeHit(player1);
            bool hitPlayer2 = red->CheckExplodeHit(player2);

            if (!hitOccurred) {
                if (red->ownerID == 1) {
                    if (hitPlayer2) {
                        hitOccurred = true;
                        hitWinnerID = 1;
                    }
                    else if (red->explodeTimer <= 0) {
                        hitOccurred = true;
                        hitWinnerID = 2; // 自爆
                    }
                }
                else if (red->ownerID == 2) {
                    if (hitPlayer1) {
                        hitOccurred = true;
                        hitWinnerID = 2;
                    }
                    else if (red->explodeTimer <= 0) {
                        hitOccurred = true;
                        hitWinnerID = 1; // 自爆
                    }
                }
            }
        }

        // 拾う処理
        TryPickup(player1, i);
        TryPickup(player2, i);

        if (items[i] == nullptr) continue;
        if (items[i]->itemState == Item::ITEM_INACTIVE) {
            delete items[i];
            items[i] = nullptr;
        }
    }
}

void ItemManager::Draw() {
    for (int i = 0; i < ITEM_MAX; i++) {
        if (items[i] == nullptr) continue;
        items[i]->Draw();
    }
}

void ItemManager::SpawnItem(const RestrictionManager& restrictions) {
    for (int i = 0; i < ITEM_MAX; i++) {
        if (items[i] == nullptr) {
#ifdef _DEBUG
            ItemType type;
            if (DBG_FORCE_ITEM) {
                type = DBG_ITEM_TYPE;
            }
            else {
                do {
                    type = (ItemType)(rand() % ITEM_TYPE_MAX);
                } while (type == ITEM_BANANA || type == ITEM_KINOKO);
            }
            // デバッグ時も制限チェックを通す
            if (!DBG_FORCE_ITEM && restrictions.IsActive(REST_MASH_MOVE)) {
                type = (rand() % 2 == 0) ? ITEM_BANANA : ITEM_KINOKO;
            }
            else if (!DBG_FORCE_ITEM && restrictions.IsActive(REST_THROW_NO_DAMAGE)) {
                int roll = rand() % 10;
                if (roll < 4)      type = ITEM_POTION_PURPLE;
                else if (roll < 6) type = ITEM_POTION_BLUE;
                else if (roll < 8) type = ITEM_POTION_YELLOW;
                else               type = ITEM_HANKACHI;
            }
            else if (!DBG_FORCE_ITEM && restrictions.IsActive(REST_METEOR)) {
                int roll = rand() % 4;
                if (roll == 0)      type = ITEM_POTION_BLUE;
                else if (roll == 1) type = ITEM_POTION_PURPLE;
                else if (roll == 2) type = ITEM_POTION_YELLOW;
                else                type = ITEM_HANKACHI;
            }
            else if (!DBG_FORCE_ITEM && restrictions.IsActive(REST_HYPETSUYOI)) {
                // 赤ポーションなし・紫多め
                int roll = rand() % 10;
                if (roll < 5)      type = ITEM_POTION_PURPLE; // 50%
                else if (roll < 7) type = ITEM_POTION_BLUE;   // 20%
                else if (roll < 9) type = ITEM_POTION_YELLOW; // 20%
                else               type = ITEM_HANKACHI;       // 10%
            }
            else if (!DBG_FORCE_ITEM && restrictions.IsActive(REST_MELEE_NO_DAMAGE)) {
                int roll = rand() % 4;
                if (roll == 0)      type = ITEM_POTION_BLUE;
                else if (roll == 1) type = ITEM_POTION_PURPLE;
                else if (roll == 2) type = ITEM_POTION_YELLOW;
                else                type = ITEM_HANKACHI;
            }
#else
            ItemType type;
            if (restrictions.IsActive(REST_MASH_MOVE)) {
                type = (rand() % 2 == 0) ? ITEM_BANANA : ITEM_KINOKO;
            }
            else if (restrictions.IsActive(REST_THROW_NO_DAMAGE)) {
                int roll = rand() % 10;
                if (roll < 4)      type = ITEM_POTION_PURPLE;
                else if (roll < 6) type = ITEM_POTION_BLUE;
                else if (roll < 8) type = ITEM_POTION_YELLOW;
                else               type = ITEM_HANKACHI;
            }
            else if (restrictions.IsActive(REST_METEOR)) {
                // 赤ポーション除外
                int roll = rand() % 4;
                if (roll == 0)      type = ITEM_POTION_BLUE;
                else if (roll == 1) type = ITEM_POTION_PURPLE;
                else if (roll == 2) type = ITEM_POTION_YELLOW;
                else                type = ITEM_HANKACHI;
            }
            else if (restrictions.IsActive(REST_HYPETSUYOI)) {
                // 赤ポーションなし・紫多め
                int roll = rand() % 10;
                if (roll < 5)      type = ITEM_POTION_PURPLE; // 50%
                else if (roll < 7) type = ITEM_POTION_BLUE;   // 20%
                else if (roll < 9) type = ITEM_POTION_YELLOW; // 20%
                else               type = ITEM_HANKACHI;       // 10%
            }
            else if (restrictions.IsActive(REST_MELEE_NO_DAMAGE)) {
                int roll = rand() % 4;
                if (roll == 0)      type = ITEM_POTION_BLUE;
                else if (roll == 1) type = ITEM_POTION_PURPLE;
                else if (roll == 2) type = ITEM_POTION_YELLOW;
                else                type = ITEM_HANKACHI;
            }
            else {
                do {
                    type = (ItemType)(rand() % ITEM_TYPE_MAX);
                } while (type == ITEM_BANANA || type == ITEM_KINOKO);
            }
#endif

            if (type == ITEM_POTION_BLUE) {
                ItemPotionBlue* item = new ItemPotionBlue();
                item->itemImage = imgMgr->potionBlue;
                items[i] = item;
            }
            else if (type == ITEM_POTION_RED) {
                ItemPotionRed* item = new ItemPotionRed();
                item->itemImage = imgMgr->potionRed;
                item->exploding = false;
                item->explodeTimer = 0;
                item->bombImage = bombImage;
                items[i] = item;
            }
            else if (type == ITEM_POTION_PURPLE) {
                ItemPotionPurple* item = new ItemPotionPurple();
                item->itemImage = imgMgr->potionPurple;
                items[i] = item;
            }
            else if (type == ITEM_POTION_YELLOW) {
                ItemPotionYellow* item = new ItemPotionYellow();
                item->itemImage = imgMgr->potionYellow;
                items[i] = item;
            }
            else if (type == ITEM_HANKACHI) {
                ItemHankachi* item = new ItemHankachi();
                item->itemImage = imgMgr->hankachi;
                items[i] = item;
            }
            else if (type == ITEM_BANANA) {
                ItemBanana* item = new ItemBanana();
                item->itemImage = imgMgr->banana;
                items[i] = item;
            }
            else if (type == ITEM_KINOKO) {
                ItemKinoko* item = new ItemKinoko();
                item->itemImage = imgMgr->kinoko;
                items[i] = item;
            }
            if (items[i] == nullptr) break;

            items[i]->x = (float)(rand() % 1100 + 90);
            items[i]->y = 0.0f;
            items[i]->itemState = Item::ITEM_FALLING;
            break;
        }
    }
}

void ItemManager::CheckExplode(Player& player) {
    if (!player.wantExplode) return;
    player.wantExplode = false;
    for (int i = 0; i < ITEM_MAX; i++) {
        if (items[i] == nullptr) {
            ItemPotionRed* explosion = new ItemPotionRed();
            explosion->x = player.x;
            explosion->y = player.y - 60.0f;
            explosion->exploding = true;
            explosion->explodeTimer = 180;
            explosion->bombImage = bombImage;
            explosion->ownerID = player.PlayerID;
            explosion->itemState = Item::ITEM_EXPLODING;
            explosion->explodePhase = ItemPotionRed::EXPLODE_WINDUP;
            items[i] = explosion;
            break;
        }
    }
}

void ItemManager::TryPickup(Player& player, int index) {
    if (items[index] == nullptr) return;
    if (items[index]->itemState != Item::ITEM_GROUND &&
        items[index]->itemState != Item::ITEM_FALLING) return;
    if (hyperPlayerID != 0 && player.PlayerID == hyperPlayerID) return;

    float dx = fabsf(player.x - items[index]->x);
    float dy = fabsf((player.y - PLAYER_HIT_CY) - items[index]->y);
    if (dx < (PLAYER_HIT_W + ITEM_HIT_W) / 2 &&
        dy < (PLAYER_HIT_H + ITEM_HIT_H) / 2) {
        items[index]->OnPickup(player);
    }
}