#pragma once
#include "Item.h"
#include "ImageManager.h"
#include "RestrictionManager.h"

class Player;

const int ITEM_MAX = 5;

enum ItemType {
    ITEM_POTION_BLUE,
    ITEM_POTION_RED,
    ITEM_POTION_PURPLE,
    ITEM_POTION_YELLOW,
    ITEM_HANKACHI,
    ITEM_TYPE_MAX
};

class ItemManager {
public:
    void Init(ImageManager& imgMgr);
    void Update(Player& player1, Player& player2, const RestrictionManager& restrictions);
    void Draw();
    void SpawnItem(const RestrictionManager& restrictions);

    int bombImage;
    bool hitOccurred;  // 爆発ヒットがあったか
    int hitWinnerID;   // 勝ったプレイヤーのID（1か2）
    int hyperPlayerID; // 0なら無効
    bool isExploding;

private:
    Item* items[ITEM_MAX];
    int itemSpawnTimer;
    ImageManager* imgMgr;
};