#include "SceneTutorial.h"
#include "WeaponConfig.h"
#include "DxLib.h"

static const TCHAR* TOP_ITEMS[] = {
    _T("操作説明"),
    _T("アイテム/武器"),
    _T("各制限について"),
};
static const int TOP_COUNT = 3;

static const TCHAR* ZUKAN_ITEMS[] = {
    _T("武器"),
    _T("アイテム"),
};
static const int ZUKAN_COUNT = 2;

void SceneTutorial::Init(ImageManager& imgMgr_) {
    imgMgr = &imgMgr_;
    nextScene = SCENE_NONE;
    state = TUTO_TOP;
    selectIndex = 0;
    zukanIndex = 0;
    prevUp = prevDown = prevEscape = false;
    prevEnter = true;
    animTimer = animFrame = 0;

    // 武器図鑑エントリ
    weaponEntries[0] = { _T("鎌"),            imgMgr->weaponImages[WEAPON_KAMA],           _T("シンプルなカマ"), _T("特筆していうことはない") };
    weaponEntries[1] = { _T("棍棒"),           imgMgr->weaponImages[WEAPON_KONBOU],        _T("スタンダードで使いやすい"), _T("はたき落とす回数が多めだ") };
    weaponEntries[2] = { _T("剣"),             imgMgr->weaponImages[WEAPON_KEN],           _T("投げた時の速度が早め"), _T("振った時の判定は少し狭いから注意だ") };
    weaponEntries[3] = { _T("銃"),             imgMgr->weaponImages[WEAPON_GUN],           _T("遠距離から攻撃できる..と思ったら大間違いだ"), _T("意外と振りが一番早い") };
    weaponEntries[4] = { _T("箒"),             imgMgr->weaponImages[WEAPON_HOUKI],         _T("横に広い攻撃範囲がある") , _T("はたき落としがしやすい") };
    weaponEntries[5] = { _T("ハンマー"),       imgMgr->weaponImages[WEAPON_HAMMER],        _T("重い一撃を放つことができるぞ") , _T("ジャンプしながら攻撃がおすすめ") };
    weaponEntries[6] = { _T("杖"),             imgMgr->weaponImages[WEAPON_STICK],         _T("振ると魔法を一回撃てる") , _T("魔法は三種類 ランダムで放つぞ") };
    weaponEntries[7] = { _T("ブーメラン"),     imgMgr->weaponImages[WEAPON_BOOMERANG],     _T("投げると戻ってくる 当たらないように") , _T("気を付けて投げるべし 当たったらダサいぞ") };
    weaponEntries[8] = { _T("メメントモリ"),   imgMgr->weaponImages[WEAPON_MEMENTO_MORI],  _T("攻撃すると撃てる 外したら自分がやられる") , _T("覚悟を決めて撃つんだ") };
    weaponEntries[9] = { _T("ピコハン"),       imgMgr->weaponImages[WEAPON_PIKOHAN],       _T("特定の制限でしか出ないぞ"), _T("当てるとスタン 近接の方がスタン時間が長い") };
    weaponEntries[10] = { _T("テンサイノツエ"), imgMgr->weaponImages[WEAPON_TENSAI_TSUE],  _T("普通の杖に見えるが…?") , _T("拾うと何かしらで変化が起きているぞ") };

    // アイテム図鑑エントリ
    itemEntries[0] = { _T("青ポーション"), imgMgr->potionBlue,   _T("拾うとジャンプ力が上がる"),  _T("場合によっては不利になるかも") };
    itemEntries[1] = { _T("赤ポーション"), imgMgr->potionRed,    _T("拾った後少し経ったら爆発する"), _T("爆発にうまく巻き込めたら勝ちだ") };
    itemEntries[2] = { _T("紫ポーション"), imgMgr->potionPurple, _T("拾うとランダムな位置にワープする") , _T("決して攻撃で使うものではない") };
    itemEntries[3] = { _T("黄ポーション"), imgMgr->potionYellow, _T("拾うと色が変わる...") , _T("ほんとうにそれだけ ほんとに") };
    itemEntries[4] = { _T("ハンカチ"),     imgMgr->hankachi,     _T("宝の地図に見えるハンカチ") , _T("拾うと少し操作が快適になる") };
    itemEntries[5] = { _T("爆弾"),         imgMgr->bomb,         _T("アイテムとしては出ないので") , _T("安心してください") };
    itemEntries[6] = { _T("バナナ"),       imgMgr->banana,       _T("拾うとスタンする"), _T("レアアイテムかもしれない") };
    itemEntries[7] = { _T("毒キノコ"),     imgMgr->kinoko,       _T("拾うと移動が反転する"), _T("効果時間が短いから気を付けるべし") };


    restrictionEntries[0] = { _T("なにもなし！"),                        _T("制限なし！ガチ勝負だ"),                        _T("実力がそのまま出るぞ") };
    restrictionEntries[1] = { _T("重力がなくなった！"),                   _T("ふわふわ浮いて動きが別物になる"),              _T("空中戦が得意なやつが有利かも") };
    restrictionEntries[2] = { _T("ジャンプ回数が無制限に！"),             _T("何度でもジャンプできるぞ"),                    _T("空中をうまく使って立ち回ろう") };
    restrictionEntries[3] = { _T("武器を投げてもダメージがないぞ！"),     _T("投げるたびにノックバックが蓄積される"),        _T("近接攻撃でまとめて吹っ飛ばせ！") };
    restrictionEntries[4] = { _T("近接ダメージがゼロだ！"),               _T("殴っても倒せない 投げて当てよう"),             _T("武器を拾い続けることが大事だ") };
    restrictionEntries[5] = { _T("杖ばっか降ってくるぞ！"),               _T("降ってくる武器が杖だけになる"),                _T("まれに暴走した杖が混ざるかも…") };
    restrictionEntries[6] = { _T("ブーメランばっか降ってくるぞ！"),       _T("降ってくる武器がブーメランだけになる"),        _T("自分に当たらないように気をつけろ！") };
    restrictionEntries[7] = { _T("！マークが出たら攻撃だ！"),             _T("！マークが出たら攻撃を仕掛けよう"),           _T("マークが出てない時に攻撃したら負けだ") };
    restrictionEntries[8] = { _T("重力がおかしくなったぞ！"),             _T("重力がランダムに変化し続ける"),                _T("とんでもないものまで落ちてくるぞ") };
    restrictionEntries[9] = { _T("画面がひっくり返るぞ！"),               _T("色々なものが反転しまくるぞ"),                 _T("早くなれたほうが勝つぞ") };
    restrictionEntries[10] = { _T("横移動は連打しろ！"),                   _T("移動は連打しないといけない"),            _T("壁に触れたらダメな時があるから気を付けるべし") };
    restrictionEntries[11] = { _T("隕石が降ってくるぞ！"),                 _T("隕石が降ってきて当たると負けだ"),          _T("持っているピコハンを使って妨害だ") };
    restrictionEntries[12] = { _T("強いやつから逃げ切れ！"),               _T("片方がめっちゃ強くなる 逃げきれ！"),         _T("強い方は攻撃キーで突進ができるぞ") };
    restrictionEntries[13] = { _T("地上で移動できないぞ！"),               _T("地面にいる間は横移動ができない"),             _T("ジャンプで移動するしかない") };
    restrictionEntries[14] = { _T("なんか画面おかしくね？"),               _T("画面が終わるぞ 目を酷使するべし"),            _T("邪魔なものが飛び回るぞ") };
}

