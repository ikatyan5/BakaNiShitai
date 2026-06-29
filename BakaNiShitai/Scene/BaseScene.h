// BaseScene.h
#pragma once
#include "SceneID.h"

// 全シーン（タイトル・対戦・設定など）の共通土台（抽象基底クラス）
// メインループは何も知らず ポインタで継承された Update/Draw を呼ぶ
// GetNextScene() で次に移りたいシーンを受け取る（State パターン）

class BaseScene {
public:
    virtual ~BaseScene() {}
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual SceneID GetNextScene() { return SCENE_NONE; }
};