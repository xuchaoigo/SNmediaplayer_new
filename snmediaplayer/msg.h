#ifndef _MSG_H
#define _MSG_H

#include <sys/msg.h>

//消息队列 key值
#define MANAGER_QUEUE_KEY 5131L
#define JS_QUEUE_KEY 5132L
#define PLAYER_QUEUE_KEY 5133L
#define DOWNLOAD_QUEUE_KEY 5134L
#define COPY_QUEUE_KEY 5135L
#define PPPOE_QUEUE_KEY 5136L
#define MANAGER_DIALOG_QUEUE_KEY 5137L
#define PHONE_REMOTE_KEY_QUEUE_KEY 5138L

#define MAX_MESSAGE_LEN	2048


//js控制下载相关
#define START_DOWNLOAD_FILE 600
#define GET_DOWNLOAD_FILESTATUS 601
#define PAUSE_DOWNLOAD_FILE 602
#define CANCEL_DOWNLOAD_FILE 603
#define GET_DOWNLOAD_FILELIST 604 


#define PLAY_DOWNLOAD_FILE 605
#define PLAY_DOWNLOAD_TVSHOW_FILE 606
#define PLAY_DOWNLOAD_WHOLE_TVSHOW_FILE 607
#define PLAY_HD_FILE  608
#define PLAY_HD_FILELIST 609
#define PLAY_FILE_OVER 610
#define PLAY_MUSIC_FILELIST 611
#define PLAY_DOWNLOAD_SPECIAL_FILE 612
#define PLAY_DOWNLOAD_WHOLE_SPECIAL_FILE 613
#define PLAY_PLAYING_WHILE_DOWNLOADING_FILE 614		//zhanghs video
#define PLAY_INTERNET_VIDEO 615						//zhanghs adds
#define STOP_INTERNET_VIDEO 616						// zhanghs adds

//manager与js通信
#define JS_SND_DIALOGRESULT 300
#define JS_SHOW_INFO 301
#define JS_DRAW_DIALOG 302//默认是取消
#define JS_SET_INFO 303
#define JS_GO_BACKWARD 304
#define JS_GO_MAINMENU 305
#define INFO_SERVICE_EXIT 306
#define JS_MAIN_VIDEO 307
#define JS_SECOND_VIDEO 308
#define JS_LASTING_SHOW_INFO 309
#define JS_UPDATE_SOFTWARE 310
#define JS_HD_FORMAT 311
#define JS_UPDATE_SOFTWARE_NET 312
#define JS_DRAW_DIALOG_NOANSWER 313
#define JS_ADSL_START  314
#define JS_ADSL_STOP 315
#define JS_REFRESH_UI 316
#define JS_LASTING_SHOWINFO_DISAPPEAR 317

//#define INFO_SERVICE_SWITCHMODE 804

#define JS_WIFI_START 318
#define BACK_DHCP 319

#define JS_REFRESH_USB_UI 320 //liucm

#define JS_DRAW_DIALOG_CONFIRM 321 //默认是确定
#define TEST_SWITCH_SERVICE 322

//播放器相关消息
#define CHANGE_PLAYER_WINDOW 400
#define CHANGE_PLAYER_URL 401
#define PLAYER_INBACKGROUND 402 //后台播放模式
#define PLAYER_QUIT 403 //
#define PLAYER_SEEKTIME 404 //搜索时间
#define PLAYER_STOP 405
#define PLAYER_DRAW_DIALOG 406
#define PLAYER_SND_DIALOGRESULT 407
#define PLAYER_INFORGROUND 408 //后台播放模式
#define PLAYER_CHANGE_BACKGROUNDIMG 409
#define DSP_MUSIC_STOP 410
#define PLAYER_BACKGROUNDMODE 411 //
#define PLAYER_BGStart 412//开启背景视频
#define PLAYER_BGStop 413//关闭背景视频




#define MAX_NAME_LENGTH SN_MAX_FILENAME_LENGTH

typedef struct msgbuff{
	long mtype;
	char mdata[MAX_MESSAGE_LEN];
}Message;



#ifdef __cplusplus
extern "C"{
#endif

void Init_Msg();

void Fini_Msg();

int Sendto_Manager_Msg(int cmd,char *data);

int Sendto_Manager_Msg_Dialog(int cmd, char * data);

int Sendto_Manager_Msg_STRUCT(int cmd,char *data, int datalen);

int Sendto_JS_Msg(int cmd,char *data, int datalen);

int Sendto_Download_Msg(int cmd,char *data, int datalen);

int Sendto_Player_Msg(int cmd,char *data, int datalen);

int Manager_Rcv_Msg(int *cmd, char *data);

int JS_Rcv_Msg(int *cmd, char *data);

int Download_Rcv_Msg(int *cmd, char *data);

int Player_Rcv_Msg(int *cmd, char *data);

int Manager_Rcv_Phone_Remote_Control_Msg(int *cmd);

int Sendto_Manager_Phone_Remote_Control_Msg(int cmd);
#ifdef __cplusplus
}
#endif


#endif
