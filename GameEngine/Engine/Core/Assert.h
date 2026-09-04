#pragma once
#include <cstdlib>
#include <string_view>
#include "Logger.h"

// Debug break cross-platform
#ifdef _WIN32
    #define GE_DEBUGBREAK() __debugbreak()
#else
    #include <csignal>
    #define GE_DEBUGBREAK() std::raise(SIGTRAP)
#endif

// Assert with message
#define GE_ASSERT(condition, ...) \
    do { \
        if (!(condition)) { \
            GE_CRITICAL("Assertion failed: {} | File: {} | Line: {}", \
                #condition, __FILE__, __LINE__); \
            GE_DEBUGBREAK(); \
            std::abort(); \
        } \
    } while(0)

// Assert without message
#define GE_ASSERT_MSG(condition, msg) \
    do { \
        if (!(condition)) { \
            GE_CRITICAL("Assertion failed: {} | Message: {} | File: {} | Line: {}", \
                #condition, msg, __FILE__, __LINE__); \
            GE_DEBUGBREAK(); \
            std::abort(); \
        } \
    } while(0)

// Static (compile-time) assert
#define GE_STATIC_ASSERT(condition, msg) static_assert(condition, msg)

// Verify (runs in release, but doesn't break — just logs)
#define GE_VERIFY(condition, ...) \
    do { \
        if (!(condition)) { \
            GE_ERROR("Verify failed: {}", #condition); \
        } \
    } while(0)
