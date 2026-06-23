#pragma once
#include "BaseScene.h"
#include "ImageManager.h"

enum TutorialState {
    TUTO_TOP,           // トップメニュー
    TUTO_ZUKAN_SELECT,  // 武器かアイテムか選ぶ
    TUTO_WEAPON,        // 武器図鑑
    TUTO_ITEM,          // アイテム図鑑
    TUTO_CONTROL,       // 操作説明
    TUTO_RESTRICTION,   // 制限説明
};

struct ZukanEntry {
    const TCHAR* name;
    int imgHandle;
    const TCHAR* desc;
    const TCHAR* desc2;
};

struct RestrictionEntry {
    const TCHAR* name;
    const TCHAR* desc;
    const TCHAR* desc2;
};

class SceneTutorial : public BaseScene {
public:
    void Init(ImageManager& imgMgr);
    void Update() override;
    void Draw() override;
    SceneID GetNextScene() override;

private:
    ImageManager* imgMgr = nullptr;
    SceneID nextScene = SCENE_NONE;
    TutorialState state = TUTO_TOP;

    int selectIndex = 0;
    int zukanIndex = 0;

    bool prevUp = false;
    bool prevDown = false;
    bool prevEnter = false;
    bool prevEscape = false;

    int animTimer = 0;
    int animFrame = 0;

    static const int WEAPON_ENTRY_COUNT = 11;
    static const int ITEM_ENTRY_COUNT = 8;

    static const int RESTRICTION_ENTRY_COUNT = 17;
    RestrictionEntry restrictionEntries[17];
    ZukanEntry weaponEntries[11];
    ZukanEntry itemEntries[8];
};