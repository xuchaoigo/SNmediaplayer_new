#include <iostream>
#include "vod_def.h"
#include "RTSPClient_prv.h"


int CreateMsgQueue(const char * name)  /*xuc:input name for READ! const.*/
{

	return 0;
}



ServiceStatus_t currentServiceStatus;
int msgqueue_ir = -1;
int msgqueue_browser = -1;
int msgqueue_vod = -1;
int msgqueue_feedback = -1;

void init_msgqueues()
{
	msgqueue_ir = CreateMsgQueue("ir");
	msgqueue_browser = CreateMsgQueue("browser");
	msgqueue_vod = CreateMsgQueue("vod");
	msgqueue_feedback= CreateMsgQueue("feedback");


}
int recv_ir_key()
{

	return 0;
}


int send_key_to_service()
{

	return 0;
}
void manager_task(void *param)
{
	currentServiceStatus = NONE;
	init_msgqueues();
	
}