void SceneTutorial::Update() {
    animTimer++;
    if (animTimer >= 10) {
        animTimer = 0;
        animFrame = (animFrame + 1) % 2;
    }

    bool up = CheckHitKey(KEY_INPUT_UP);
    bool down = CheckHitKey(KEY_INPUT_DOWN);
    bool enter = CheckHitKey(KEY_INPUT_RETURN);
    bool esc = CheckHitKey(KEY_INPUT_ESCAPE);

    switch (state) {
    case TUTO_TOP:
        if (up && !prevUp)       selectIndex = (selectIndex - 1 + TOP_COUNT) % TOP_COUNT;
        if (down && !prevDown)   selectIndex = (selectIndex + 1) % TOP_COUNT;
        if (enter && !prevEnter) {
            if (selectIndex == 0) { state = TUTO_CONTROL;    selectIndex = 0; }
            if (selectIndex == 1) { state = TUTO_ZUKAN_SELECT; selectIndex = 0; }
            if (selectIndex == 2) { state = TUTO_RESTRICTION; selectIndex = 0; }
        }
        if (esc && !prevEscape) nextScene = SCENE_MENU;
        break;

    case TUTO_ZUKAN_SELECT:
        if (up && !prevUp)       selectIndex = (selectIndex - 1 + ZUKAN_COUNT) % ZUKAN_COUNT;
        if (down && !prevDown)   selectIndex = (selectIndex + 1) % ZUKAN_COUNT;
        if (enter && !prevEnter) {
            if (selectIndex == 0) { state = TUTO_WEAPON; zukanIndex = 0; }
            if (selectIndex == 1) { state = TUTO_ITEM;   zukanIndex = 0; }
            selectIndex = 0;
        }
        if (esc && !prevEscape) { state = TUTO_TOP; selectIndex = 1; }
        break;

    case TUTO_WEAPON:
        if (up && !prevUp)     zukanIndex = (zukanIndex - 1 + WEAPON_ENTRY_COUNT) % WEAPON_ENTRY_COUNT;
        if (down && !prevDown) zukanIndex = (zukanIndex + 1) % WEAPON_ENTRY_COUNT;
        if (esc && !prevEscape) { state = TUTO_ZUKAN_SELECT; selectIndex = 0; }
        break;

    case TUTO_ITEM:
        if (up && !prevUp)     zukanIndex = (zukanIndex - 1 + ITEM_ENTRY_COUNT) % ITEM_ENTRY_COUNT;
        if (down && !prevDown) zukanIndex = (zukanIndex + 1) % ITEM_ENTRY_COUNT;
        if (esc && !prevEscape) { state = TUTO_ZUKAN_SELECT; selectIndex = 1; }
        break;

    case TUTO_CONTROL:
    case TUTO_RESTRICTION:
        if (up && !prevUp)     zukanIndex = (zukanIndex - 1 + RESTRICTION_ENTRY_COUNT) % RESTRICTION_ENTRY_COUNT;
        if (down && !prevDown) zukanIndex = (zukanIndex + 1) % RESTRICTION_ENTRY_COUNT;
        if (esc && !prevEscape) { state = TUTO_TOP; selectIndex = 0; }
        break;
    }

    prevUp = up; prevDown = down; prevEnter = enter; prevEscape = esc;
}

