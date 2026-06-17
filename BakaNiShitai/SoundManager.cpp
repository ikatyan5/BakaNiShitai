#include "SoundManager.h"

// 効果音ファイルは BakaNiShitai/Sound/ フォルダに置く。
// 拡張子(.mp3)は実ファイルに合わせて変更すること。
// ファイルが無い場合 LoadSoundMem は -1 を返すが、再生時に無視されるだけで落ちはしない。

// BGM:イワシロ音楽素材 https://iwashiro-sounds.work/
// SE :Bfxrで自作　https://www.bfxr.net/


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
    attack       = LoadSoundMem(_T("Sound/attack.wav"));
    countdown    = LoadSoundMem(_T("Sound/countdown.wav"));
    decide       = LoadSoundMem(_T("Sound/decide.wav"));
    tensai       = LoadSoundMem(_T("Sound/tensai.wav"));
    win          = LoadSoundMem(_T("Sound/win.wav"));

    // BGM（メニュー系1曲＋ゲーム用3曲）
    bgmMenu     = LoadSoundMem(_T("Sound/iwashiro_atsugiri_ham.mp3"));
    bgmGame[0]  = LoadSoundMem(_T("Sound/iwashiro_marugoshi.mp3"));
    bgmGame[1]  = LoadSoundMem(_T("Sound/iwashiro_miyako_ochisugara.mp3"));
    bgmGame[2]  = LoadSoundMem(_T("Sound/iwashiro_yanagi_bocho.mp3"));
    currentGameBgm = -1;
}

void SoundManager::PlayMenuBgm() {
    // ゲームBGMが鳴っていたら止める（ゲームからメニューに戻ってきた時）
    if (currentGameBgm != -1 && CheckSoundMem(currentGameBgm) == 1) {
        StopSoundMem(currentGameBgm);
    }
    currentGameBgm = -1;
    // メニュー系BGMがまだ鳴っていなければ流す。
    // すでに鳴っているなら何もしない＝シーンを跨いでも頭出しせず流れ続ける。
    if (bgmMenu != -1 && CheckSoundMem(bgmMenu) == 0) {
        PlaySoundMem(bgmMenu, DX_PLAYTYPE_LOOP);
    }
}

void SoundManager::PlayGameBgmRandom() {
    // メニュー系BGMを止める
    if (bgmMenu != -1 && CheckSoundMem(bgmMenu) == 1) {
        StopSoundMem(bgmMenu);
    }
    // 念のため、前のゲームBGMが残っていたら止める
    if (currentGameBgm != -1 && CheckSoundMem(currentGameBgm) == 1) {
        StopSoundMem(currentGameBgm);
    }
    std::uniform_int_distribution<int> pick(0, 2);
    int idx = pick(rng);
    currentGameBgm = bgmGame[idx];
    if (currentGameBgm != -1) {
        ChangeVolumeSoundMem(GAME_BGM_VOLUME, currentGameBgm);
        PlaySoundMem(currentGameBgm, DX_PLAYTYPE_LOOP);
    }
}

void SoundManager::StopGameBgm() {
    if (currentGameBgm != -1 && CheckSoundMem(currentGameBgm) == 1) {
        StopSoundMem(currentGameBgm);
    }
    // currentGameBgm の値は消さない＝同じ曲を後で再開できるようにする
}

void SoundManager::ResumeGameBgm() {
    if (currentGameBgm != -1 && CheckSoundMem(currentGameBgm) == 0) {
        ChangeVolumeSoundMem(GAME_BGM_VOLUME, currentGameBgm);
        PlaySoundMem(currentGameBgm, DX_PLAYTYPE_LOOP);
    }
}
