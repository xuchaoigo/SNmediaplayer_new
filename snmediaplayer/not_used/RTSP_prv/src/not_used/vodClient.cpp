#include <RTSPClient_prv.h>
#include <iostream>
#include <pthread.h>
#include <vod_def.h>
#include <string.h>


extern "C" void* QAMplayer_task(void *play_param_in);
//extern "C" void *VodClient_task(void *in_initParam);
#ifdef __cplusplus
extern "C" {
#endif

RTSPClient_prv rtsp_session_server;
RTSPClient_prv rtsp_streaming_server;

void *VodClient_task(void *in_initParam)
{
	pthread_t player_thread;
	PlayParam_t current_play_param;
	VODInitParam_t * initParam;	
	
	initParam = (VODInitParam_t *)in_initParam;


	strcpy(rtsp_session_server.server_infor.ip, "192.168.10.229");
	rtsp_session_server.server_infor.port = 554;

	rtsp_session_server.init();

	//sdv procedure test
	rtsp_session_server.run(1);

	return (void*)0;
	//sleep(100000);

	rtsp_streaming_server.init();
	rtsp_streaming_server.run(1);

	while(!rtsp_session_server.playParamReady())
		usleep(200000);
	rtsp_session_server.getPlayParam(&current_play_param);
#if 0	
	current_play_param.ready = 1;
	current_play_param.frequency = 555000000;
	current_play_param.annex = 1;
	current_play_param.mode = 64;
	current_play_param.symbolRate = 6875000;
	current_play_param.pnID =1;
#endif
	printf("into player:\nready = %d\nfrequency = %d\nannex = %d\nmode = %d\nsymbolRate = %d\npnID = %d\n",\
			current_play_param.ready,current_play_param.frequency,current_play_param.annex,current_play_param.mode,current_play_param.symbolRate,current_play_param.pnID);

	printf("VodClient: thread QAMplayer create\n");
	//	pthread_create(&player_thread, NULL, QAMplayer_task, (void*)&current_play_param);


	return NULL;
}

#if 1
int main(int argc, char* argv[])
{


	pthread_t live_thread;
	VODInitParam_t initParam;
/*
	strcpy(initParam.server_ip, "192.168.8.159");
	initParam.port = 554;                      
	VOD_DEBUG("server ip:%s, port:%d\n",initParam.server_ip, initParam.port);
*/
#if 0
	VOD_DEBUG("setup tunnel.");
	setup_tun();
#endif 	
	pthread_create(&live_thread, NULL, VodClient_task, (void*)&initParam);

	printf("main sleep.\n");
	sleep(100000);

	return 0;

}	
#endif

#ifdef __cplusplus
}
#endif

