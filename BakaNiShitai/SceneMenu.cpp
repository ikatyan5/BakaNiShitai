// SceneMenu.cpp
#include "SceneMenu.h"
#include "DxLib.h"

static const TCHAR* TIPS[] = {
    _T("杖がたまに暴走して恐ろしいものを呼び出すかも..."),
    _T("制限によっては 武器の投げ方が変わることがある…"),
    _T("弾きが決まると 投げた武器が弾き返される！タイミングが重要だ"),
    _T("ラウンドが変わると制限も変わる！前の戦い方が通じないぞ！"),
    _T("武器を投げると遠くから攻撃できる！"),
    _T("制限はラウンドごとに変わる！毎回ルールを確認しよう！"),
    _T("ブーメランは戻ってくる！自分に当たらないように気をつけろ！"),
    _T("ピコハンを近接で当てるとスタン時間が長い！投げるより近づいた方がお得！"),
    _T("ジャンプ中の攻撃も侮れない！うまく使って上から仕掛けよう！"),
    _T("制限「なにもなし」はガチ勝負！実力を見せる時だ！"),
    _T("投げられた武器を武器で攻撃してみよう 消すことができるぞ"),
    _T("ゲームは健康に害がない程度に遊ぼうね！"),
    _T("動かしてるキャラの名前募集中"),
    _T("Tipsを見てくれてありがとう 次にいいことが書いてあるといいね"),
    _T("空中にいる時 下キーを押すと素早く落下できるぞ！"),
    _T("この文が出てるということは あなたはこの文を見ているということです"),
    _T("もしかして:　暇"),
    _T("何かおもしろくなりそうな要素を思いついたら 制作者に伝えて下さい"),
    // 他のTipsも追加してね～
};
static const int TIPS_COUNT = 18;

static const TCHAR* MENU_ITEMS[] = {
    _T("たたかう"),
    _T("チュートリ"),
    _T("設定"),
};
static const int MENU_COUNT = 3;

void SceneMenu::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    nextScene = SCENE_NONE;
    selectIndex = 0;
    prevUp = false;
    prevDown = false;
    prevEnter = true;
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

    bool up = CheckHitKey(KEY_INPUT_UP);
    bool down = CheckHitKey(KEY_INPUT_DOWN);
    bool enter = CheckHitKey(KEY_INPUT_RETURN);

    if (up && !prevUp) {
        selectIndex = (selectIndex - 1 + MENU_COUNT) % MENU_COUNT;
    }
    if (down && !prevDown) {
        selectIndex = (selectIndex + 1) % MENU_COUNT;
    }
    if (enter && !prevEnter) {
        if (selectIndex == 0) nextScene = SCENE_GAME;
        if (selectIndex == 1) nextScene = SCENE_TUTORIAL;
        if (selectIndex == 2) nextScene = SCENE_SETTINGS;
    }

    prevUp = up;
    prevDown = down;
    prevEnter = enter;
}

void SceneMenu::Draw() {
    ClearDrawScreen();
    DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->blackboard[animFrame], TRUE);

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
    DrawString((1280 - tipsW) / 2, 70, TIPS[tipsIndex], GetColor(255, 255, 255));
    SetFontSize(16);
}

SceneID SceneMenu::GetNextScene() {
    return nextScene;
}