#include "SceneTutorial.h"
#include "Config.h"
#include "WeaponConfig.h"
#include "DxLib.h"

static const TCHAR* TOP_ITEMS[] = {
    ("操作説明"),
    ("アイテム/武器"),
    ("各制限について"),
};
static const int TOP_COUNT = 3;

static const TCHAR* ZUKAN_ITEMS[] = {
    ("武器"),
    ("アイテム"),
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
    weaponEntries[0] = { ("鎌"),             imgMgr->weaponImages[WEAPON_KAMA],          ("シンプルなカマ"), ("特筆していうことはない") };
    weaponEntries[1] = { ("棍棒"),           imgMgr->weaponImages[WEAPON_KONBOU],        ("スタンダードで使いやすい"), ("はたき落とす回数が多めだ") };
    weaponEntries[2] = { ("剣"),             imgMgr->weaponImages[WEAPON_KEN],           ("投げた時の速度が早め"), ("振った時の判定は少し狭いから注意だ") };
    weaponEntries[3] = { ("銃"),             imgMgr->weaponImages[WEAPON_GUN],           ("遠距離から攻撃できる..と思ったら大間違いだ"), ("意外と振りが一番早い") };
    weaponEntries[4] = { ("箒"),             imgMgr->weaponImages[WEAPON_HOUKI],         ("横に広い攻撃範囲がある") , ("はたき落としがしやすい") };
    weaponEntries[5] = { ("ハンマー"),       imgMgr->weaponImages[WEAPON_HAMMER],        ("重い一撃を放つことができるぞ") , ("ジャンプしながら攻撃がおすすめ") };
    weaponEntries[6] = { ("杖"),             imgMgr->weaponImages[WEAPON_STICK],         ("振ると魔法を一回撃てる") , ("魔法は三種類 ランダムで放つぞ") };
    weaponEntries[7] = { ("ブーメラン"),     imgMgr->weaponImages[WEAPON_BOOMERANG],     ("投げると戻ってくる 当たらないように") , ("気を付けて投げるべし 当たったらダサいぞ") };
    weaponEntries[8] = { ("メメントモリ"),   imgMgr->weaponImages[WEAPON_MEMENTO_MORI],  ("攻撃すると撃てる 外したら自分がやられる") , ("覚悟を決めて撃つんだ") };
    weaponEntries[9] = { ("ピコハン"),       imgMgr->weaponImages[WEAPON_PIKOHAN],       ("特定の制限でしか出ないぞ"), ("当てるとスタン 近接の方がスタン時間が長い") };
    weaponEntries[10] = { ("テンサイノツエ"),imgMgr->weaponImages[WEAPON_TENSAI_TSUE],   ("普通の杖に見えるが…?") , ("拾うと何かしらで変化が起きているぞ") };

    // アイテム図鑑エントリ
    itemEntries[0] = { ("青ポーション"), imgMgr->potionBlue,   ("拾うとジャンプ力が上がる"),  ("場合によっては不利になるかも") };
    itemEntries[1] = { ("赤ポーション"), imgMgr->potionRed,    ("拾った後少し経ったら爆発する"), ("爆発にうまく巻き込めたら勝ちだ") };
    itemEntries[2] = { ("紫ポーション"), imgMgr->potionPurple, ("拾うとランダムな位置にワープする") , ("決して攻撃で使うものではない") };
    itemEntries[3] = { ("黄ポーション"), imgMgr->potionYellow, ("拾うと色が変わる...") , ("ほんとうにそれだけ ほんとに") };
    itemEntries[4] = { ("ハンカチ"),     imgMgr->hankachi,     ("宝の地図に見えるハンカチ") , ("拾うと少し操作が快適になる") };
    itemEntries[5] = { ("爆弾"),         imgMgr->bomb,         ("アイテムとしては出ないので") , ("安心してください") };
    itemEntries[6] = { ("バナナ"),       imgMgr->banana,       ("拾うとスタンする"), ("レアアイテムかもしれない") };
    itemEntries[7] = { ("毒キノコ"),     imgMgr->kinoko,       ("拾うと移動が反転する"), ("効果時間が短いから気を付けるべし") };


    restrictionEntries[0] = { ("なにもなし！"),                        ("制限なし！ガチ勝負だ"),                        ("実力がそのまま出るぞ") };
    restrictionEntries[1] = { ("重力がなくなった！"),                   ("ふわふわ浮いて動きが別物になる"),              ("空中戦が得意なやつが有利かも") };
    restrictionEntries[2] = { ("ジャンプ回数が無制限に！"),             ("何度でもジャンプできるぞ"),                    ("空中をうまく使って立ち回ろう") };
    restrictionEntries[3] = { ("近接が必殺だ！武器で殴って場外へ！"),   ("素手じゃ倒せない 武器で殴るんだ"),             ("素手よりも武器のほうが飛ぶぞ！") };
    restrictionEntries[4] = { ("杖ばっか降ってくるぞ！"),               ("降ってくる武器が杖だけになる"),                ("まれに暴走した杖が混ざるかも…") };
    restrictionEntries[5] = { ("ブーメランばっか降ってくるぞ！"),       ("降ってくる武器がブーメランだけになる"),        ("自分に当たらないように気をつけろ！") };
    restrictionEntries[6] = { ("！マークが出たら攻撃だ！"),             ("！マークが出たら攻撃を仕掛けよう"),           ("マークが出てない時に攻撃したら負けだ") };
    restrictionEntries[7] = { ("重力がおかしくなったぞ！"),             ("重力がランダムに変化し続ける"),                ("とんでもないものまで落ちてくるぞ") };
    restrictionEntries[8] = { ("画面がひっくり返るぞ！"),               ("色々なものが反転しまくるぞ"),                 ("早くなれたほうが勝つぞ") };
    restrictionEntries[9] = { ("加速＆武器が跳ね回るぞ！"),             ("時間で動きが加速 投げた武器は壁で跳ね回る"),   ("自分は速く 武器はどこ飛ぶか読めない！") };
    restrictionEntries[10] = { ("隕石が降ってくるぞ！"),                 ("隕石が降ってきて当たると負けだ"),          ("持っているピコハンを使って妨害だ") };
    restrictionEntries[11] = { ("強いやつから逃げ切れ！"),               ("片方がめっちゃ強くなる 逃げきれ！"),         ("強い方は攻撃キーで突進ができるぞ") };
    restrictionEntries[12] = { ("床がツルツルで滑るぞ！"),               ("地面が氷みたいに滑って急に止まれない"),       ("勢い余って場外注意 早めに動くべし") };
    restrictionEntries[13] = { ("なんか画面おかしくね？"),               ("画面が終わるぞ 目を酷使するべし"),            ("邪魔なものが飛び回るぞ") };
    restrictionEntries[14] = { ("分身が出現＋位置が入れ替わるぞ！"),   ("分身に紛れつつ 時々2人の位置が入れ替わる"),   ("どれが本体か見失うなよ！") };
    restrictionEntries[15] = { ("画面の真ん中へ引っ張られるぞ！"),       ("放っておくと画面中央へ吸い寄せられる"),       ("歩けば逆らえる 端を取るなら踏ん張れ") };
    restrictionEntries[16] = { ("停電だ！雷の一瞬だけがチャンス！"),     ("画面が真っ暗 数秒おきの雷で一瞬だけ見える"), ("雷のタイミングを逃すな！") };
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
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboard[animFrame], TRUE);
        const TCHAR** items = (state == TUTO_TOP) ? TOP_ITEMS : ZUKAN_ITEMS;
        int count = (state == TUTO_TOP) ? TOP_COUNT : ZUKAN_COUNT;
        SetFontSize(48);
        for (int i = 0; i < count; i++) {
            unsigned int color = (i == selectIndex) ? GetColor(255, 50, 50) : GetColor(255, 255, 255);
            int w = GetDrawStringWidth(items[i], lstrlen(items[i]));
            DrawString((SCREEN_W - w) / 2, 280 + i * 120, items[i], color);
        }
        SetFontSize(54);
        DrawString(40, 830, ("ESC: 戻る"), GetColor(255, 255, 255));
        SetFontSize(16);
        break;
    }

    case TUTO_WEAPON:
    case TUTO_ITEM:
    {
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->zukan, TRUE);
        ZukanEntry* entries = (state == TUTO_WEAPON) ? weaponEntries : itemEntries;
        int count = (state == TUTO_WEAPON) ? WEAPON_ENTRY_COUNT : ITEM_ENTRY_COUNT;

        // 左ページ リスト
        SetFontSize(28);
        for (int i = 0; i < count; i++) {
            unsigned int color = (i == zukanIndex) ? GetColor(200, 50, 50) : GetColor(50, 50, 50);
            DrawString(50, 80 + i * 70, ("・"), color);
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

        SetFontSize(54);
        DrawString(40, 830, ("ESC: 戻る"), GetColor(0, 0, 0));
        SetFontSize(16);
        break;
    }

    case TUTO_CONTROL:
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->blackboard[animFrame], TRUE);
        // TODO: 操作説明は後で実装
        SetFontSize(36);
        DrawString(100, 200, ("操作説明"), GetColor(255, 255, 255));
        SetFontSize(54);
        DrawString(40, 830, ("ESC: 戻る"), GetColor(255, 255, 255));
        SetFontSize(16);
        break;

    case TUTO_RESTRICTION:
    {
        DrawExtendGraphF(0.0f, 0.0f, SCREEN_W, SCREEN_H, imgMgr->zukan_restriction, TRUE);

        // 左ページ リスト
        SetFontSize(24);
        for (int i = 0; i < RESTRICTION_ENTRY_COUNT; i++) {
            unsigned int color = (i == zukanIndex) ? GetColor(200, 50, 50) : GetColor(50, 50, 50);
            DrawString(50, 55 + i * 48, ("・"), color);
            DrawString(90, 55 + i * 48, restrictionEntries[i].name, color);
        }

        // 右ページ 制限名を大きく
        SetFontSize(36);
        int nameW = GetDrawStringWidth(restrictionEntries[zukanIndex].name, lstrlen(restrictionEntries[zukanIndex].name));
        DrawString(750 + (360 - nameW) / 2, 200, restrictionEntries[zukanIndex].name, GetColor(50, 50, 50));

        // 右ページ 説明
        SetFontSize(22);
        DrawString(690, 320, restrictionEntries[zukanIndex].desc, GetColor(80, 80, 80));
        DrawString(690, 355, restrictionEntries[zukanIndex].desc2, GetColor(80, 80, 80));

        SetFontSize(54);
        DrawString(40, 850, ("ESC: 戻る"), GetColor(0, 0, 0));
        SetFontSize(16);
        break;
    }
    }

}

SceneID SceneTutorial::GetNextScene() {
    return nextScene;
}