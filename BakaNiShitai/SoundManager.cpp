#include "SoundManager.h"

// 効果音ファイルは BakaNiShitai/Sound/ フォルダに置く。
// 拡張子(.mp3)は実ファイルに合わせて変更すること。
// ファイルが無い場合 LoadSoundMem は -1 を返すが、再生時に無視されるだけで落ちはしない。
void SoundManager::Load() {
    hit          = LoadSoundMem(_T("Sound/damage.wav"));
    explosion    = LoadSoundMem(_T("Sound/explosion.wav"));
    pickup       = LoadSoundMem(_T("Sound/pickup.wav"));
    mementoMori  = LoadSoundMem(_T("Sound/mementoMori.wav"));
    menu         = LoadSoundMem(_T("Sound/menu.wav"));
    parry        = LoadSoundMem(_T("Sound/parry.wav"));
    setsuna      = LoadSoundMem(_T("Sound/setsuna.wav"));
    weaponThrow  = LoadSoundMem(_T("Sound/throw.wav"));
    weaponPickup = LoadSoundMem(_T("Sound/weapon_pickup.wav"));
    jump         = LoadSoundMem(_T("Sound/jump.wav"));
    setsunaSign  = LoadSoundMem(_T("Sound/setsuna_sign.wav"));
    deflect      = LoadSoundMem(_T("Sound/deflect.wav"));
    meteor       = LoadSoundMem(_T("Sound/meteor.wav"));
}
