#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
/*调试信息*/
#define DEBUG_ON
#define _DEBUG_IF_ 1
#define DEBUG_LOG_ON

#ifdef DEBUG_LOG_ON
#define OPENLOG openlog("DEBUG_LOG", LOG_CONS | LOG_PID, 0)
#define DEBUGLOG(format, ...) do {if (_DEBUG_IF_) { syslog(LOG_INFO, "%s(%d)-%s: "format ,__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);} }while (0)
#else
#define DEBUGLOG(format, ...)
#define OPENLOG
#endif /* DEBUG_LOG_ON */

#ifdef DEBUG_ON
#define DEBUGMSG(format, ...) do {if (_DEBUG_IF_) { fprintf(stderr, "%s(%d)-%s: ",__FILE__, __LINE__, __FUNCTION__); fprintf(stderr, format, ##__VA_ARGS__); fprintf(stderr,"\n");} }while (0)
#else
#define DEBUGMSG(x)
#endif
#endif /*__DEBUG_H__*/
