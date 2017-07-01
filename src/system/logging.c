#include "board.h"

#define LOG_SIZE_MAX 256

#ifdef ENABLE_LOGGING
static bool    loggingIsEnabled = false;
static int32_t logMask          = 0x0000007F;
#endif

bool setLogStatus(bool enabled) 
{ 
#ifdef ENABLE_LOGGING
    loggingIsEnabled = BoardLogEnable(enabled);
    return true;
#else
    return false;
#endif
}


bool setLogMask(int mask) 
{ 
#ifdef ENABLE_LOGGING
    logMask = mask; 
    return true;
#else
    return false;

#endif
}

#ifdef ENABLE_LOGGING


static const char* logLevelStrings[LOG_LEVEL_MAX] = 
{   "EMERG",
    "ALERT", 
    "CRIT", 
    "ERROR", 
    "WARN", 
    "NOTICE",
    "INFO",
    "DEBUG"
};

void logMessage(ELogLevel level, const char* format, ...)
{
    struct tm *tm;
    struct     timeval tv;
    va_list    args;
    char       msg[LOG_SIZE_MAX];
    size_t     n;

    if((loggingIsEnabled == false ) || (logMask & (1<<level)) == 0)
        return;

    // Format log message
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    n = strftime(msg,sizeof(msg),"%H:%M:%S", tm);
    if(n < LOG_SIZE_MAX)
    { 
        n += snprintf(msg + n,  LOG_SIZE_MAX - n,".%06lu<%s>",tv.tv_usec, logLevelStrings[level]); 
        va_start(args, format);
        if (n <  LOG_SIZE_MAX)
        {
            vsnprintf(msg + n, LOG_SIZE_MAX - n, format, args);
            va_end(args);
        }
    }
    
    // Display log 
    BoardLogMessage(msg);
}

#endif
