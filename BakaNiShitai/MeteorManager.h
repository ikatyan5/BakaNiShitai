// MeteorManager.h
#pragma once
#include "Meteor.h"
#include "ImageManager.h"

class Player;

class MeteorManager {
public:
    void Init();
    void Update(Player& player1, Player& player2);
    void Draw(ImageManager& imgMgr);

    bool hitOccurred;
    int hitWinnerID;

private:
    static const int METEOR_MAX = 8;
    Meteor meteors[METEOR_MAX];

    int spawnTimer;
    int elapsedFrames;

    float GetCurrentSpeed();
    int GetCurrentInterval();
    void Spawn();
};