
#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <syslog.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdio.h>
#define DEBUG_ON
#define LOG_LVL 0
//#define USE_SYS_LOG

#define CONMSG(stdxx, format, ...)do {fprintf(stdxx, "%s(%d)-%s: ",__FILE__, __LINE__, __FUNCTION__); fprintf(stdxx, format, ##__VA_ARGS__); fprintf(stdxx,"\n");}while (0)
#define LOGMSG(LOG_LEVEL, format, ...)  do {syslog(LOG_LEVEL, "%s(%d)-%s: "format ,__FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); }while (0)
#ifdef DEBUG_ON

	#ifdef USE_SYS_LOG
		#define OPENLOG() openlog("DEBUG_LOG", LOG_CONS | LOG_PID, 0)
		#define MSG_ERR(format, ...)		do{ if(LOG_LVL <= 2){ LOGMSG(LOG_ERR,   format, ##__VA_ARGS__); }} while(0)
		#define MSG_IFO(format, ...)		do{ if(LOG_LVL <= 1){ LOGMSG(LOG_INFO,  format, ##__VA_ARGS__); }} while(0)
		#define MSG_DEBUG(format, ...)	do{ if(LOG_LVL <= 0){ LOGMSG(LOG_DEBUG, format, ##__VA_ARGS__); }} while(0)
	#else
		#define MSG_ERR(format, ...)		do{ if(LOG_LVL<=2){CONMSG(stderr, format, ##__VA_ARGS__);}}while(0)
		#define MSG_IFO(format, ...)		do{ if(LOG_LVL<=1){CONMSG(stdout, format, ##__VA_ARGS__);}}while(0)
		#define MSG_DEBUG(format, ...)	do{ if(LOG_LVL<=0){CONMSG(stdout, format, ##__VA_ARGS__);}}while(0)
	#endif
#else
	#define OPENLOG()
	#define MSG_ERR(format, ...)
	#define MSG_IFO(format, ...)
	#define MSG_DEBUG(format, ...)
#endif

#endif /*__DEBUG_H__*/
