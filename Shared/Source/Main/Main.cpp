#include "Main.h"

bool StartApplication(MainApplication& App) 
{
    try 
    {
        App.Initialize();
        App.Run();
    } 
    catch (const std::exception& e) {
        DEBUG_ERROR("%s", e.what());
        return false;
    }
    return true;
}