void SceneTutorial::Draw() {
    ClearDrawScreen();

    switch (state) {
    case TUTO_TOP:
    case TUTO_ZUKAN_SELECT:
    {
        DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->blackboard[animFrame], TRUE);
        const TCHAR** items = (state == TUTO_TOP) ? TOP_ITEMS : ZUKAN_ITEMS;
        int count = (state == TUTO_TOP) ? TOP_COUNT : ZUKAN_COUNT;
        SetFontSize(48);
        for (int i = 0; i < count; i++) {
            unsigned int color = (i == selectIndex) ? GetColor(255, 50, 50) : GetColor(255, 255, 255);
            int w = GetDrawStringWidth(items[i], lstrlen(items[i]));
            DrawString((1280 - w) / 2, 280 + i * 120, items[i], color);
        }
        SetFontSize(24);
        DrawString(40, 860, _T("ESC: 戻る"), GetColor(180, 180, 180));
        SetFontSize(16);
        break;
    }

    case TUTO_WEAPON:
    case TUTO_ITEM:
    {
        DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->zukan, TRUE);
        ZukanEntry* entries = (state == TUTO_WEAPON) ? weaponEntries : itemEntries;
        int count = (state == TUTO_WEAPON) ? WEAPON_ENTRY_COUNT : ITEM_ENTRY_COUNT;

        // 左ページ リスト
        SetFontSize(28);
        for (int i = 0; i < count; i++) {
            unsigned int color = (i == zukanIndex) ? GetColor(200, 50, 50) : GetColor(50, 50, 50);
            DrawString(50, 80 + i * 70, _T("・"), color);
            DrawString(90, 80 + i * 70, entries[i].name, color);
        }

        // 右ページ 画像
        DrawExtendGraphF(750.0f, 80.0f, 1110.0f, 390.0f, entries[zukanIndex].imgHandle, TRUE);

        // 右ページ 名前
        SetFontSize(32);
        int nameW = GetDrawStringWidth(entries[zukanIndex].name, lstrlen(entries[zukanIndex].name));
        DrawString(750 + (360 - nameW) / 2, 415, entries[zukanIndex].name, GetColor(50, 50, 50));

        // 右ページ 説明
        SetFontSize(22);
        DrawString(690, 475, entries[zukanIndex].desc, GetColor(80, 80, 80));
        DrawString(690, 510, entries[zukanIndex].desc2, GetColor(80, 80, 80));

        SetFontSize(20);
        DrawString(40, 880, _T("ESC: 戻る"), GetColor(100, 100, 100));
        SetFontSize(16);
        break;
    }

    case TUTO_CONTROL:
        DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->blackboard[animFrame], TRUE);
        // TODO: 操作説明は後で実装
        SetFontSize(36);
        DrawString(100, 200, _T("操作説明"), GetColor(255, 255, 255));
        SetFontSize(24);
        DrawString(40, 860, _T("ESC: 戻る"), GetColor(180, 180, 180));
        SetFontSize(16);
        break;

    case TUTO_RESTRICTION:
    {
        DrawExtendGraphF(0.0f, 0.0f, 1280.0f, 920.0f, imgMgr->zukan_restriction, TRUE);

        // 左ページ リスト
        SetFontSize(24);
        for (int i = 0; i < RESTRICTION_ENTRY_COUNT; i++) {
            unsigned int color = (i == zukanIndex) ? GetColor(200, 50, 50) : GetColor(50, 50, 50);
            DrawString(50, 60 + i * 54, _T("・"), color);
            DrawString(90, 60 + i * 54, restrictionEntries[i].name, color);
        }

        // 右ページ 制限名を大きく
        SetFontSize(36);
        int nameW = GetDrawStringWidth(restrictionEntries[zukanIndex].name, lstrlen(restrictionEntries[zukanIndex].name));
        DrawString(750 + (360 - nameW) / 2, 200, restrictionEntries[zukanIndex].name, GetColor(50, 50, 50));

        // 右ページ 説明
        SetFontSize(22);
        DrawString(690, 320, restrictionEntries[zukanIndex].desc, GetColor(80, 80, 80));
        DrawString(690, 355, restrictionEntries[zukanIndex].desc2, GetColor(80, 80, 80));

        SetFontSize(20);
        DrawString(40, 880, _T("ESC: 戻る"), GetColor(100, 100, 100));
        SetFontSize(16);
        break;
    }
    }

}

SceneID SceneTutorial::GetNextScene() {
    return nextScene;
}