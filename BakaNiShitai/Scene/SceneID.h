// SceneID.h
#pragma once

// シーン遷移に使う番号。GetNextScene() がこの値で「次どこへ行くか」を伝える。
// SCENE_NONE = -1 は「移行しない（今のシーンのまま）」の合図。
// 残りは TITLE=0, MENU=1 … と0から順に並ぶ。


enum SceneID {
    SCENE_NONE = -1,
    SCENE_TITLE,
    SCENE_MENU,
    SCENE_GAME,
    SCENE_TUTORIAL,
    SCENE_SETTINGS,
};