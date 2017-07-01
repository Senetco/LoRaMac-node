#ifndef __LOGGING_H__
#define __LOGGING_H__

#include "utilities.h"

typedef enum 
{
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ALERT,
    LOG_LEVEL_CRITICAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX
} ELogLevel;

extern bool     setLogStatus(bool enabled); 
extern bool     setLogMask(int32_t mask);
extern void     logMessage(ELogLevel level, const char* format, ...);


/* Logging MACROS */
#ifdef ENABLE_LOGGING
#define LOG(level, format, args...) logMessage(level, format, ##args)
#define FATAL_LOG(format, args...) LOG(LOG_LEVEL_FATAL, format, ##args)
#define ERROR_LOG(format, args...) LOG(LOG_LEVEL_ERROR, format, ##args)
#define WARN_LOG(format, args...)  LOG(LOG_LEVEL_WARNING, format, ##args) 
#define NOTICE_LOG(format, args...) LOG(LOG_LEVEL_NOTICE, format, ##args) 
#define INFO_LOG(format, args...)  LOG(LOG_LEVEL_INFO, format, ##args)
#define DEBUG_LOG(format, args...) LOG(LOG_LEVEL_DEBUG, format, ##args) 
#define BYTES_TO_STRING(bytes, nbytes, string, slen)\
        do { byteArrayToByteString(bytes, nbytes, string, slen); }while(0)

#else // Compile out logging

#define LOG(level, format, args...) 
#define FATAL_LOG(format, args...) 
#define ERROR_LOG(format, args...) 
#define WARN_LOG(format, args...)  
#define NOTICE_LOG(format, args...) 
#define INFO_LOG(format, args...)  
#define DEBUG_LOG(format, args...) 
#define BYTES_TO_STRING(bytes, nbytes, string, slen)


#endif

#define MACEVENT(header, format,args...) NOTICE_LOG("%-12s:"format,header, ##args)


#endif
