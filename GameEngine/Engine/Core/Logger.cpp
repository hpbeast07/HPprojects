#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace GE {

Logger& Logger::Get() {
    static Logger instance;
    return instance;
}

void Logger::SetLogFile(const std::string& path) {
    std::lock_guard lock(m_Mutex);
    m_File.open(path, std::ios::out | std::ios::app);
}

void Logger::WriteLog(LogLevel level, const std::string& msg) {
    std::lock_guard lock(m_Mutex);

    std::string ts  = Timestamp();
    std::string lvl = LevelString(level);
    std::string col = LevelColor(level);

    // Console output with ANSI color codes
    std::cout << col << "[" << ts << "] [" << lvl << "] " << msg << "\033[0m\n";

    // File output (no color codes)
    if (m_File.is_open()) {
        m_File << "[" << ts << "] [" << lvl << "] " << msg << "\n";
        m_File.flush();
    }

    // Store for ImGui console
    if (m_Entries.size() >= k_MaxEntries)
        m_Entries.erase(m_Entries.begin());
    m_Entries.push_back({ level, ts, msg });
}

std::string Logger::LevelString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:    return "TRACE";
        case LogLevel::Debug:    return "DEBUG";
        case LogLevel::Info:     return "INFO ";
        case LogLevel::Warn:     return "WARN ";
        case LogLevel::Error:    return "ERROR";
        case LogLevel::Critical: return "CRIT ";
        default:                 return "?????";
    }
}

std::string Logger::LevelColor(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:    return "\033[37m";   // white
        case LogLevel::Debug:    return "\033[36m";   // cyan
        case LogLevel::Info:     return "\033[32m";   // green
        case LogLevel::Warn:     return "\033[33m";   // yellow
        case LogLevel::Error:    return "\033[31m";   // red
        case LogLevel::Critical: return "\033[35m";   // magenta
        default:                 return "\033[0m";
    }
}

std::string Logger::Timestamp() {
    auto now  = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms   = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now.time_since_epoch()) % 1000;
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S")
       << '.' << std::setw(3) << std::setfill('0') << ms.count();
    return ss.str();
}

} // namespace GE
