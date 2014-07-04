#ifndef __CCGWCLIENT_IPC_H__
#define __CCGWCLIENT_IPC_H__
#include "dsp_porting.h"

#ifdef __cplusplus
extern "C"
{
#endif
#define MAX_MSG_WORD_NUM 6
#define MAX_BACKGROUND_STATUS_NUM 1
#define MSGQ_MAX_TRY_TIME 10
typedef struct DSP_IPC_Msg_s
{
	long mtype;
	int32 word[MAX_MSG_WORD_NUM];
	

}DSP_IPC_Msg_t;

typedef enum
{
	APP_INITIAL = 1,
	APP_DOWNLOAD,
	APP_VOD,
	APP_SDV,
	APP_GATEWAY,
	APP_DVB

}AppID_t;

typedef enum
{
	IPQAM_DEFAULT_BANDWIDTH = 1,  // C1
	IPQAM_BANDWIDTH_GIVEN,  // C2
	IPQAM_FREQUENCY_GIVEN,  // C3
	DVB_WITH_IP_TUN  // C4
} Channel_type_t; 

typedef enum
{
	REQUEST_START = 1,  // R1 
	REQUEST_END,  // R2
	REQUEST_NOTICE

} Request_type_t;

typedef enum
{
	CCGW_ADDR_READY = 1,
	CCGW_CLIENT_ONLINE

}Notice_type_t;

typedef enum
{
	NOT_REALTIME = 1,
	SOFT_REALTIME,
	HARD_REALTIME

}QoS_degree_t;

typedef struct Background_status_s
{
	int8 used;
	AppID_t  appID;
	Channel_type_t channel;
	
	


}Background_status_t;

typedef struct Channel_status_s
{
	Channel_type_t busy_channel; //the channel type in use, indicating the foreground status
	Background_status_t background_status[MAX_BACKGROUND_STATUS_NUM];
	int32 bandwidth_request_num;
	int32 background_QoS;
	int32 bandwidth_need;
}Channel_status_t;

/* 初始化进程间通信的工具。实现方式为两个消息队列。一个是从业务进程发消息到CCGW客户端，另一个是从CCGW客户端发消息到业务进程。不同业务的消息用mtype字段区分*/
int32 init_IPC_channel();

/* 释放进程间通信的工具  */
int32 deinit_IPC_channel();

/*从业务进程发消息到CCGW客户端 */
int32 send_msg_to_CCGWclient( const DSP_IPC_Msg_t  *command);

/* 从CCGW客户端发消息到业务进程 */
int32 CCGWclient_send_msg( const DSP_IPC_Msg_t  *command);

/* CCGW客户端接收业务进程的消息 */
int32 CCGWclient_recv_msg( DSP_IPC_Msg_t   *command, uint32 timeout_ms);

/* 业务进程接收CCGW客户端的消息 */
int32 recv_msg_from_CCGWclient(AppID_t appID, DSP_IPC_Msg_t  *command, uint32 timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
