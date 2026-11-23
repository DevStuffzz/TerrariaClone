#pragma once

#include <Engine.h>

class TerrariaGame : public Application {
public:
    TerrariaGame();

protected:
    void onUpdate(float deltaTime) override;
    void onRender() override;
    void onResize(int width, int height) override;

private:
    Camera2D camera_;
    Mesh ground_;
};
