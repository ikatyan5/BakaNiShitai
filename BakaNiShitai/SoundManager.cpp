#include "SoundManager.h"

// 効果音ファイルは BakaNiShitai/Sound/ フォルダに置く。
// 拡張子(.mp3)は実ファイルに合わせて変更すること。
// ファイルが無い場合 LoadSoundMem は -1 を返すが、再生時に無視されるだけで落ちはしない。
void SoundManager::Load() {
    hit         = LoadSoundMem(_T("Sound/damage.wav"));
    explosion   = LoadSoundMem(_T("Sound/explosion.wav"));
    pickup      = LoadSoundMem(_T("Sound/pickup.wav"));
    mementoMori = LoadSoundMem(_T("Sound/mementoMori.wav"));
}
