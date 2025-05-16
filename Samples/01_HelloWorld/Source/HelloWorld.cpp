#include "pch.h"

class HelloWorldApp
{
public:
    void Initialize();
};

void HelloWorldApp::Initialize()
{
    DEBUG_LOG("This is a message");
    DEBUG_WARNING("This is a warning");
    DEBUG_ERROR("This is a error");
    CHECK(false, "This is a fatal error");
}

int main()
{
    try 
    {
        HelloWorldApp app;
        app.Initialize();
    }
    catch(const std::exception& Exception)
    {
        DEBUG_FATAL(Exception.what());
        DEBUG_ASSERT(false, "This is assertion error");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
