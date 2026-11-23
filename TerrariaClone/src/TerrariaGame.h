#pragma once

#include <Engine.h>

class TerrariaGame : public Application {
public:
    TerrariaGame();

protected:
    void OnUpdate(float deltaTime) override;
    void OnRender() override;
    void OnResize(int width, int height) override;

private:
    Camera2D m_camera;
    Mesh m_ground;
};
