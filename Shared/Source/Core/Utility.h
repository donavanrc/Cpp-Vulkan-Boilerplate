#pragma once
#include "pch.h"

namespace Utility
{
    inline void Print(const std::string& message)
    {
        std::cout << message;
    }

    inline void Printf(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    inline std::string Format(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        std::string result;
        result.resize(vsnprintf(nullptr, 0, format, args));
        vsnprintf(result.data(), result.size() + 1, format, args);
        va_end(args);
        return result;
    }
}
