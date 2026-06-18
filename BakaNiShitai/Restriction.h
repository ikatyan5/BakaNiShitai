// Restriction.h
#pragma once
#include "DxLib.h"
#include "RestrictionConfig.h"

// 妨害は SceneGame を操作対象として受け取る。
// ヘッダ同士の循環インクルードを避けるため前方宣言だけしておく。
class SceneGame;

// 妨害（制限）1種類分の振る舞いをまとめる基底クラス。
// 「状況に応じて差し替え可能な振る舞いの集合」なので Strategy パターンの Strategy 役。
// 各妨害はこれを継承し、必要なフック（説明文・準備・毎フレーム更新・描画）だけを上書きする。
// 新しい妨害を足すときは、このクラスを継承して CreateRestriction に1行追加するだけでよい。
class Restriction {
public:
    virtual ~Restriction() {}

    // カウントダウン中に出す説明文
    virtual const TCHAR* Name() const = 0;

    // ラウンド開始時の準備（プレイヤーへのシールド付与・タイマー初期化など）
    virtual void OnRoundStart(SceneGame& game) {}

    // 対戦中の毎フレーム更新（座標いじり・専用タイマー・湧き制御など）
    virtual void UpdatePlaying(SceneGame& game) {}

    // 妨害専用の画面演出
    virtual void Draw(SceneGame& game) {}
};

// RestrictionType から対応する具象 Restriction を生成するファクトリ（Factory パターン）。
// 生成した Restriction は呼び出し側が delete する責任を持つ。
Restriction* CreateRestriction(RestrictionType type);
