#include "TerrariaGame.h"

#include <exception>
#include <iostream>

int main()
{
    Application* app = nullptr;
    int result = 0;
    try {
        app = new TerrariaGame();
        app->Run();
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        result = 1;
    }
    delete app;
    return result;
}
