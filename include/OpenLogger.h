#ifndef OPEN_LOGGER_H
#define OPEN_LOGGER_H

#include <string>
#include <string.h>
#include <stdio.h>

#define MAX_MSG_SIZE 1024

enum LOG_TYPE {ERR = 0, WAR, INF, DEB, VER};

class OpenLogger
{
    // Log filename
    char *m_fileName;

    char *m_logMsg;

    int m_logLevel;
    
    int m_enableTimeLog;

    int m_enableThread;

    FILE *m_loggerPtr;

    int logMessage(LOG_TYPE type, const char* msg);

    public:
    OpenLogger(const char *file = NULL, int logLevel = 3);
    OpenLogger(std::string sfile, int logLevel = 3);
    ~OpenLogger();

    int initLogger(std::string fileName);

    int setProperty(std::string property, int value);

    int log(LOG_TYPE type, const char *msg, ...);
    int e(const char *msg, ...);
    int w(const char *msg, ...);
    int i(const char *msg, ...);
    int d(const char *msg, ...);
    int v(const char *msg, ...);
};

#endif // OPEN_LOGGER_H 
