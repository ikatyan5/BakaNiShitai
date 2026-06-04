#pragma once

class BaseScene {
public:
    virtual ~BaseScene() {}
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual int GetNextScene() { return -1; } // -1‚Í‘JˆÚ‚È‚µ
};