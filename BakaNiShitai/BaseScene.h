// BaseScene.h
#pragma once
#include "SceneID.h"
class BaseScene {
public:
    virtual ~BaseScene() {}
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual SceneID GetNextScene() { return SCENE_NONE; }
};