#include "OpenLogger.h"

#include <sstream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
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
    if (m_enableThread)
    { 
        m_enableThread = -1;
        pthread_join(m_loggingThread, NULL);
    }

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
    m_enableThread = 0;
    m_logLevel = 2;

    m_logLongTime = 1;

    m_prevMsg = "unknown";
    m_prevMsgPrefix = "unknown";
    m_prevMsgCount = 0;

    m_fileName = new char[1024];
    memset((void*)m_fileName, 0, sizeof(m_fileName));

    if (fileName == "default.log")
    {
        fileName = "./log/"+fileName;
        m_loggerPtr = fopen(fileName.c_str(), "w");
    }
    else
        m_loggerPtr = fopen(fileName.c_str(), "a+");

    sprintf(m_fileName, "%s", fileName.c_str());

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
        int tid = pthread_create(&m_loggingThread, NULL, 
                &OpenLogger::threadHelper, (void *)this);

        if (tid == -1)
        {
            printf("[OpenLogger] :: could not initialize thread!!\n");
            m_enableThread = 0;
        }
        else
            m_enableThread = 1;
    }
    else
    {
        printf("[OpenLogger] :: invalid property: %s\n", property.c_str());
        retStatus = 0;
    }

    return retStatus;
}

void OpenLogger::threadLogFunc()
{
    printf("[OpenLogger] :: Threaded Logging enabled..\n");

    int writeFailed = 0;

    while (m_enableThread)
    {
        if (m_logMsgVec.size() > 0)
        {
            if (writeFailed < 3)
            {
                pthread_mutex_lock(&logMutex);

                int logBuffer = (m_logMsgVec.size()*100.0)/MSG_BUFFER_SIZE; 

                for (int i = 60; (i < logBuffer && i < 65) || i == 60; i++)
                {
                    std::string logMsg = m_logMsgVec.front();

                    try 
                    {
                        if (fprintf(m_loggerPtr, "%s", logMsg.c_str()) <= 0)
                        {
                            printf("[OpenLogger] :: could not write log!!\n");
                            writeFailed++;
                        }
                        else
                        {
                            writeFailed = 0;

                            m_logMsgVec.pop();
                        }

                        fflush(m_loggerPtr);
                    }
                    catch(...) {
                        printf("[OpenLogger] :: Ops, something went wrong!!\n");

                        writeFailed++;
                    }
                }
                
                pthread_mutex_unlock(&logMutex);
            }
            else
            {
                m_enableThread = 0;
            }
        }
        else if (m_enableThread == -1)
            m_enableThread = 0;

        if (m_logMsgVec.size() < MSG_BUFFER_SIZE/5)
            usleep(10000);
        else
            usleep(1000);
    }

    printf("[OpenLogger] :: Logging Thread exited!!\n");
}

