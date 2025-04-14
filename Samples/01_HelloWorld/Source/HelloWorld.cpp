#include "Main/Main.h"

class HelloWorld : public MainApplication
{
public:
    void Initialize() override;
    void Run() override;
};

void HelloWorld::Initialize()
{
    DEBUG_CRITICAL("Critical error message");
    DEBUG_DISPLAY("Display message");
    DEBUG_ERROR("Error message");
    DEBUG_INFO("Info message");
    DEBUG_LOG("Log message");
    DEBUG_WARNING("Warning message");
    DEBUG_TRACE("Trace message");
    DEBUG_ASSERT(false, "Assertion error message");
}

void HelloWorld::Run()
{
}

START_APPLICATION(HelloWorld)
