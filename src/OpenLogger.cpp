#include "OpenLogger.h"

#include <sys/time.h>
#include <cstdarg>

std::string LogType_g[] = {"ERR", "WAR", "INF", "DEB", "VER"};

pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

OpenLogger::OpenLogger(const char *file, int logLevel)
{
    m_logLevel = logLevel;

    initLogger(std::string(file));
}

OpenLogger::OpenLogger(std::string sFile, int logLevel)
{
    m_logLevel = logLevel;

    initLogger(sFile);
}

OpenLogger::~OpenLogger()
{ }

void OpenLogger::initLogger(std::string fileName)
{
    m_logMsg = NULL;
    m_loggerPtr = NULL;
    m_enableTimeLog= 0;

    m_fileName = (char *)fileName.c_str();
    m_loggerPtr = fopen(m_fileName, "a+");
}

int OpenLogger::setProperty(std::string property, int value)
{
    int retStatus = 1;

    if (property == "LOG_LEVEL")
        m_logLevel = value;
    else if (property == "LOG_TIME")
        m_enableTimeLog = 1;
    else if (property == "LOG_THREADED")
        m_enableThread = 1;
    else
        retStatus = 0;

    return retStatus;
}

int OpenLogger::logMessage(LOG_TYPE type, const char *msg)
{
    char preMsg[MAX_MSG_SIZE];

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time_t cur_timep = tv.tv_sec;
    struct tm cur_time;
    localtime_r(&cur_timep, &cur_time);

    int dataCount = 0;

    //dataCount += sprintf(preMsg, "< ");
    if (m_enableTimeLog)
    {
        dataCount += sprintf(preMsg+dataCount, "%02d/%02d/%04d %02d:%02d:%02d %06ld ",
                cur_time.tm_mday, cur_time.tm_mon+1,
                cur_time.tm_year+1900, cur_time.tm_hour,
                cur_time.tm_min, cur_time.tm_sec, tv.tv_usec);
    }
    
    dataCount += sprintf(preMsg+dataCount, "%s > ", LogType_g[type].c_str());

    dataCount += sprintf(preMsg+dataCount, "%s", msg);

    if (m_loggerPtr)
    {
        dataCount = fwrite(preMsg, 1, dataCount, m_loggerPtr);
    }

    return dataCount;
}

int OpenLogger::log(LOG_TYPE type, const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    logMessage(type, currMsg);
}

int OpenLogger::e(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    logMessage(ERR, currMsg);
}

int OpenLogger::w(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    logMessage(WAR, currMsg);
}

int OpenLogger::i(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    logMessage(INF, currMsg);
}

int OpenLogger::d(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    logMessage(DEB, currMsg);
}

int OpenLogger::v(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    logMessage(VER, currMsg);
}
