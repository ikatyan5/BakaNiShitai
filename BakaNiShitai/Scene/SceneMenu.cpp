// SceneMenu.cpp
#include "SceneMenu.h"
#include "Config.h"
#include "DxLib.h"

static const TCHAR* TIPS[] = {
    ("杖がたまに暴走して恐ろしいものを呼び出すかも..."),
    ("制限によっては 武器の投げ方が変わることがある…"),
    ("弾きが決まると 投げた武器が弾き返される！タイミングが重要だ"),
    ("ラウンドが変わると制限も変わる！前の戦い方が通じないぞ！"),
    ("武器を投げると遠くから攻撃できる！"),
    ("制限はラウンドごとに変わる！毎回ルールを確認しよう！"),
    ("ブーメランは戻ってくる！自分に当たらないように気をつけろ！"),
    ("ピコハンを近接で当てるとスタン時間が長い！投げるより近づいた方がお得！"),
    ("ジャンプ中の攻撃も侮れない！うまく使って上から仕掛けよう！"),
    ("制限「なにもなし」はガチ勝負！実力を見せる時だ！"),
    ("投げられた武器を武器で攻撃してみよう 消すことができるぞ"),
    ("ゲームは健康に害がない程度に遊ぼうね！"),
    ("動かしてるキャラの名前募集中"),
    ("Tipsを見てくれてありがとう 次にいいことが書いてあるといいね"),
    ("空中にいる時 下キーを押すと素早く落下できるぞ！"),
    ("この文が出てるということは あなたはこの文を見ているということです"),
    ("もしかして:　暇"),
    ("何かおもしろくなりそうな要素を思いついたら 制作者に伝えて下さい"),
    // 他のTipsも追加！
};
static const int TIPS_COUNT = 18;

static const TCHAR* MENU_ITEMS[] = {
    ("たたかう"),
    ("チュートリ"),
    ("設定"),
};
static const int MENU_COUNT = 3;

// マウス判定用：各メニュー項目のおおよその矩形 {x1, y1, x2, y2}（Drawの描画範囲に対応）
static const int MENU_RECT[MENU_COUNT][4] = {
    { 170, 140, 620, 340 }, // たたかう
    { 180, 370, 520, 490 }, // チュートリ
    { 170, 490, 390, 690 }, // 設定
};

void SceneMenu::Init(ImageManager& imgMgr_, SoundManager& sndMgr_) {
    imgMgr = &imgMgr_;
    sound = &sndMgr_;
    // メニュー系BGMを流す。タイトルや設定から来た時はすでに鳴っているので継続、
    // ゲームから戻ってきた時はゲームBGMを止めてメニューBGMを復活させる。
    sound->PlayMenuBgm();
    nextScene = SCENE_NONE;
    selectIndex = 0;
    prevUp = false;
    prevDown = false;
    prevEnter = true;
    prevMouseLeft = false;
}

void SceneMenu::Update() {
    tipsTimer++;
    if (tipsTimer >= 600) {
        tipsTimer = 0;
        int next = rand() % (TIPS_COUNT - 1);
        if (next >= tipsIndex) next++;
        tipsIndex = next;
    }
    animTimer++;
    if (animTimer >= 10) {
        animTimer = 0;
        animFrame = (animFrame + 1) % 2;
    }
    animTimer2++;
    if (animTimer2 >= 40) { // 10→20で半分の速度
        animTimer2 = 0;
        animFrame2 = (animFrame2 + 1) % 2;
    }

    // 上下キーは矢印に加えてW/Sでも操作できる
    bool up = CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W);
    bool down = CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S);
    bool enter = CheckHitKey(KEY_INPUT_RETURN);

    // マウス：項目の上にカーソルがあれば、その項目を選択中にする
    int mx, my;
    GetMousePoint(&mx, &my);
    int hoverItem = -1;
    for (int i = 0; i < MENU_COUNT; i++) {
        if (mx >= MENU_RECT[i][0] && mx <= MENU_RECT[i][2] &&
            my >= MENU_RECT[i][1] && my <= MENU_RECT[i][3]) {
            hoverItem = i;
        }
    }
    if (hoverItem != -1 && hoverItem != selectIndex) {
        selectIndex = hoverItem;
        PlaySoundMem(sound->menu, DX_PLAYTYPE_BACK);
    }

    if (up && !prevUp) {
        selectIndex = (selectIndex - 1 + MENU_COUNT) % MENU_COUNT;
        PlaySoundMem(sound->menu, DX_PLAYTYPE_BACK);
    }
    if (down && !prevDown) {
        selectIndex = (selectIndex + 1) % MENU_COUNT;
        PlaySoundMem(sound->menu, DX_PLAYTYPE_BACK);
    }

    // 決定：Enter か、項目の上での左クリック
    bool mouseLeft = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
    bool keyDecide = enter && !prevEnter;
    bool mouseDecide = mouseLeft && !prevMouseLeft && hoverItem != -1;
    if (keyDecide || mouseDecide) {
        PlaySoundMem(sound->decide, DX_PLAYTYPE_BACK);
        if (selectIndex == 0) nextScene = SCENE_GAME;
        if (selectIndex == 1) nextScene = SCENE_TUTORIAL;
        if (selectIndex == 2) nextScene = SCENE_SETTINGS;
    }

    prevUp = up;
    prevDown = down;
    prevEnter = enter;
    prevMouseLeft = mouseLeft;
}

void SceneMenu::Draw() {
    ClearDrawScreen();
    DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboard[animFrame], TRUE);

    int idx0 = (selectIndex == 0) ? (2 + animFrame) : animFrame;
    int idx1 = (selectIndex == 1) ? (2 + animFrame) : animFrame;
    int idx2 = (selectIndex == 2) ? (2 + animFrame) : animFrame;

    // たたかう（大きめ）500x200基準
    DrawExtendGraphF(170.0f, 140.0f, 170.0f + 450.0f, 140.0f + 200.0f, imgMgr->menuTatakau[idx0], TRUE);


    // チュートリ（ふつう）
    DrawExtendGraphF(180.0f, 370.0f, 170.0f + 350.0f, 370.0f + 120.0f, imgMgr->menuTutorial[idx1], TRUE);

    // 非選択：0と1を切り替え、選択中：2と3を切り替え
    DrawExtendGraphF(170.0f, 490.0f, 170.0f + 220.0f, 480.0f + 210.0f, imgMgr->menuSetting[idx2], TRUE);

    // 右側アニメ
    int* currentAnim = (selectIndex == 0) ? imgMgr->menuAnimTatakau :
        (selectIndex == 1) ? imgMgr->menuAnimTutorial :
        imgMgr->menuAnimSetting;
    DrawExtendGraphF(700.0f, 250.0f, 1150.0f, 700.0f, currentAnim[animFrame2], TRUE);

    SetFontSize(30);
    int tipsW = GetDrawStringWidth(TIPS[tipsIndex], lstrlen(TIPS[tipsIndex]));
    DrawString((SCREEN_W - tipsW) / 2, 70, TIPS[tipsIndex], GetColor(255, 255, 255));

    // 画面下に操作説明を表示
    SetFontSize(26);
    const TCHAR* guide = ("WS / ↑↓ : えらぶ　　Enter / 左クリック : けってい");
    int gw = GetDrawStringWidth(guide, lstrlen(guide));
    DrawString((SCREEN_W - gw) / 2, SCREEN_H - 130, guide, GetColor(255, 255, 255));
    SetFontSize(16);
}

SceneID SceneMenu::GetNextScene() {
    return nextScene;
}