int OpenLogger::logMessage(LOG_TYPE type, const char *msg)
{
    struct timeval ts, te;
    gettimeofday(&ts, NULL);
    double tdd = 0.0;

    if (ifLogFisible(type) < 0)
        return -1;

    char currMsg[MAX_MSG_SIZE];
    memset(currMsg, 0, (size_t)MAX_MSG_SIZE);

    struct timeval tv;
    gettimeofday(&tv, NULL);

    time_t cur_timep = tv.tv_sec;
    struct tm cur_time;
    localtime_r(&cur_timep, &cur_time);

    int dataCount = 0;

    if (m_enableTimeLog)
    {
        dataCount += sprintf(currMsg, "%02d/%02d/%04d %02d:%02d:%02d %06ld ",
                cur_time.tm_mday, cur_time.tm_mon+1,
                cur_time.tm_year+1900, cur_time.tm_hour,
                cur_time.tm_min, cur_time.tm_sec, tv.tv_usec);
    }

    dataCount += sprintf(currMsg+dataCount, "%s > ", LogType_g[type].c_str());

    if (m_prevMsg == std::string(msg))
        m_prevMsgCount ++;
    else if (m_prevMsgCount > 1)
    {
        std::stringstream ss;
        ss << (m_prevMsgCount-1);

        m_prevMsgCount = 0;

        m_prevMsg = m_prevMsgPrefix;
        m_prevMsg += "same message occured " + ss.str() + " more times.\n";
    }

    m_prevMsgPrefix = currMsg;

    if (m_prevMsgCount < 1)
    {
        dataCount += sprintf(currMsg+dataCount, "%s", msg);

        if (strlen(currMsg) > MAX_MSG_SIZE)
        {
            printf("[OpenLogger] :: Log message too long!!\n");
            dataCount = -1;
        }
        else
        {
            double fsize = getFileSize()/(1024.0*1024.0);
            if (fsize >= MAX_FILE_SIZE)
            {
                printf ("[OpenLogger] Exceeded File size = %.2lfmbs (max = %d)\n", fsize, MAX_FILE_SIZE);
                dataCount = sprintf(currMsg, "%s File size limit exeeded(%.2lf)\
                            \n%s Stoping logging now..\n", m_prevMsgPrefix.c_str(), 
                            fsize, m_prevMsgPrefix.c_str());
                m_fileSizeExceeded = 1;
            }

            if (m_enableThread)
            {
                int waitTimeOut = -10;
                while (m_logMsgVec.size() >= MSG_BUFFER_SIZE+2 && ++waitTimeOut)
                    usleep(10000);
                
                if (waitTimeOut == 0)
                {
                    printf ("[OpenLogger] :: Logger is not responding, skipping log!!\n");
                    return -1; // Logger is taking longer than accepted
                }

                pthread_mutex_lock(&logMutex);
                
                if (m_prevMsg.find("same message") != std::string::npos)
                    m_logMsgVec.push(m_prevMsg);

                m_logMsgVec.push(std::string(currMsg));

                pthread_mutex_unlock(&logMutex);    
            }
            else
            {
                try {
                    if (m_logMsgVec.size() == 0)
                    {
                        if (m_prevMsg.find("same message") != std::string::npos)
                            dataCount = fprintf(m_loggerPtr, "%s", m_prevMsg.c_str());

                        dataCount = fprintf(m_loggerPtr, "%s", currMsg);
                        fflush(m_loggerPtr);
                    }
                    else
                    {
                        if (m_prevMsg.find("same message") != std::string::npos)
                            m_logMsgVec.push(m_prevMsg);

                        m_logMsgVec.push(std::string(currMsg));

                        for (int i = 0; i < 3 && m_logMsgVec.size() > 0; i++)
                        {
                            std::string lastMsg =  m_logMsgVec.front();

                            dataCount = fprintf(m_loggerPtr, "%s", lastMsg.c_str());
                            fflush(m_loggerPtr);
                            if (dataCount > 0)
                                m_logMsgVec.pop();
                        }
                    }
                }
                catch(...) {
                    printf("[OpenLogger] :: Ops, something went wrong!!\n");
                    dataCount = -1;
                }
            } 
        }
    }

    m_prevMsg = msg;

    gettimeofday(&te, NULL);

    double td = (te.tv_sec - ts.tv_sec)*1000.0 + (te.tv_usec-ts.tv_usec)/1000.0;

    if (m_logLongTime && td > 20.0)
    {
        fprintf(m_loggerPtr, "%s Logging is taking so long(%.2lf ms)\n", 
                m_prevMsgPrefix.c_str(), td);

        fflush(m_loggerPtr);

        if (!m_enableThread)
            printf("[OpenLogger] :: Use threading by setting ENABLE_THREAD flag.\n");
    }

    return dataCount;
}

int OpenLogger::ifLogFisible(LOG_TYPE type)
{
    if (!m_loggerPtr || type > m_logLevel || m_fileSizeExceeded)
    {
        if (!m_loggerPtr)
            printf("[OpenLogger] :: logger not initialized!!\n");
        else if (m_fileSizeExceeded)
        {
            m_fileSizeExceeded ++;
            if (m_fileSizeExceeded >= 10000)
                m_fileSizeExceeded = 1;

            if (getFileSize() >= MAX_FILE_SIZE)
            {
                if (m_fileSizeExceeded < 5)
                    printf("[OpenLogger] :: File size exceeded, logging stoped..\n");
            }
            else
                m_fileSizeExceeded = 0;
        }

        usleep(10000);
        return -1;
    }

    return 0;
}

long OpenLogger::getFileSize()
{
    struct stat stat_buf;
    memset(&stat_buf, '\0', sizeof(stat_buf));

    int rc = stat(m_fileName, &stat_buf);

    return rc == 0 ? stat_buf.st_size : -1;
}

int OpenLogger::log(LOG_TYPE type, const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    return logMessage(type, currMsg);
}

int OpenLogger::e(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    return logMessage(ERR, currMsg);
}

int OpenLogger::w(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    return logMessage(WAR, currMsg);
}

int OpenLogger::i(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    return logMessage(INF, currMsg);
}

int OpenLogger::d(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    return logMessage(DEB, currMsg);
}

int OpenLogger::v(const char *format, ...)
{
    char currMsg[MAX_MSG_SIZE];

    va_list va;
    va_start(va, format);
    vsprintf(currMsg, format, va);
    va_end(va);

    return logMessage(VER, currMsg);
}
