#ifndef OPEN_LOGGER_H
#define OPEN_LOGGER_H

//--    VERSION INFORMATION     --//

#define MAJOR_VERSION 1.0
#define MINOR_VERSION 0.1
#define VERSION_DATE  "11.12.16"

// -----------   +   ------------//

#include <string>
#include <queue>
#include <string.h>
#include <stdio.h>

#define MAX_MSG_SIZE 2048 // bytes
#define MAX_FILE_SIZE 512 // mb's
#define MSG_BUFFER_SIZE 500 // number

enum LOG_TYPE {ERR = 0, WAR, INF, DEB, VER};

class OpenLogger
{
    // Log filename
    char *m_fileName;

    char *m_logMsg;

    int m_logLevel;
    
    int m_enableTimeLog;

    std::string m_prevMsg;
    
    std::string m_prevMsgPrefix;

    int m_prevMsgCount;

    int m_logLongTime;

    int m_enableThread;
    
    int m_fileSizeExceeded;

    pthread_t m_loggingThread;

    std::queue<std::string> m_logMsgVec;

    FILE *m_loggerPtr;

    int logMessage(LOG_TYPE type, const char* msg);

    void initLogger(std::string fileName);

    void threadLogFunc();

    long getFileSize();
    int ifLogFisible(LOG_TYPE type);

    static void * threadHelper(void *logger)
    {
        ((OpenLogger *)logger)->threadLogFunc();

        return NULL;
    }

    public:
        OpenLogger(const char *file = "default.log", int logLevel = 3);
        OpenLogger(std::string sfile, int logLevel = 3);
        ~OpenLogger();

        int setProperty(std::string property, int value=1);

        int log(LOG_TYPE type, const char *msg, ...);
        int e(const char *msg, ...);
        int w(const char *msg, ...);
        int i(const char *msg, ...);
        int d(const char *msg, ...);
        int v(const char *msg, ...);
};

#endif // OPEN_LOGGER_H 
