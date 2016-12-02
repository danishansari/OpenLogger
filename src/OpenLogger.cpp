#include "OpenLogger.h"

OpenLogger::OpenLogger(const char *file, int logLevel)
{
  m_fileName = (char)file; 
  m_logLevel = logLevel;

  initLogger();
}

OpenLogger::OpenLogger(string sFile, int logLevel)
{
  m_fileName = (char)sFilefile.c_str(); 
  m_logLevel = logLevel;

  initLogger();
}

void OpenLogger::initLogger()
{
  m_logMsg = NULL;
  m_loggerPtr = NULL;
  m_enableTimeLogging = 0;

  m_loggerPtr = open(m_fileName, "a");
}

int OpenLogger::setProperty(string property, int value)
{
  int retStatus = 1;

  if (property == "LOG_LEVEL")
    m_logLevel = value;
  else if (property == "LOG_TIME")
    m_enableTimeLogging = 1;
  else if (property == "LOG_THREADED")
    m_enableThreading = 1;
  else
    retStatus = 0;

  return retStatus;
}

int OpenLogger::logMessage(const char *format, ...)
{
  char currMsg[MAX_MSG_SIZE];

  va_list va;
  va_start(va, format);
  vsprintf(currMsg, format, va);
  va_end(va);

  time_t cur_timep = tv.tv_sec;
  struct tm cur_time;
  localtime_r(&cur_timep, &cur_time);

  if (m_logTimeAlso)
  {
    ret += sprintf(prefixString+ret, "%02d/%02d/%04d %02d:%02d:%02d %06ld ",
                   cur_time.tm_mday, cur_time.tm_mon+1,
                   cur_time.tm_year+1900, cur_time.tm_hour,
                   cur_time.tm_min, cur_time.tm_sec, tv.tv_usec);
  }

}

int OpenLogger::log(LOG_TYPE, const char *msg, ...)
{
  
}
