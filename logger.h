#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <mutex>

#define printDataValues(A,B,C,D) Logger::logMessage(A,B,C,D)

enum logSeverity{
    INFO = 0,
    WARNING,
    ERROR,
    DEBUG
};

class Logger
{
    Logger();
    ~Logger();
    static Logger* singletonLogger;
    std::string dirPath;
    std::string currentOS;
    static std::mutex syncMutex;
    std::string getCurrentTime();
    const char* getPlatformName();
    void setDirPath(std::string path);
    std::string getDirPath();
    void setDirBasedOnPlatform();
    std::string getLogFilePath();
    const char* severityToString(logSeverity severity);
    void createProjectLogDir(std::string path);

public:
    static Logger* getInstance();
    void logMessage(logSeverity severity, const std::string functionName , const std::string description,const char* value);
};

#endif // LOGGER_H
