#include "logger.h"

Logger* Logger::singletonLogger = nullptr;
std::mutex Logger::syncMutex;

Logger::Logger()
{
    this->currentOS = getPlatformName();
    this->setDirBasedOnPlatform();
    this->createProjectLogDir(this->dirPath);
}

Logger* Logger::getInstance()
{
    syncMutex.lock();
    if(!singletonLogger)
    {
        singletonLogger = new Logger;
    }
    syncMutex.unlock();
    return singletonLogger;
}

void Logger::setDirPath(std::string path){
    this->dirPath = path;
}

std::string Logger::getDirPath(){
    return dirPath;
}

const char* Logger::severityToString(logSeverity severity){
    switch (severity) {
        case 0:
                return "INFO";
        case 1:
                return "HIGH";
        case 2:
                return "MEDIUM";
        case 3:
                return "LOW";
    }
    return  "NULL";
}

std::string Logger::getLogFilePath(){
    std::string path = "";
    if(currentOS == "windows"){
        path = this->dirPath + "\\ThinkLogs.log";
    }
    else{
        path = this->dirPath + "/ThinkLogs.log";
    }
    return path;
}

std::string Logger::getCurrentTime()
{
    time_t currentTime = time(nullptr);
    tm *localTime = localtime(&currentTime);

    std::string date = std::to_string(localTime->tm_mday);
    std::string month = std::to_string(1 + localTime->tm_mon);
    std::string year = std::to_string(1900 + localTime->tm_year);
    std::string hour = std::to_string(localTime->tm_hour);
    std::string min = std::to_string(localTime->tm_min);
    std::string sec = std::to_string(localTime->tm_sec);

    std::string curTime = date + "/" + month + "/" + year + " " + hour + ":" + min + ":" + sec;
    return curTime;
}

const char* Logger::getPlatformName(){

    #if defined (_WIN32) || defined (_WIN64) || (defined (__CYGWIN__) && !defined (_WIN32))
        #define PLATFORM_NAME "windows"

    #elif defined (__APPLE__) && defined(__MACH__)
        #include <TargetConditionals.h>
        #if TARGET_OS_MAC == 1
            #define PLATFORM_NAME "OSX"
        #else
            #define PLATFORM_NAME "ios"
        #endif

    #elif defined (__linux__)
        #define PLATFORM_NAME "linux"

    #else
        #define PLATFORM_NAME "NULL"

#endif

    return PLATFORM_NAME;
}


void Logger::createProjectLogDir(std::string path){
    struct stat dirCheck;

    if(stat(path.c_str(),&dirCheck) == 0){}
    else
    {
#if defined (__APPLE__) || defined (__linux__)
        if (mkdir(path.c_str(), 0777) == -1){}
#elif defined (_WIN32)
        if (mkdir(path.c_str()) == -1){}
#endif
     }
}

void Logger::setDirBasedOnPlatform(){
    if (currentOS == "windows"){
        char* appdata = getenv("APPDATA");
        strcat(appdata, "\\Think2wice");
        this->dirPath = appdata;
    }
    else if (currentOS == "linux"){
        this->setDirPath("/tmp/Think2wice");
    }
    else if (currentOS == "OSX"){
        char* path = std::getenv("HOME");
        strcat(path,"/Library/Logs/Think2wice");
        this->setDirPath(path);
    }
    else{
        this->dirPath = "";
    }
}

void Logger::logMessage(logSeverity severity, const std::string functionName , const std::string description,const char* value){

    if(value == nullptr){
        value = "";
    }
    syncMutex.lock();
    std::ofstream out;
    out.open(this->getLogFilePath() , std::ios::out | std::ios::app);

    if(!out)
    {
        out << this->getCurrentTime() <<" : " << "  pid:"<<getpid() << "  " << this->severityToString(severity)<< "   " <<functionName<<"   "<<"File creation failed";
    }
    else
    {
        out << this->getCurrentTime() <<" : " << "  pid:"<<getpid() << "  " << this->severityToString(severity)<< "   " <<functionName<<"   "<<description<<"   "<< value;
        out << std::endl;
    }
    syncMutex.unlock();
}

Logger::~Logger(){
    delete singletonLogger;
    singletonLogger = nullptr;
}
