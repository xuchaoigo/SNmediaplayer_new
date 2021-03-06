#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "vod_def.h"

//extern void * VodClient_task(void *initParam);

int handle_vod_err(const char *errMsg)
{
	VOD_DEBUG("%s", errMsg);
	return 0;

}
int main(int argc, char* argv[])
{

	/*exec browser and manager thread*/

	pthread_t live_thread;
	VODInitParam_t initParam;
	int ret = -1;
	
	strcpy(initParam.server_ip, "172.27.60.15");
	initParam.port = 554;                      
	VOD_DEBUG("server ip:%s, port:%d\n",initParam.server_ip, initParam.port);
#if 0
	VOD_DEBUG("setup tunnel.");
        setup_tun();
#endif 	
	ret = pthread_create(&live_thread, NULL, VodClient_task, (void*)&initParam);
	if(ret)
	{
	VOD_DEBUG("Can't create thread VodClient_task\n");
	return -1;
	}
	else 
		VOD_DEBUG("thread VodClient_task create\n");

	printf("main sleep.\n");
	//sleep(100000);
	
	return 0;

}	
