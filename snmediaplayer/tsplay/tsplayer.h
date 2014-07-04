#ifndef _TSPLAYER_H
#define _TSPLAYER_H

#include "commonplayer.h"
typedef enum {

	SDSPFP_OK = 0,
	SDSPFP_ERROR,
	SDSPFP_PENDING
	
}SDSPFP_RETVALUE;


#define FILENAME_LENGTH 256
#define VIDEO_FULL_SCREEN 10000
//#define SCREEN_WIDTH 1920
//#define SCREEN_HEIGHT 1080

/*
enum DSPPlaymode{
	DSPVideoPlayFwd, //��������ģʽ
	DSPVideoPlayIFrame,//I֡����ģʽ
};
enum DSPStopmode {
	DSPStopMode_BlackFrame = 0,//ֹͣ�ں�����
	DSPStopMode_LastFrame // ֹͣ�����һ֡��
};
*/
#if defined(__cplusplus)
extern "C" {
#endif

SDSPFP_RETVALUE DSP_TSPlayer_Open(int *pTSPlayerhandle);
SDSPFP_RETVALUE DSP_TSPlayer_Close(int TSPlayerhandle);
SDSPFP_RETVALUE DSP_TSPlayer_Reset(int TSPlayerhandle, int need_resetdemuxinfo);
//dma
SDSPFP_RETVALUE DSP_TSPlayer_GetBuffer(int TSPlayerhandle, unsigned int *bufsize, unsigned char **buf);
SDSPFP_RETVALUE DSP_TSPlayer_ReleaseBuffer(int TSPlayerhandle, unsigned char *buf);
SDSPFP_RETVALUE DSP_TSPlayer_SendData(int TSPlayerhandle, unsigned char *buf, unsigned int count);
SDSPFP_RETVALUE DSP_TSPlayer_Play(int TSPlayerhandle, enum DSPPlaymode mode, int speed);
SDSPFP_RETVALUE DSP_TSPlayer_Stop(int TSPlayerhandle, enum DSPStopmode mode);
SDSPFP_RETVALUE DSP_TSPlayer_Pause(int TSPlayerhandle);
SDSPFP_RETVALUE DSP_TSPlayer_GetVolumeIndex(int TSPlayerhandle, int *index);
SDSPFP_RETVALUE DSP_TSPlayer_SetVolumeIndex(int TSPlayerhandle, int index);
SDSPFP_RETVALUE DSP_TSPlayer_Setlocation(int TSPlayerhandle, unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height);
SDSPFP_RETVALUE DSP_TSPlayer_GetCurrentPTS(int TSPlayerhandle, unsigned long long *time);
SDSPFP_RETVALUE DSP_TSPlayer_GetVideoReadSize(int TSPlayerhandle, unsigned long *datasize);



int DSP_DVBplay_init(unsigned int *phAvplay, unsigned int *phWin);
int DSP_DVBplay_tune(unsigned int Freq);
int DSP_DVBplay_search(unsigned short pnid);
int DSP_DVBplay(unsigned int hAvplay, unsigned int hWin, unsigned int  ProgNum, unsigned char *cas_key);
int DSP_DVBplay_stop(unsigned int hAvplay);
int DSP_DVBplay_close(unsigned int hAvplay, unsigned int hWin);  
int DSP_DVBplay_setwindow(int x,int y, int h,int w,unsigned int hWin);  //������Ƶ����С
int DSP_DVBplay_tplay(unsigned int hAvplay);   //I֡����ģʽ�����ڿ������
int DSP_DVBplay_resume(unsigned int hAvplay);   //�ָ���������
int DSP_DVBplay_check_stream_state(unsigned int hAvplay); //vod����ʱ������Ƿ�����ֹͣ
int DSP_DVBplay_setvolume(unsigned int u32Volume);

#if defined(__cplusplus)
}
#endif
//������
#endif

