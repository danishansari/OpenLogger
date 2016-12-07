#ifndef OPEN_LOGGER_H
#define OPEN_LOGGER_H

#include <string>
#include <vector>
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

    std::vector<std::string> m_fileNamesVec;

    FILE *m_loggerPtr;

    int logMessage(LOG_TYPE type, const char* msg);

    void initLogger(std::string fileName);

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
