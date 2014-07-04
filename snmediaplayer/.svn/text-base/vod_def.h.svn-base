#ifndef __VOD_DEF_H__
#define __VOD_DEF_H__
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#define OPEN_VOD_DEBUG 1
#if OPEN_VOD_DEBUG
#define VOD_DEBUG(args...) \
        do{ \
                printf("[DEBUG] %s:%s() ", __FILE__, __FUNCTION__); printf("" args); \
                printf("\n");} \
        while(0)
#else
#define VOD_DEBUG(args...)
#endif
typedef struct PlayParam_s
{
	unsigned char ready;
	unsigned frequency;     //MHz
	unsigned char annex;
	unsigned short mode;   // 1,2,3,4,5  -> 16,32,64,128,256
	unsigned symbolRate;  //MSymbol/s
	unsigned pnID;
	

}PlayParam_t;
typedef enum 
{
	NONE,
	BROWSER,
	VOD_FULLSCREEN


}ServiceStatus_t;
#define MAX_IPADDR_LEN 256
typedef struct VODInitParam_s
{
	char server_ip[MAX_IPADDR_LEN];
	unsigned short port;

}VODInitParam_t;
/*
ServiceStatus_t currentServiceStatus;
int msgqueue_ir = -1;
int msgqueue_browser = -1;
int msgqueue_vod = -1;
int msgqueue_feedback = -1;
*/
int handle_vod_err(const  char *errMsg);
void *VodClient_task(void *initParam);

#ifdef __cplusplus
}
#endif
#endif
