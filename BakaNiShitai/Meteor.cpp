// Meteor.cpp
#include "Meteor.h"

void Meteor::Init() {
    x = y = 0.0f;
    vx = vy = 0.0f;
    targetX = 0.0f;
    angle = 0.0f;
    active = false;
}

void Meteor::Update() {
    if (!active) return;
    x += vx;
    y += vy;
    angle += 0.1f;
    if (y > 1220.0f) active = false;
}