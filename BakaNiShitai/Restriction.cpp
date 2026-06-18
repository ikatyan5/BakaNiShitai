// Restriction.cpp
#include "Restriction.h"
#include "SceneGame.h" // 各妨害は SceneGame のアクセサ経由でゲーム本体を操作する
#include <cstdlib>
#include <cmath>

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
    // 連打移動：横移動は連打式（Player側）。SceneGame側は「端に壁が出て、押し込まれたら場外負け」。
    class MashMoveRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("横移動は連打しろ！"); }

        void OnRoundStart(SceneGame& g) override {
            wallEndLeft = false;
            wallEndRight = false;
            wallEndTimer = 180 + rand() % 300;
        }

        void UpdatePlaying(SceneGame& g) override {
            if (wallEndTimer > 0) wallEndTimer--;
            else {
                int roll = rand() % 3;
                wallEndLeft = (roll == 0 || roll == 2);
                wallEndRight = (roll == 1 || roll == 2);
                wallEndTimer = 180 + rand() % 240;
            }
            CheckWall(g, g.GetPlayer1(), 2);
            CheckWall(g, g.GetPlayer2(), 1);
        }

        void DrawForeground(SceneGame& g) override {
            unsigned int wallColor = GetColor(255, 0, 0);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 180);
            if (wallEndLeft)  DrawBoxAA(0.0f, 0.0f, 50.0f, 920.0f, wallColor, TRUE);
            if (wallEndRight) DrawBoxAA(1230.0f, 0.0f, 1280.0f, 920.0f, wallColor, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }

    private:
        bool wallEndLeft = false;
        bool wallEndRight = false;
        int wallEndTimer = 0;

        // 壁が出てる端に押し込まれたら場外負け
        void CheckWall(SceneGame& g, Player& player, int winnerID) {
            if (wallEndLeft && player.x < 80.0f) g.EnterHitState(winnerID == 2, true);
            if (wallEndRight && player.x > 1250.0f) g.EnterHitState(winnerID == 2, true);
        }
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
    // 入れ替え＋分身：本体（赤と青）だけが一定間隔で位置を交換し、分身は置いてけぼり。
    // 分身は当たり判定なしの描画専用で、本体のまわりを地に足つけてウロウロする。
    // 状態（分身配列・タイマー・地面基準）はこのクラスが丸ごと所有する＝SceneGame から消える。
    class SwapRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("分身が出現！＋位置が入れ替わるぞ！"); }

        void OnRoundStart(SceneGame& g) override {
            groundY[0] = g.GetPlayer1().y; // 開始時は接地している前提
            groundY[1] = g.GetPlayer2().y;
            ScatterDecoys(decoys[0], g.GetPlayer1());
            ScatterDecoys(decoys[1], g.GetPlayer2());
            swapTimer = 180 + rand() % 120;
        }

        void UpdatePlaying(SceneGame& g) override {
            Player* owners[2] = { &g.GetPlayer1(), &g.GetPlayer2() };
            // 分身は常に勝手に漂わせ続ける
            for (int s = 0; s < 2; s++) UpdateDecoys(decoys[s], *owners[s], groundY[s]);
            // 本体だけ位置・速度を交換（分身は連れていかない＝置いてけぼり）
            if (swapTimer > 0) swapTimer--;
            else {
                Player& p1 = *owners[0];
                Player& p2 = *owners[1];
                float t;
                t = p1.x;  p1.x  = p2.x;  p2.x  = t;
                t = p1.y;  p1.y  = p2.y;  p2.y  = t;
                t = p1.vx; p1.vx = p2.vx; p2.vx = t;
                t = p1.vy; p1.vy = p2.vy; p2.vy = t;
                PlaySoundMem(g.GetSound()->setsunaSign, DX_PLAYTYPE_BACK); // 入れ替わり合図の仮の音
                swapTimer = 180 + rand() % 120; // 次の交換まで3〜5秒
            }
        }

        void Draw(SceneGame& g) override {
            Player* owners[2] = { &g.GetPlayer1(), &g.GetPlayer2() };
            ImageManager& img = g.GetImageManager();
            for (int s = 0; s < 2; s++)
                for (int i = 0; i < DECOY_COUNT; i++)
                    owners[s]->DrawDecoy(decoys[s][i].x, decoys[s][i].y, decoys[s][i].faceRight, img);
        }

    private:
        static const int DECOY_COUNT = 5; // プレイヤー1人につきの分身の数
        struct Decoy {
            float x, y;      // ワールド座標（yは本体のジャンプを倍率付きで反映）
            int moveSign;    // 進む向き（+1=右 / -1=左）
            float jumpScale; // ジャンプの高さ倍率（個体差でバラバラに見せる）
            bool faceRight;  // 向き
        };
        Decoy decoys[2][DECOY_COUNT];
        float groundY[2] = { 0.0f, 0.0f }; // 各本体の地面基準
        int swapTimer = 0;

        // 本体のまわり±420pxに散らして初期化
        static void ScatterDecoys(Decoy* arr, const Player& p) {
            const float MINX = 60.0f, MAXX = 1220.0f;
            for (int i = 0; i < DECOY_COUNT; i++) {
                float x = p.x + (float)(rand() % 841 - 420);
                if (x < MINX) x = MINX; if (x > MAXX) x = MAXX;
                arr[i].x = x;
                arr[i].y = p.y; // 縦は本体に合わせる＝地面に立つ
                arr[i].moveSign = (rand() % 2 == 0) ? 1 : -1;
                arr[i].jumpScale = 0.65f + (float)(rand() % 66) / 100.0f; // 0.65〜1.30倍
                arr[i].faceRight = (arr[i].moveSign > 0);
            }
        }

        // 毎フレームの漂い更新。縦は本体のジャンプ高さに個体差を掛けて反映、横は本体が歩く時だけ進む。
        static void UpdateDecoys(Decoy* arr, Player& p, float& groundY) {
            const float FOLLOW = 1.0f;      // 本体の速さに対する分身の速さの倍率
            const int   FLIP_CHANCE = 180;  // 1/FLIP_CHANCEで進む向きを気まぐれに変える
            const float MINX = 60.0f, MAXX = 1220.0f;
            // 地面＝本体が到達する一番下（y最大）。ジャンプ中は上書きしないので壊れない。
            if (p.y > groundY) groundY = p.y;
            float jumpHeight = groundY - p.y;
            if (jumpHeight < 0.0f) jumpHeight = 0.0f;
            for (int i = 0; i < DECOY_COUNT; i++) {
                Decoy& d = arr[i];
                d.y = groundY - jumpHeight * d.jumpScale; // 接地時は高さ0なので全員ちゃんと立つ
                if (p.vx != 0.0f) d.x += fabsf(p.vx) * FOLLOW * (float)d.moveSign;
                if (rand() % FLIP_CHANCE == 0) d.moveSign = -d.moveSign;
                if (d.x < MINX) { d.x = MINX; d.moveSign = 1; }
                if (d.x > MAXX) { d.x = MAXX; d.moveSign = -1; }
                d.faceRight = (d.moveSign > 0);
            }
        }
    };
    class TugRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("画面の真ん中へ引っ張られるぞ！"); }
        // 常に画面中央(X=640)へ引っ張る。中央から遠いほど強く引かれる。
        void UpdatePlaying(SceneGame& g) override {
            const float TUG_CENTER = 640.0f;
            const float TUG_PULL = 0.008f;
            Player& p1 = g.GetPlayer1();
            Player& p2 = g.GetPlayer2();
            p1.x += (TUG_CENTER - p1.x) * TUG_PULL;
            p2.x += (TUG_CENTER - p2.x) * TUG_PULL;
        }
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
