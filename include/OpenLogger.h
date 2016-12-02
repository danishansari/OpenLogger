#ifndef OPEN_LOGGER_H
#define OPEN_LOGGER_H

#include <string.h>

#define MAX_MSG_SIZE 1024

pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

enum LOG_TYPE {ERR: 1, WAR, INFO, DEB, VER};

class OpenLogger
{
    char *m_fileName;

    char *m_logMsg;

    int m_logLevel;

    int m_enableTimeLogging;

    int m_enableThread;

    FILE *m_loggerPtr;

    void initLogger();
    int logMessage(int logLevel);

    public:
        OpenLogger(const char *file = NULL, int logLevel = 3);
        OpenLogger(std::string sfile, int logLevel = 3);
        ~OpenLogger();

        int setProperty(string property, int value);

        int log(LOG_TYPE, const char *msg, ...);
        int e(const char *msg, ...);
        int w(const char *msg, ...);
        int i(const char *msg, ...);
        int d(const char *msg, ...);
        int v(const char *msg, ...);
};

#endif // OPEN_LOGGER_H 
