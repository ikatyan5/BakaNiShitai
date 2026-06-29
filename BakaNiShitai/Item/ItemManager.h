#pragma once
#include "Item.h"
#include "ImageManager.h"
#include "RestrictionManager.h"
#include "SoundManager.h"

// 落ちてくるアイテムの湧き・更新・描画・当たり判定をまとめて持つ係。

class Player;

const int ITEM_MAX = 5;

enum ItemType {
    ITEM_POTION_BLUE,
    ITEM_POTION_RED,
    ITEM_POTION_PURPLE,
    ITEM_POTION_YELLOW,
    ITEM_HANKACHI,
    ITEM_BANANA,
    ITEM_KINOKO,
    ITEM_TYPE_MAX
};

class ItemManager {
public:
    void Init(ImageManager& imgMgr, SoundManager& sndMgr);
    void Update(Player& player1, Player& player2, const RestrictionManager& restrictions);
    void Draw();
    void SpawnItem(const RestrictionManager& restrictions);
    void CheckExplode(Player& player);
    void TryPickup(Player& player, int index);

    int bombImage;
    bool hitOccurred;  // 爆発ヒットがあったか
    int hitWinnerID;   // 勝ったプレイヤーのID（1か2）
    int hyperPlayerID; // 0なら無効
    bool isExploding;

private:
    // 現在の妨害に応じて降ってくるアイテム種別を決める（デバッグ／本番で共通）
    ItemType SelectItemType(const RestrictionManager& restrictions);

    Item* items[ITEM_MAX];
    int itemSpawnTimer;
    ImageManager* imgMgr;
    SoundManager* sound;
};