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

#ifdef _DEBUG
#define PRINT_TAGGED(Tag, ...) \
{ \
    Utility::Printf("\n[%s] ", Tag); \
    Utility::Printf(__VA_ARGS__); \
    Utility::Printf("\n"); \
}
#define DEBUG_PRINT_TAGGED(Tag, ...) \
{ \
    PRINT_TAGGED(Tag, __VA_ARGS__); \
    Utility::Printf("[%s:%d] ", __FILE__, __LINE__); \
    Utility::Printf("\n"); \
}
#define DEBUG_ASSERT(Condition, ...) \
{ \
    if (!(Condition)) \
    { \
        DEBUG_PRINT_TAGGED("ASSERTION ERROR", __VA_ARGS__); \
        std::abort(); \
    } \
}
#define DEBUG_LOG(...) PRINT_TAGGED("LOG", __VA_ARGS__)
#define DEBUG_ERROR(...) DEBUG_PRINT_TAGGED("ERROR", __VA_ARGS__)
#define DEBUG_FATAL(...) DEBUG_PRINT_TAGGED("FATAL", __VA_ARGS__)
#define DEBUG_WARNING(...) DEBUG_PRINT_TAGGED("WARNING", __VA_ARGS__)
#else
#define DEBUG_ASSERT(Condition, ...)
#define DEBUG_LOG(...)
#define DEBUG_ERROR(...)
#define DEBUG_FATAL(...)
#define DEBUG_WARNING(...)
#endif

#define CHECK(Condition, ...) if(!(Condition)) throw std::runtime_error(Utility::Format(__VA_ARGS__))
