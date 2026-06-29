#include "SoundManager.h"

// 効果音ファイルは BakaNiShitai/Sound/ フォルダに置く。
// 拡張子(.mp3)は実ファイルに合わせて変更すること。
// ファイルが無い場合 LoadSoundMem は -1 を返すが、再生時に無視されるだけで落ちはしない。

// BGM:イワシロ音楽素材 https://iwashiro-sounds.work/
// SE :Bfxrで自作　https://www.bfxr.net/


void SoundManager::Load() {
    hit          = LoadSoundMem("Sound/damage.wav");
    explosion    = LoadSoundMem("Sound/explosion.wav");
    pickup       = LoadSoundMem("Sound/pickup.wav");
    mementoMori  = LoadSoundMem("Sound/mementoMori.wav");
    menu         = LoadSoundMem("Sound/menu.wav");
    parry        = LoadSoundMem("Sound/parry.wav");
    setsuna      = LoadSoundMem("Sound/setsuna.wav");
    weaponThrow  = LoadSoundMem("Sound/throw.wav");
    weaponPickup = LoadSoundMem("Sound/weapon_pickup.wav");
    jump         = LoadSoundMem("Sound/jump.wav");
    setsunaSign  = LoadSoundMem("Sound/setsuna_sign.wav");
    deflect      = LoadSoundMem("Sound/deflect.wav");
    meteor       = LoadSoundMem("Sound/meteor.wav");
    attack       = LoadSoundMem("Sound/attack.wav");
    countdown    = LoadSoundMem("Sound/countdown.wav");
    decide       = LoadSoundMem("Sound/decide.wav");
    tensai       = LoadSoundMem("Sound/tensai.wav");
    win          = LoadSoundMem("Sound/win.wav");

    // BGM（メニュー系1曲＋ゲーム用3曲）
    bgmMenu     = LoadSoundMem("Sound/iwashiro_atsugiri_ham.mp3");
    bgmGame[0]  = LoadSoundMem("Sound/iwashiro_marugoshi.mp3");
    bgmGame[1]  = LoadSoundMem("Sound/iwashiro_miyako_ochisugara.mp3");
    bgmGame[2]  = LoadSoundMem("Sound/iwashiro_yanagi_bocho.mp3");
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
