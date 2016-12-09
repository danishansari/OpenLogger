#include "OpenLogger.h"

#include <sstream>
#include <unistd.h>
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

    if (m_enableThread)
    { 
        m_enableThread = -1;
        pthread_join(m_loggingThread, NULL);
    }
}

void OpenLogger::initLogger(std::string fileName)
{
    m_logMsg = NULL;
    m_loggerPtr = NULL;
    m_enableTimeLog= 0;
    m_enableThread = 0;

    m_prevMsg = "unknown";
    m_prevMsgCount = 0;

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
    else if (property == "ENABLE_THREAD")
    {
        m_enableThread = 1;

        int tid = pthread_create(&m_loggingThread, NULL, 
                            &OpenLogger::threadHelper, (void *)this);

        if (tid == -1)
        {
            printf("[OpenLogger] :: could not initialize thread!!\n");
            m_enableThread = 0;
        }
    }
    else
    {
        printf("[OpenLogger] :: invalid property!!\n");
        retStatus = 0;
    }

    return retStatus;
}

void OpenLogger::threadLogFunc()
{
    printf("[OpenLogger] :: Threaded Loggin enabled!!\n");

    while (m_enableThread)
    {
        if (m_logMsgVec.size() > 0)
        {
            std::string logMsg = m_logMsgVec.front();
            
            try {
                int ret = fprintf(m_loggerPtr, logMsg.c_str());
            }
            catch(...) {
                printf("[OpenLogger] :: something went wrong!!\n");

                break;
            }

            pthread_mutex_lock(&logMutex);

            m_logMsgVec.pop();
                
            pthread_mutex_unlock(&logMutex);
        }
        else if (m_enableThread == -1)
            m_enableThread = 0;
        else
            usleep(10000);
    }
}

int OpenLogger::logMessage(LOG_TYPE type, const char *msg)
{
    if (!m_loggerPtr)
    {
        printf("[OpenLogger] :: logger not initialized!!\n");
        return -1;
    }
    
    char currMsg[MAX_MSG_SIZE];

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time_t cur_timep = tv.tv_sec;
    struct tm cur_time;
    localtime_r(&cur_timep, &cur_time);

    int dataCount = 0;

    if (m_enableTimeLog)
    {
        dataCount += sprintf(currMsg+dataCount, "%02d/%02d/%04d %02d:%02d:%02d %06ld ",
                cur_time.tm_mday, cur_time.tm_mon+1,
                cur_time.tm_year+1900, cur_time.tm_hour,
                cur_time.tm_min, cur_time.tm_sec, tv.tv_usec);
    }
    
    dataCount += sprintf(currMsg+dataCount, "%s > ", LogType_g[type].c_str());

    dataCount += sprintf(currMsg+dataCount, "%s", msg);

    if (m_prevMsg == std::string(currMsg))
        m_prevMsgCount ++;
    else if (m_prevMsgCount > 0)
    {
        std::stringstream ss;
        ss << m_prevMsgCount;

        m_prevMsgCount = 0;
        
        m_prevMsg = "same message occured " + ss.str() + " times.\n";
    }
   
    if (m_prevMsgCount == 0)
    {
        if (strlen(currMsg) > MAX_MSG_SIZE)
        {
            printf("[OpenLogger] :: Log message too long!!\n");
            dataCount = -1;
        }
        else
        {
            if (m_enableThread)
            {
                while (m_logMsgVec.size() >= 99)
                    usleep(10000);

                pthread_mutex_lock(&logMutex);

                if (m_prevMsg.find("same message") != std::string::npos)
                    m_logMsgVec.push(std::string(currMsg));

                m_logMsgVec.push(std::string(currMsg));

                pthread_mutex_unlock(&logMutex);    
            }
            else
            {
                try {
                    
                    dataCount = fprintf(m_loggerPtr, currMsg);
                }
                catch(...) {
                    printf("[OpenLogger] :: something went wrong!!\n");
                    dataCount = -1;
                    m_enableThread = 0;
                }
            }
        
            m_prevMsg = currMsg;
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
