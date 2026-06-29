// Meteor.h
#pragma once

// 隕石1個ぶんのデータ（位置・速度・狙う先）
struct Meteor {
    float x, y;
    float vx, vy;
    float targetX;
    float targetY;
    float angle;
    bool active;

    void Init();
    void Update();
};