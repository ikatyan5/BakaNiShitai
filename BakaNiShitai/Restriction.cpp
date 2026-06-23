// Restriction.cpp
#include "Restriction.h"
#include "Config.h"
#include "SceneGame.h" // 各妨害は SceneGame のアクセサ経由でゲーム本体を操作する
#include <cstdlib>
#include <cmath>

// 各妨害の振る舞いを表す具象クラス群（Strategy パターンの具象 Strategy 役）。
// SceneGame 内に if 分岐で散らばっていた妨害ごとの挙動・描画・準備を、ここへ集約した。
// 自己完結する妨害は状態ごとこのクラスが所有し、描画パイプラインや当たり判定など
// 横断的な処理だけは SceneGame 側に残して「今の妨害は何か」を問い合わせる形にしている。
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
    class BounceRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("武器が跳ね回るぞ！"); }
    };
    // 近接無双：武器近接で場外まで吹っ飛ばす。当たり判定の挙動は横断的なのでヒット処理側に残し、
    // ここでは「両プレイヤーに最初からシールドを配る」ラウンド準備だけを担当する。
    class MeleeMusouRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("近接が必殺だ！武器で殴って場外へ！"); }
        void OnRoundStart(SceneGame& g) override {
            g.GetPlayer1().hasShield = true; // 投げを1回だけ無効化するシールド
            g.GetPlayer2().hasShield = true;
        }
    };
    class StickOnlyRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("杖ばっか降ってくるぞ！"); }
    };
    class BoomerangOnlyRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("ブーメランばっか降ってくるぞ！"); }
    };
    // 刹那の見切り：早撃ち勝負。状態・描画・武器配り・BGM など多くが横断的なので SceneGame に残し、
    // ここでは毎フレームの進行（UpdateSetsuna）をプレイヤー更新前に委譲で呼ぶ。
    class SetsunaRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("！マークが出たら攻撃だ！"); }
        void UpdateBeforePlayers(SceneGame& g) override { g.UpdateSetsuna(); }
    };
    // 重力ランダム：重力レベルを時間で変える。レベルは武器の飛び方・UI・プレイヤー物理と
    // 広く読まれる共有パラメータなので SceneGame に残し、ここでは毎フレームの駆動を
    // プレイヤー更新前（物理に反映させるため）に委譲で呼ぶ。
    class GravityInsaneRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("重力がおかしくなったぞ！"); }
        void UpdateBeforePlayers(SceneGame& g) override { g.UpdateGravityInsane(); }
    };
    // 画面反転：上下/左右の反転パターンを時間で切り替える。反転パターンの状態と、
    // それを使った画面の反転描画は描画パイプラインに食い込むため SceneGame 側に残し、
    // ここでは毎フレームの駆動（パターン切り替え）を委譲で呼ぶ。
    class ScreenFlipRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("画面がひっくり返るぞ！"); }
        void UpdatePlaying(SceneGame& g) override { g.UpdateScreenFlip(); }
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
            if (wallEndLeft)  DrawBoxAA(0.0f, 0.0f, 50.0f, SCREEN_H, wallColor, TRUE);
            if (wallEndRight) DrawBoxAA(1230.0f, 0.0f, SCREEN_W, SCREEN_H, wallColor, TRUE);
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
    // ハイパー強い：強い側から逃げ切る。突進はプレイヤー更新前（速度を本体更新に反映させるため）、
    // 接触判定は更新後に走る。担当割り当て・時間切れ・ヒット判定は横断的なので SceneGame に残す。
    class HyperTsuyoiRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("強いやつから逃げ切れ 触れられたら負けだぞ！"); }
        void UpdateBeforePlayers(SceneGame& g) override { g.UpdateHyperDash(); }
        void UpdatePlaying(SceneGame& g) override { g.CheckHyperTouch(); }
    };
    class IceFloorRestriction : public Restriction {
    public: const TCHAR* Name() const override { return _T("床がツルツルで滑るぞ！"); }
    };
    // 画面ぼやけ：ぼかしの強さ(blurMode)は描画の合成に使う共有状態なので SceneGame に残し、
    // ここでは毎フレームの駆動（強さの切り替え＋広告更新）を委譲で呼ぶ。
    class ScreenBlurRestriction : public Restriction {
    public:
        const TCHAR* Name() const override { return _T("なんか画面おかしくね？"); }
        void UpdateBeforePlayers(SceneGame& g) override { g.UpdateScreenBlur(); }
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
    case REST_BOUND:           return new BounceRestriction();
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
