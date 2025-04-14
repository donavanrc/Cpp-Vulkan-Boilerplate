#pragma once
#include "pch.h"
#include "MainApplication.h"

bool StartApplication(MainApplication& App);

#define START_APPLICATION(AppClass) \
int main() \
{ \
    AppClass App; \
    return StartApplication(App) ? 0 : 1; \
}