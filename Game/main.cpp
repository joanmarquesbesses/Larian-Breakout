#include "Application.h"
#include "GameLayer.h"

class ArkanoidApp : public Application
{
public:
    ArkanoidApp() : Application("Larian Arkanoid - Prova Tecnica")
    {
        std::cout << "[ArkanoidApp] Joc inicialitzat correctament.\n";
    }

    ~ArkanoidApp()
    {
        std::cout << "[ArkanoidApp] Joc tancat.\n";
    }
};

int main() {

    std::cout << "--- Iniciant Motor ---\n";

    // Instanciem l'aplicació
    ArkanoidApp* app = new ArkanoidApp();
    app->PushLayer(new GameLayer());

    // Executem el bucle infinit (aquí dins hi ha el Update i el Render)
    app->Run();

    // Neteja de memòria un cop tanquem la finestra
    delete app;

    std::cout << "--- Sortida Neta ---\n";
    return 0;
}