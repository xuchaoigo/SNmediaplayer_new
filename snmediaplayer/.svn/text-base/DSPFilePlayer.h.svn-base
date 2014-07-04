#ifndef _DSPFILEPLAYER_H
#define _DSPFILEPLAYER_H


//#include <directfb.h>
//#include "config.h"
//#define WITHSIGMADFB

//#define MAX_NAME_LENGTH 256
#define VIDEO_FULL_SCREEN 10000

#include "commonplayer.h"

/*
typedef enum {

	DSPAUDIO_AAC = 0,
	DSPAUDIO_MPEG,
	DSPAUDIO_AC3,
	DSPAUDIO_DTS
	
}DSPAUDIO_TYPE;


typedef enum {

	DSPVIDEO_H264 = 0,
	DSPVIDEO_MPEG4,
	DSPVIDEO_MPEG2,
	DSPVIDEO_AVS
	
}DSPVIDEO_TYPE;
*/
enum DSPOutputDualMode_type {
	DSPDualMode_Stereo = 0,
	DSPDualMode_LeftMono,
	DSPDualMode_RightMono,
	DSPDualMode_MixMono
};


// 输出屏幕大小
struct DSPOut_window
{
	int x;
	int y;
	int width;
	int height;
};

//typedef int (*SHOWING_DIALOG)();

#define DSP_MAX_FILENAME_LENGTH 512
struct DSPFilePlayParms
{
	char url[DSP_MAX_FILENAME_LENGTH];
	int loop;
	int starttime;
	int fullscreen;
	int play_audio;
	int start_play;
	int volumeindex;
	int video_ratio;
	int stop_at_lastpicture;
	struct DSPOut_window playwindow;	
	//zhanghs adds for loading video
	//SHOWING_DIALOG start_loading_dialogue;
	//SHOWING_DIALOG end_loading_dialogue;
};
/*
struct DSPFilmInfo
{
    DSPAUDIO_TYPE atype;
    DSPVIDEO_TYPE  vtype;
    int ishdvideo;
};
*/
typedef enum {
	DSPFP_Playback_state_Playing = 56,
	DSPFP_Playback_state_Paused,
	DSPFP_Playback_state_Stopped,

	//DSPFP_Playback_state_NextPic,

	DSPFP_Playback_state_IForward,
	DSPFP_Playback_state_IBackward,

	//DSPFP_Playback_state_IPaused,
	//DSPFP_Playback_state_INextPic,

	//DSPFP_Playback_state_SilentForward,
	//DSPFP_Playback_state_SilentBackward,

	DSPFP_Playback_state_Buffering,
	DSPFP_Playback_state_Quitting
}DSPFP_Playback_state;

typedef enum{
	DSPFP_Playmode_PlayFwd,
	DSPFP_Playmode_PlayIFrame
}DSPFP_Playmode;
	
typedef enum{
	DSPFP_Player_Event_Ending,		//播放到文件尾
	DSPFP_Player_Event_Start,		//暂停
	DSPFP_Player_Event_Paused,		//暂停
	DSPFP_Playback_Event_Play,		//播放
	DSPFP_Playback_Event_IFrameForward,	//I 帧模式快进
	DSPFP_Playback_Event_IFranmeBackward,	//I 帧模式快退
	DSPFP_Player_Event_Error		//播放出错，可通过DSPFP_GetLastError 获取具体错误类型
}DSPFP_Player_Event;

typedef enum{
	DSP_Player_Error_None,			//没有错误
	DSP_Player_Error_File_Notexist,	//文件不存在
	DSP_Playback_Error_DecodeAudio,	//音频解码错误
	DSP_Playback_Error_DecodeVideo,	//视频解码错误
	DSP_Playback_Error_Memory,		//内存错误
	DSP_Playback_Error_ParseFile		//文件格式错误
}DSPFP_Player_ErrorType;
/*
struct DSPFPPlaybackState {
	DSPFP_Playback_state state;
	struct DSPFPSpeedParameters speed;
};

struct DSPFPPlaybackStatus {
	struct DSPFPPlaybackState playbackState;		// Current playback state
};
*/
#ifdef __cplusplus
	extern "C" {
#endif

typedef int (*EventCallBack)(int, const char *);
    
DSPFP_RETVALUE DSPFP_OpenPlayer(int * pPlayerHandle, int maindecoder);

DSPFP_RETVALUE DSPFP_LoadPlayparms(int PlayerHandle, struct DSPFilePlayParms *Playparms);

DSPFP_RETVALUE DSPFP_PlayFile(int pPlayerHandle);

DSPFP_RETVALUE DSPFP_SetPlayerLocation(int PlayerHandle, struct DSPOut_window playwindow);

DSPFP_RETVALUE DSPFP_StopPlayer(int PlayerHandle);

DSPFP_RETVALUE DSPFP_ClosePlayer(int PlayerHandle);

DSPFP_RETVALUE DSPFP_GetStreamDuration(int PlayerHandle,unsigned int *duration);

DSPFP_RETVALUE DSPFP_GetPlaybackTime(int PlayerHandle, unsigned int *time);

DSPFP_Playback_state DSPFP_GetPlayerState(int PlayerHandle);

DSPFP_RETVALUE DSPFP_SetAVRatio(int PlayerHandle, int arx,int ary);

DSPFP_RETVALUE DSPFP_PlayerSwitchAudio(int PlayerHandle, int stream);


DSPFP_RETVALUE DSPFP_SetAudiomode(int PlayerHandle, int audiomode);

DSPFP_RETVALUE DSPFP_SetVolume(int PlayerHandle, int volume);

DSPFP_RETVALUE DSPFP_SeekFileByTime(int PlayerHandle, int playtime);

DSPFP_RETVALUE DSPFP_Pause(int PlayerHandle);

DSPFP_RETVALUE DSPFP_Play(int PlayerHandle, int playmode, int speedM, int speedN);

DSPFP_Player_ErrorType DSPFP_GetLastError(int PlayerHandle);

int DSP_FPOVER(int PlayerHandle);

int DSP_FPSTART(int PlayerHandle);//jinhl
DSPFP_RETVALUE DSPFP_GetAudioStreamInfo(int PlayerHandle, int *curindex,int *newid,unsigned int *streams,char *newCodecInfo);
DSPFP_RETVALUE DSPFP_GetAudioStreamCodec(int PlayerHandle, char *audioCodec);

DSPFP_RETVALUE DSPFP_SetCircle(int PlayerHandle,int circle);

int DSP_FPOVER_WITHERROR(int PlayerHandle);

DSPFP_RETVALUE DSPFP_EventCallback(int PlayerHandle, EventCallBack eventcb);

DSPFP_RETVALUE DSPFP_SwitchAudio(int PlayerHandle, int stream);
#ifdef __cplusplus
}
#endif

#endif 

