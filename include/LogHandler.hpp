#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "LoggingLib.hpp"


namespace Logger {
/**
 * Log handler
 */
class LogHandler {
public:

    LogHandler(const LogHandler&) = delete;
    LogHandler & operator=(const LogHandler&) = delete;
    ~LogHandler();

    struct OutputEntity;

    void init();

    // configuration
    enum class Output {FILE, CONSOLE};
    void setOutput(const Output&, const bool&);
    void setLogFile(const std::string&);
    void setLogLevel(const Level&);
    void setFlushFrequency(const unsigned&);
    void setMaxBufferSize(const unsigned&);
    // main method
    void log(const Level&,
             const std::string& msg,
             const std::string& file,
             const std::string& func,
             const unsigned& line);
    // other helpers
    static bool isLevelAvailable(const Level&);
    // return a static global log handler, singleton
    static LogHandler& getHandler();

private:
    LogHandler();
    void startOutputThread();
    void freshCurrentTime();
    void openLogStream() const;
    void outputToConsole(const std::string&) const;
    void outputToFile(const std::string&) const;
    OutputEntity formatOutput(const Level&, const std::string&,
                              const std::string&, const std::string&,
                              const unsigned&) const;

    // running status control
    bool isInited;
    mutable std::mutex logMtx;
    mutable std::mutex outputMtx;
    std::condition_variable logCV;  // condition: logWriteBuffer
    std::condition_variable outputCV;  // condition: isEngineReady
    bool isOutputReady;
    bool isCloseOutput;
    bool isStop;
    std::thread outputThread;

    // log configuration
    const unsigned MaxMsgSize;  // Max single log msg size
    unsigned maxBufferSize;  // max logWriteBuffer
    std::chrono::seconds flushFrequency;  // output engine flush buffer frequency
    std::string logDir;
    std::string logFile;
    mutable std::ofstream logStream;
    std::string currentTime;
    Level logLevel;  // limit log level
    std::map<Output, bool> output;  // limit output

    // log buffer
    // NOTE avoid false sharing
    std::deque<OutputEntity> logReadBuffer;
    std::deque<OutputEntity> logWriteBuffer;
};
}

#endif /* LOGHANDLER_H */