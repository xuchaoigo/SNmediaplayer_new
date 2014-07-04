#ifndef COMMON_PLAYER_H
#define COMMON_PLAYER_H

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include "tsplayer.h"
#include "parse_svalue.h"
#include "DSPRemoteKey.h"
#include "common_setting.h"
#include "loginfo.h"


#define DSP_CMD_UNKNOWN 0

#define DSP_CMD_PLAYING 1
#define DSP_CMD_PAUSED  2
#define DSP_CMD_STOPPED 3

#define DSP_CMD_PLAY  4
#define DSP_CMD_PAUSE 5
#define DSP_CMD_STOP  6
#define DSP_CMD_STOP_SEEK_ZERO  7

#define DSP_CMD_QUIT  8

#define DSP_CMD_PLAYING_TRICKMODE 9   // no audio

#define DSP_CMD_CHANNEL_CHANGE	10
#define DSP_CMD_PAT_INFO		11
#define DSP_CMD_PMT_CHANGE		12
#define DSP_CMD_AUDIO_STREAM_CHANGE	13
#define DSP_CMD_VIDEO_STREAM_CHANGE	14

#define DSP_CMD_SEEK                 15
#define DSP_CMD_DUALMODE_CHANGE      16

#define DSP_CMD_ROTATE_PICTURE       17

#define DSP_CMD_MANU_QUIT_OK		18

#define DSP_CMD_NONE                 19
#define DSP_CMD_SPEED                20
#define DSP_CMD_SLOW_FWD             21
#define DSP_CMD_FAST_FWD             22
#define DSP_CMD_IFWD                 23
#define DSP_CMD_IRWD                 24
#define DSP_CMD_NEXTPIC              25

#define DSP_CMD_DECODER_CHANGE       26

#define DSP_CMD_NERO_NEXT_CHAPTER 27
#define DSP_CMD_NERO_PREV_CHAPTER 28
#define DSP_CMD_NERO_SWITCH_SUBTITLE 29

#define DSP_CMD_REWIND 30

#define DSP_CMD_CHANNEL_SWITCH 31
#define DSP_CMD_STEP_FW       32     
#define DSP_CMD_STEP_BW       33     

#define DSP_CMD_SKIPPING 34

#define DSP_CMD_SKIPTO 35

#define DSP_CMD_PREBUFFERING 38
#define DSP_CMD_CHANGE_WINDOW 39

#define DSP_CMD_NO_TRICKMODE		0
#define DSP_CMD_TRICKMODE_FAST_FWD	1
#define DSP_CMD_TRICKMODE_FAST_RWD	2
#define DSP_CMD_TRICKMODE_SLOW_FWD	3
#define DSP_CMD_TRICKMODE_SLOW_RWD	4
#define DSP_CMD_TRICKMODE_NEXT_PIC	5
#define DSP_CMD_TRICKMODE_FWD_IFRAME	6
#define DSP_CMD_TRICKMODE_RWD_IFRAME	7

#define RET_PLAYER_QUIT     0     /*退出播放器，不做其他任何操作*/
#define RET_PLAYER_LOCAL   1   /*在本业务内切换*/ 
#define RET_PLAYER_OTHER   2  /*切换到其他业务*/


#define SECONDS_ONEDAY 86400
#define SECONDS_EIGHTHOURS 28800
#define SPEED_OFFSET 100


#define PLAYER_Full_Win_X 0 
#define PLAYER_Full_Win_Y 0
#define PLAYER_Full_Win_WIDTH 10000
#define PLAYER_Full_Win_HEGHT 10000


//超时设置 秒为单位
#define GET_BUFFER_TIMEOUT 3
#define NO_DATA_TIMEOUT 3
#define NO_ADDATA_TIMEOUT 2
#define SEND_DATA_TIMEOUT 4
#define GET_URL_TIMEOUT 3

enum Trick_Mode{
	RM_TRICKMODE_NO = 0,
	RM_TRICKMODE_FWD2 = 2,
	RM_TRICKMODE_FWD4 = 4,
	RM_TRICKMODE_FWD8 = 8,
	RM_TRICKMODE_FWD16 = 16,
	RM_TRICKMODE_BWD2 = -2,
	RM_TRICKMODE_BWD4 = -4,
	RM_TRICKMODE_BWD8 = -8,
	RM_TRICKMODE_BWD16 = -16
};

/* Key press goto actions */
typedef enum 
{
    PlAYERProcess_cmd_goto_none=15,
    PLAYERProcess_cmd_goto_rstart,
    PLAYERProcess_cmd_goto_mainloop,
    PLAYERProcess_cmd_goto_cleanup
}PLAYERProcess_cmd_goto;

#ifndef offsetof
#define offsetof(s, e) ((size_t)&((s *)0)->e)
#endif

#define U64fromTwo32(timestamp_msb,timestamp_lsb)  (((unsigned long long)timestamp_msb << 32)|timestamp_lsb)

#define PROCESS_MSG_RESULT(cmd)    			 \
do{											\
	switch(cmd)								\
	{										\
		case PLAYERProcess_cmd_goto_rstart:	\
			goto restart;						\
		case PLAYERProcess_cmd_goto_mainloop:	\
			goto main_loop;					\
		case PLAYERProcess_cmd_goto_cleanup:	\
			goto cleanup;						\
		default:								\
			break;							\
	}										\
}while(0);									

class CommonPlayer
{
	
public:
	CommonPlayer();

	~CommonPlayer();

	int PlayerRecvStopKey();

	void process_common_msg(int cmdtype, int *processed);

	void Set_Player_Window(player_window *pplayerwindow);

	void check_prebuffering_state(unsigned int buffersize);

	
protected:

	player_window fullwindowparms; //全屏窗口
	
	int 	TSPlayerHandle;

	int ifalways_fullwin; //是否总处于全屏状态

	player_window playerwindow; //播放窗口
	
	int iftvfullscreen;//当前是否为全屏状态
	
	int prebuf_level;
	int prebuf_max;

	int trickmode_id; //标明快进、快退的方向,大于0快进
	
	enum DSPStopmode stopmode;

	enum DSPPlaymode playmode;

	int volumeindex;

	int ismute;
	
	int status_network;
	
	int status_decoder;

	//unsigned int playingtime;

	char msgdata[MAX_MESSAGE_LEN];
	int msgtype;

	unsigned short     seqnumber;
	
};

#endif
