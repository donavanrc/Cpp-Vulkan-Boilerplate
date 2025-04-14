#pragma once
#include "pch.h"

class MainApplication
{
public:
    virtual ~MainApplication() = default;
    virtual void Initialize() = 0;
    virtual void Run() = 0;
};