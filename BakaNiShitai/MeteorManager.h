// MeteorManager.h
#pragma once
#include "Meteor.h"
#include "ImageManager.h"

class Player;

class MeteorManager {
public:
    void Init();
    void Update(Player& player1, Player& player2, bool tensaiMode = false);
    void Draw(ImageManager& imgMgr);
    bool HasActiveMeteor() const;

    bool hitOccurred;
    int hitWinnerID;

    void SpawnTensai(); // テンサイノツエ用5個スポーン
    int tensaiSpawnTimer;   // 次の1個を降らせるまでのタイマー
    int tensaiSpawnCount;   // あと何個降らせるか

private:
    static const int METEOR_MAX = 8;
    Meteor meteors[METEOR_MAX];

    int spawnTimer;
    int elapsedFrames;
    int edgeSpawnTimer;
    bool edgeSpawnLeft;
    float GetCurrentSpeed();
    int GetCurrentInterval();
    void Spawn(float targetX, float speed = -1.0f); // -1なら通常速度
};