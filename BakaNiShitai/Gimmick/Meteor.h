// Meteor.h
#pragma once

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