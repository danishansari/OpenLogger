#ifndef OPEN_LOGGER_H
#define OPEN_LOGGER_H

enum LOG_TYPE {ERR: 1, WAR, INFO, DEB, VER};

class Logger
{
    char *m_fileName;
    int m_logLevel;
    char *m_logMsg;

    FILE *m_loggerPtr;

    void initLogger();
    int logMessage(int logLevel);

    public:
        Logger(const char *file = "", int logLevel = 3);
        ~Logger();

        void setLogLevel (int level);
        void setLogTime (bool logTime);

        void log(LOG_TYPE, const char *msg, ...);
        void e(const char *msg, ...);
        void w(const char *msg, ...);
        void i(const char *msg, ...);
        void d(const char *msg, ...);
        void v(const char *msg, ...);
};

#endif // OPEN_LOGGER_H 
