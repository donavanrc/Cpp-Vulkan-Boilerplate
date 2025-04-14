#pragma once
#include "pch.h"

enum class SeverityLevel
{
    None     = 0, // No debug output
    Critical = 1, // Only critical errors
    Error    = 2, // Errors and critical errors
    Warning  = 3, // Warnings, errors, and critical errors
    Info     = 4, // Info, warnings, errors, and critical errors
    Trace    = 5, // Trace, info, warnings, errors, and critical errors
    Display  = 6, // Display, trace, info, warnings, errors, and critical errors
    Log      = 7, // All debug output including log messages
    All      = 8  // All possible debug output
};

enum class DebugType
{
    None     = 0, // Unknown or default
    Assert   = 1, // Assertion Error
    Critical = 2, // Critical
    Display  = 3, // Display
    Error    = 4, // Error
    Info     = 5, // Info
    Log      = 6, // Log
    Trace    = 7, // Trace
    Warning  = 8  // Warning
};

static inline const char* GetColorCode(DebugType Type)
{
    switch (Type)
    {
        case DebugType::Assert:   return "\033[031m"; // Vermelho
        case DebugType::Critical: return "\033[031m"; // Vermelho
        case DebugType::Display:  return "\033[092m"; // Verde
        case DebugType::Error:    return "\033[091m"; // Vermelho claro
        case DebugType::Info:     return "\033[094m"; // Azul
        case DebugType::Log:      return "\033[0m";   // Normal
        case DebugType::Trace:    return "\033[095m"; // Magenta
        case DebugType::Warning:  return "\033[093m"; // Amarelo
        default:                  return "\033[0m";   // Normal
    }
}

static inline const char* GetTagString(DebugType Type)
{
    switch (Type)
    {
        case DebugType::Assert:   return "ASSERTION ERROR";
        case DebugType::Critical: return "CRITICAL";
        case DebugType::Display:  return "DISPLAY";
        case DebugType::Error:    return "ERROR";
        case DebugType::Info:     return "INFO";
        case DebugType::Log:      return "LOG";
        case DebugType::Trace:    return "TRACE";
        case DebugType::Warning:  return "WARNING";
        default:                  return "";
    }
}

class Debug
{
public:
    static void SetSeverityLevel(SeverityLevel severity)
    {
        sm_Severity = severity;
    }

    static SeverityLevel GetSeverityLevel()
    {
        return sm_Severity;
    }

    static bool IsSeverityEnabled(SeverityLevel severity)
    {
        return static_cast<int>(sm_Severity) >= static_cast<int>(severity);
    }

    static inline void PrintMessage(DebugType Type, const char* Format, ...)
    {
#ifdef _DEBUG
        if (!ShouldPrintMessage(Type))
            return;

        const char* Color = GetColorCode(Type);
        const char* Tag = GetTagString(Type);

        PrePrint(Color, Tag);

        va_list args;
        va_start(args, Format);
        PrintFormattedMessage(Format, args);
        va_end(args);

        PostPrint(Color);
#endif
    }

    static inline void PrintQualifiedMessage(DebugType Type, const char* File, int Line, const char* Format, ...)
    {
#ifdef _DEBUG
        if (!ShouldPrintMessage(Type))
            return;

        const char* Color = GetColorCode(Type);
        const char* Tag = GetTagString(Type);

        PrePrint(Color, Tag);

        va_list args;
        va_start(args, Format);
        PrintFormattedMessage(Format, args);
        va_end(args);

        Utility::Printf("\n[%s:%d]", File, Line);
        PostPrint(Color);
#endif
    }

private:
#ifdef _DEBUG
    static inline SeverityLevel sm_Severity = SeverityLevel::All;
#else
    static inline SeverityLevel sm_Severity = SeverityLevel::Error;
#endif

#ifdef _DEBUG
    static inline bool ShouldPrintMessage(DebugType Type)
    {
        SeverityLevel requiredSeverity;
        switch (Type)
        {
            case DebugType::Critical:
            case DebugType::Assert:
                requiredSeverity = SeverityLevel::Critical;
                break;
            case DebugType::Error:
                requiredSeverity = SeverityLevel::Error;
                break;
            case DebugType::Warning:
                requiredSeverity = SeverityLevel::Warning;
                break;
            case DebugType::Info:
                requiredSeverity = SeverityLevel::Info;
                break;
            case DebugType::Trace:
                requiredSeverity = SeverityLevel::Trace;
                break;
            case DebugType::Display:
                requiredSeverity = SeverityLevel::Display;
                break;
            case DebugType::Log:
                requiredSeverity = SeverityLevel::Log;
                break;
            default:
                requiredSeverity = SeverityLevel::All;
                break;
        }
        
        return IsSeverityEnabled(requiredSeverity);
    }

    static inline void PrePrint(const char* Color, const char* Tag)
    {
        Utility::Print(Color);
        Utility::Printf("[%s] ", Tag);
    }

    static inline void PrintFormattedMessage(const char* Format, va_list Args)
    {
        char buffer[4096];
        vsnprintf(buffer, sizeof(buffer), Format, Args);
        Utility::Print(buffer);
    }

    static inline void PostPrint(const char* Color)
    {
        Utility::Print("\033[0m");
        Utility::Print("\n");
    }
#endif
};

#ifdef _DEBUG
#define DEBUG_ASSERT(Condition, ...) \
if (!(Condition)) \
{ \
    Debug::PrintQualifiedMessage(DebugType::Assert, __FILE__, __LINE__, __VA_ARGS__); \
    std::abort(); \
}
#define DEBUG_CRITICAL(...)          Debug::PrintQualifiedMessage(DebugType::Critical, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG_CRITICAL_MESSAGE(...)  Debug::PrintMessage(DebugType::Critical, __VA_ARGS__)
#define DEBUG_DISPLAY(...)           Debug::PrintMessage(DebugType::Display, __VA_ARGS__)
#define DEBUG_ERROR(...)             Debug::PrintQualifiedMessage(DebugType::Error, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG_ERROR_MESSAGE(...)     Debug::PrintMessage(DebugType::Error, __VA_ARGS__)
#define DEBUG_INFO(...)              Debug::PrintMessage(DebugType::Info, __VA_ARGS__)
#define DEBUG_LOG(...)               Debug::PrintMessage(DebugType::Log, __VA_ARGS__)
#define DEBUG_TRACE(...)             Debug::PrintMessage(DebugType::Trace, __VA_ARGS__)
#define DEBUG_WARNING(...)           Debug::PrintQualifiedMessage(DebugType::Warning, __FILE__, __LINE__, __VA_ARGS__)
#define DEBUG_WARNING_MESSAGE(...)   Debug::PrintMessage(DebugType::Warning, __VA_ARGS__)
#else
#define DEBUG_ASSERT(Condition, ...)
#define DEBUG_CRITICAL(...)
#define DEBUG_CRITICAL_MESSAGE(...)
#define DEBUG_DISPLAY(...)
#define DEBUG_ERROR(...)
#define DEBUG_ERROR_MESSAGE(...)
#define DEBUG_INFO(...)
#define DEBUG_LOG(...)
#define DEBUG_TRACE(...)
#define DEBUG_WARNING(...)
#define DEBUG_WARNING_MESSAGE(...)
#endif