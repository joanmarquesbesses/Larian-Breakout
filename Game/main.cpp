#include "Application.h"
#include "GameLayer.h"

class ArkanoidApp : public Application
{
public:
    ArkanoidApp() : Application("Larian Breakout - Technical Showcase")
    {
        std::cout << "[ArkanoidApp] Game initialized successfully.\n";
    }

    ~ArkanoidApp()
    {
        std::cout << "[ArkanoidApp] Game shut down cleanly.\n";
    }
};

int main() {

    std::cout << "[Engine] --- Starting Engine ---\n";

    // Instantiate the application
    ArkanoidApp* app = new ArkanoidApp();

    // Push the main game layer into the application
    app->PushLayer(new GameLayer());

    // Execute the main game loop (Update and Render happen here)
    app->Run();

    // Memory cleanup once the window is closed
    delete app;

    std::cout << "[Engine] --- Clean Exit ---\n";

    return 0;
}