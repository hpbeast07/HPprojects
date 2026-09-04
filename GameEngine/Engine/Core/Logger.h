#pragma once
#include <string>
#include <string_view>
#include <fstream>
#include <mutex>
#include <memory>
#include <format>
#include <chrono>
#include <iostream>
#include <source_location>

namespace GE {

enum class LogLevel : uint8_t {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Off
};

class Logger {
public:
    static Logger& Get();

    void SetLevel(LogLevel level) { m_Level = level; }
    void SetLogFile(const std::string& path);

    template<typename... Args>
    void Log(LogLevel level, std::string_view fmt, Args&&... args) {
        if (level < m_Level) return;
        std::string msg;
        if constexpr (sizeof...(args) > 0) {
            msg = std::vformat(fmt, std::make_format_args(args...));
        } else {
            msg = std::string(fmt);
        }
        WriteLog(level, msg);
    }

    // Convenience methods
    template<typename... Args> void Trace(std::string_view fmt, Args&&... args)    { Log(LogLevel::Trace,    fmt, std::forward<Args>(args)...); }
    template<typename... Args> void Debug(std::string_view fmt, Args&&... args)    { Log(LogLevel::Debug,    fmt, std::forward<Args>(args)...); }
    template<typename... Args> void Info(std::string_view fmt, Args&&... args)     { Log(LogLevel::Info,     fmt, std::forward<Args>(args)...); }
    template<typename... Args> void Warn(std::string_view fmt, Args&&... args)     { Log(LogLevel::Warn,     fmt, std::forward<Args>(args)...); }
    template<typename... Args> void Error(std::string_view fmt, Args&&... args)    { Log(LogLevel::Error,    fmt, std::forward<Args>(args)...); }
    template<typename... Args> void Critical(std::string_view fmt, Args&&... args) { Log(LogLevel::Critical, fmt, std::forward<Args>(args)...); }

    // Store log entries for the ImGui console
    struct LogEntry {
        LogLevel    level;
        std::string timestamp;
        std::string message;
    };
    const std::vector<LogEntry>& GetEntries() const { return m_Entries; }
    void ClearEntries() { std::lock_guard lock(m_Mutex); m_Entries.clear(); }

private:
    Logger() = default;
    void WriteLog(LogLevel level, const std::string& msg);
    static std::string LevelString(LogLevel level);
    static std::string LevelColor(LogLevel level);
    static std::string Timestamp();

    LogLevel                m_Level = LogLevel::Trace;
    std::ofstream           m_File;
    std::mutex              m_Mutex;
    std::vector<LogEntry>   m_Entries;
    static constexpr size_t k_MaxEntries = 4096;
};

// Global convenience macros
#define GE_TRACE(...)    ::GE::Logger::Get().Trace(__VA_ARGS__)
#define GE_DEBUG(...)    ::GE::Logger::Get().Debug(__VA_ARGS__)
#define GE_INFO(...)     ::GE::Logger::Get().Info(__VA_ARGS__)
#define GE_WARN(...)     ::GE::Logger::Get().Warn(__VA_ARGS__)
#define GE_ERROR(...)    ::GE::Logger::Get().Error(__VA_ARGS__)
#define GE_CRITICAL(...) ::GE::Logger::Get().Critical(__VA_ARGS__)

} // namespace GE
