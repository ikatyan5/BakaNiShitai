#pragma once
#include "Item.h"
#include "ImageManager.h"

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
    void Update(Player& player1, Player& player2);
    void Draw();
    void SpawnItem();

    int bombImage;
    bool hitOccurred;  // 爆発ヒットがあったか
    int hitWinnerID;   // 勝ったプレイヤーのID（1か2）
    bool isExploding;

private:
    Item* items[ITEM_MAX];
    int itemSpawnTimer;
    ImageManager* imgMgr;
};