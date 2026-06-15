// GameSettings.h
#pragma once
#include "DxLib.h"

struct GameSettings {
    bool p1UseGamepad = false;
    int  p1PadID = DX_INPUT_PAD1;
    bool p2UseGamepad = false;
    int  p2PadID = DX_INPUT_PAD1;
};