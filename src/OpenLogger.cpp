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
{ 
    if (m_loggerPtr)
    {
        fclose(m_loggerPtr);
        m_loggerPtr = NULL;
    }
}

void OpenLogger::initLogger(std::string fileName)
{
    m_logMsg = NULL;
    m_loggerPtr = NULL;
    m_enableTimeLog= 0;

    m_fileName = (char *)fileName.c_str();

    if (fileName == "default.log")
        m_loggerPtr = fopen(m_fileName, "w");
    else
        m_loggerPtr = fopen(m_fileName, "a+");

    if (!m_loggerPtr)
        printf("[OpenLogger] :: error in opening logger\n");
}

int OpenLogger::setProperty(std::string property, int value)
{
    int retStatus = 1;

    if (property == "LOG_LEVEL")
        m_logLevel = value;
    else if (property == "LOG_TIME")
        m_enableTimeLog = 1;
    else
    {
        printf("[OpenLogger] :: invalid property!!\n");
        retStatus = 0;
    }

    return retStatus;
}

int OpenLogger::logMessage(LOG_TYPE type, const char *msg)
{
    if (!m_loggerPtr)
    {
        printf("[OpenLogger] :: logger not initialized!!\n");
        return -1;
    }

    char preMsg[MAX_MSG_SIZE];

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time_t cur_timep = tv.tv_sec;
    struct tm cur_time;
    localtime_r(&cur_timep, &cur_time);

    int dataCount = 0;

    if (m_enableTimeLog)
    {
        dataCount += sprintf(preMsg+dataCount, "%02d/%02d/%04d %02d:%02d:%02d %06ld ",
                cur_time.tm_mday, cur_time.tm_mon+1,
                cur_time.tm_year+1900, cur_time.tm_hour,
                cur_time.tm_min, cur_time.tm_sec, tv.tv_usec);
    }
    
    dataCount += sprintf(preMsg+dataCount, "%s > ", LogType_g[type].c_str());

    dataCount += sprintf(preMsg+dataCount, "%s", msg);

    if (strlen(preMsg) > MAX_MSG_SIZE)
    {
        printf("[OpenLogger] :: Log message too long!!");
        dataCount = -1;
    }
    else
    {
        try {
            //dataCount = fwrite(preMsg, 1, dataCount, m_loggerPtr);
            dataCount = fprintf(m_loggerPtr, preMsg);
        }
        catch(...) {
            printf("[OpenLogger] :: something went wrong!!");
            dataCount = -1;
        }
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
