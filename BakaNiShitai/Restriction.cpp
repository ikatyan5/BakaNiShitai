// Restriction.cpp
#include "Restriction.h"

// 各妨害の振る舞いを表す具象クラス群（Strategy パターンの具象 Strategy 役）。
// 段階リファクタリング中：まずは表示名だけを各クラスへ持たせ、
// SceneGame 側に散らばっている挙動・描画・準備は今後ここへ順次移植していく。
namespace {

    class NoneRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("なにもなし！"); }
    };
    class GravityZeroRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("重力がなくなった！"); }
    };
    class HoverJumpRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("ジャンプ回数が無制限に！"); }
    };
    class ThrowNoDamageRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("武器を投げてもダメージがないぞ！"); }
    };
    class MeleeMusouRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("近接が必殺だ！武器で殴って場外へ！"); }
    };
    class StickOnlyRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("杖ばっか降ってくるぞ！"); }
    };
    class BoomerangOnlyRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("ブーメランばっか降ってくるぞ！"); }
    };
    class SetsunaRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("！マークが出たら攻撃だ！"); }
    };
    class GravityInsaneRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("重力がおかしくなったぞ！"); }
    };
    class ScreenFlipRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("画面がひっくり返るぞ！"); }
    };
    class MashMoveRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("横移動は連打しろ！"); }
    };
    class MeteorRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("隕石が降ってくるぞ 相手をスタンさせよう！"); }
    };
    class HyperTsuyoiRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("強いやつから逃げ切れ 触れられたら負けだぞ！"); }
    };
    class IceFloorRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("床がツルツルで滑るぞ！"); }
    };
    class ScreenBlurRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("なんか画面おかしくね？"); }
    };
    class SwapRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("分身が出現！＋位置が入れ替わるぞ！"); }
    };
    class TugRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("画面の真ん中へ引っ張られるぞ！"); }
    };

} // namespace

Restriction* CreateRestriction(RestrictionType type) {
    switch (type) {
    case REST_GRAVITY_ZERO:    return new GravityZeroRestriction();
    case REST_HOVER_JUMP:      return new HoverJumpRestriction();
    case REST_THROW_NO_DAMAGE: return new ThrowNoDamageRestriction();
    case REST_MELEE_MUSOU:     return new MeleeMusouRestriction();
    case REST_STICK_ONLY:      return new StickOnlyRestriction();
    case REST_BOOMERANG_ONLY:  return new BoomerangOnlyRestriction();
    case REST_SETSUNA:         return new SetsunaRestriction();
    case REST_GRAVITY_INSANE:  return new GravityInsaneRestriction();
    case REST_SCREEN_FLIP:     return new ScreenFlipRestriction();
    case REST_MASH_MOVE:       return new MashMoveRestriction();
    case REST_METEOR:          return new MeteorRestriction();
    case REST_HYPETSUYOI:      return new HyperTsuyoiRestriction();
    case REST_ICE_FLOOR:       return new IceFloorRestriction();
    case REST_SCREEN_BLUR:     return new ScreenBlurRestriction();
    case REST_SWAP:            return new SwapRestriction();
    case REST_TUG:             return new TugRestriction();
    default:                   return new NoneRestriction();
    }
}
