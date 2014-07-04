#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_unf_common.h"
#include "hi_unf_avplay.h"
#include "hi_unf_sound.h"
#include "hi_unf_disp.h"
#include "hi_unf_vo.h"
#include "hi_unf_demux.h"
#include "hi_unf_hdmi.h"
#include "hi_unf_ecs.h"
#include "HA.AUDIO.MP3.decode.h"
#include "HA.AUDIO.MP2.decode.h"
#include "HA.AUDIO.AAC.decode.h"
#include "HA.AUDIO.AC3.decode.h"
#include "HA.AUDIO.DTS.decode.h"
#include "HA.AUDIO.DRA.decode.h"
#include "HA.AUDIO.PCM.decode.h"
#include "HA.AUDIO.WMA9STD.decode.h"
#include "HA.AUDIO.AMRNB.codec.h"
#include "hi_adp.h"
#include "hi_adp_audio.h"
#include "hi_adp_hdmi.h"
#include "hi_adp_boardcfg.h"
#include "hi_adp_mpi.h"
#include "hi_adp_tuner.h"
//#include "DSP_DVBplay.h"

#include "tsplayer.h"

#if defined(__cplusplus)
extern "C" {
#endif

extern int get_videosetmode(void);

#define SETTINGXMLNAME "/tango/IXApp/setting.xml"

HI_U32 g_TunerFreq;
HI_U32 g_TunerSrate;
HI_U32 g_TunerQam;
static HI_UNF_ENC_FMT_E   g_enDefaultFmt = HI_UNF_ENC_FMT_1080i_50;

PMT_COMPACT_TBL    *g_pProgTbl = HI_NULL;
static HI_HANDLE g_phKey;
static HI_HANDLE g_phDmxAudChn;
static HI_HANDLE g_phDmxVidChn;
static HI_S8 g_isScramble = HI_FAILURE;

#ifndef BOARD_TYPE_fpga
#define DEFAULT_DVB_PORT 0
#else
#define DEFAULT_DVB_PORT 1
#endif

SDSPFP_RETVALUE DSP_TSPlayer_Open(int *pTSPlayerhandle)
{}
SDSPFP_RETVALUE DSP_TSPlayer_Close(int TSPlayerhandle)
{}
SDSPFP_RETVALUE DSP_TSPlayer_Reset(int TSPlayerhandle, int need_resetdemuxinfo)
{}
//dma
SDSPFP_RETVALUE DSP_TSPlayer_GetBuffer(int TSPlayerhandle, unsigned int *bufsize, unsigned char **buf)
{}
SDSPFP_RETVALUE DSP_TSPlayer_ReleaseBuffer(int TSPlayerhandle, unsigned char *buf)
{}
SDSPFP_RETVALUE DSP_TSPlayer_SendData(int TSPlayerhandle, unsigned char *buf, unsigned int count)
{}
SDSPFP_RETVALUE DSP_TSPlayer_Play(int TSPlayerhandle, enum DSPPlaymode mode, int speed)
{}
SDSPFP_RETVALUE DSP_TSPlayer_Stop(int TSPlayerhandle, enum DSPStopmode mode)
{}
SDSPFP_RETVALUE DSP_TSPlayer_Pause(int TSPlayerhandle)
{}

SDSPFP_RETVALUE DSP_TSPlayer_GetVolumeIndex(int TSPlayerhandle, int *index)
{}
SDSPFP_RETVALUE DSP_TSPlayer_SetVolumeIndex(int TSPlayerhandle, int index)
{}
SDSPFP_RETVALUE DSP_TSPlayer_Setlocation(int TSPlayerhandle, unsigned int X, unsigned int Y, unsigned int Width, unsigned int Height)
{}
SDSPFP_RETVALUE DSP_TSPlayer_GetCurrentPTS(int TSPlayerhandle, unsigned long long *time)
{}
SDSPFP_RETVALUE DSP_TSPlayer_GetVideoReadSize(int TSPlayerhandle, unsigned long *datasize)
{}


HI_S32 DSP_DVBplay_init(HI_HANDLE *phAvplay, HI_HANDLE *phWin)
{
//如果初始化失败，可能在TRE已经初始化过，暂时不做出错处理

	HI_S32                  Ret;
	HI_HANDLE               hWin;

	HI_HANDLE               hAvplay;
	HI_UNF_AVPLAY_ATTR_S        AvplayAttr;
	HI_UNF_SYNC_ATTR_S          SyncAttr;
	HI_UNF_AVPLAY_STOP_OPT_S    Stop;
	HI_CHAR                 InputCmd[32];
	HI_U32                  ProgNum;
	HI_U32  u32Playtime;
	HI_UNF_WINDOW_ATTR_S              winattr;
	
	//HI_SYS_Init();
	//HI_SYS_PreAV(NULL);
/*
	Ret = HIADP_HDMI_Init(HI_UNF_HDMI_ID_0, HI_UNF_ENC_FMT_1080i_50);
       if (HI_SUCCESS != Ret)
       {
           printf("call HIADP_HDMI_Init  failed.\n");
       
       }
       Ret = HI_UNF_HDMI_Start(HI_UNF_HDMI_ID_0);
       if (HI_SUCCESS != Ret)
       {
           printf("call HI_UNF_HDMI_Start  failed.\n");
       
       }
	*///20111209 xuc for dongguan version
	Ret = HIADP_Snd_Init();
	if (HI_SUCCESS != Ret)
	{
		printf("call HIADP_Snd_Init failed.\n");
		//goto SYS_DEINIT;
	}

	HI_SYS_GetPlayTime(& u32Playtime);
	printf("u32Playtime = %d\n",u32Playtime);
	/*Ret = HIADP_Disp_Init(g_enDefaultFmt);
	if (HI_SUCCESS != Ret)
	{
		printf("call HIADP_Disp_DeInit failed.\n");
		//goto SND_DEINIT;
	}*/

	Ret = HIADP_VO_Init(HI_UNF_VO_DEV_MODE_NORMAL);
	Ret |= HIADP_VO_CreatWin(HI_NULL,&hWin);
	if (HI_SUCCESS != Ret)
	{
		printf("call HIADP_VO_Init failed.\n");
		HIADP_VO_DeInit();
		//goto DISP_DEINIT;
	}
	
	Ret = HI_UNF_DMX_Init();
	Ret |= HI_UNF_DMX_AttachTSPort(0,DEFAULT_DVB_PORT);
	if (HI_SUCCESS != Ret)
	{
		printf("call HIADP_Demux_Init failed.\n");
		//goto VO_DEINIT;
	}

	Ret = HIADP_Tuner_Init();
	if (HI_SUCCESS != Ret)
	{
		printf("call HIADP_Demux_Init failed.\n");
		//goto DMX_DEINIT;
	}
	

	Ret = HIADP_AVPlay_RegADecLib();
	Ret |= HI_UNF_AVPLAY_Init();
	if (Ret != HI_SUCCESS)
	{
		printf("call HI_UNF_AVPLAY_Init failed.\n");
		//goto PSISI_FREE;
	}

	Ret = HI_UNF_AVPLAY_GetDefaultConfig(&AvplayAttr, HI_UNF_AVPLAY_STREAM_TYPE_TS);
	AvplayAttr.stStreamAttr.u32VidBufSize = (9*1024*1024);
	AvplayAttr.stStreamAttr.u32AudBufSize = (1*1024*1024);
	Ret |= HI_UNF_AVPLAY_Create(&AvplayAttr, &hAvplay);
	if (Ret != HI_SUCCESS)
	{
		printf("call HI_UNF_AVPLAY_Create failed.\n");
		//goto AVPLAY_DEINIT;
	}

	Ret = HI_UNF_AVPLAY_ChnOpen(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID, HI_NULL);
	Ret |= HI_UNF_AVPLAY_ChnOpen(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_AUD, HI_NULL);
	if (HI_SUCCESS != Ret)
	{
		printf("call HI_UNF_AVPLAY_ChnOpen failed.\n");
		//goto CHN_CLOSE;
	}
	Ret = HI_UNF_VO_AttachWindow(hWin, hAvplay);
	if (HI_SUCCESS != Ret)
	{
		printf("call HI_UNF_VO_AttachWindow failed.\n");
		//return Ret;
	}
	/*
	   HI_UNF_VO_GetWindowAttr(hWin, &winattr);
	   winattr.stOutputRect.s32X = x;
	   winattr.stOutputRect.s32Y= y;
	   winattr.stOutputRect.s32Height= h;
	   winattr.stOutputRect.s32Width= w;
	   winattr.enAspectRatio = HI_UNF_ASPECT_RATIO_4TO3;
	   HI_UNF_VO_SetWindowAttr(hWin, &winattr);
	 */
	Ret = HI_UNF_VO_SetWindowEnable(hWin, HI_TRUE);
	if (HI_SUCCESS != Ret)
	{
		printf("call HI_UNF_VO_SetWindowEnable failed.\n");
	//	return Ret;
	}



	Ret = HI_UNF_SND_Attach(HI_UNF_SND_0, hAvplay, HI_UNF_SND_MIX_TYPE_MASTER, 100);
	if (Ret != HI_SUCCESS)
	{
		printf("call HI_SND_Attach failed.\n");
		//goto WIN_DETACH;
	}

	Ret = HI_UNF_SND_SetTrackMode(HI_UNF_SND_0, HI_UNF_TRACK_MODE_DOUBLE_LEFT);
	if (Ret != HI_SUCCESS)
		printf("call HI_UNF_SND_SetTrackMode failed.\n");
	
	Ret = HI_UNF_AVPLAY_GetAttr(hAvplay, HI_UNF_AVPLAY_ATTR_ID_SYNC, &SyncAttr);
	SyncAttr.enSyncRef = HI_UNF_SYNC_REF_AUDIO;
	SyncAttr.stSyncStartRegion.s32VidPlusTime = 60;
	SyncAttr.stSyncStartRegion.s32VidNegativeTime = -20;
	SyncAttr.bQuickOutput = HI_FALSE;
	Ret = HI_UNF_AVPLAY_SetAttr(hAvplay, HI_UNF_AVPLAY_ATTR_ID_SYNC, &SyncAttr);
	if (Ret != HI_SUCCESS)
	{
		printf("call HI_UNF_AVPLAY_SetAttr failed.\n");
		//goto SND_DETACH;
	}
	
	*phAvplay = hAvplay;
	*phWin = hWin;

#if 0
	if(0)    //xuc : only for the deinit of failing init!
	{
AVPLAY_STOP:
		Stop.enMode = HI_UNF_AVPLAY_STOP_MODE_BLACK;
		Stop.u32TimeoutMs = 0;
		HI_UNF_AVPLAY_Stop(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD, &Stop);

SND_DETACH:
		HI_UNF_SND_Detach(HI_UNF_SND_0, hAvplay);

WIN_DETACH:
		HI_UNF_VO_SetWindowEnable(hWin,HI_FALSE);
		HI_UNF_VO_DetachWindow(hWin, hAvplay);

CHN_CLOSE:
		HI_UNF_AVPLAY_ChnClose(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD);

		HI_UNF_AVPLAY_Destroy(hAvplay);

AVPLAY_DEINIT:
		HI_UNF_AVPLAY_DeInit();

PSISI_FREE:
		HIADP_Search_FreeAllPmt(g_pProgTbl);
		HIADP_Search_DeInit();

TUNER_DEINIT:
		HIADP_Tuner_DeInit(0);

DMX_DEINIT:
		HI_UNF_DMX_DetachTSPort(0);
		HI_UNF_DMX_DeInit();

VO_DEINIT:
		HI_UNF_VO_DestroyWindow(hWin);
		HIADP_VO_DeInit();

DISP_DEINIT:
		HIADP_Disp_DeInit();

SND_DEINIT:
		HIADP_Snd_DeInit();

SYS_DEINIT:
		HI_SYS_DeInit();
		return 1;
	}
#endif
	return HI_SUCCESS;
}

HI_S32 DSP_DVBplay_tune(HI_U32 Freq)
{
//符号率、调制模式、文件格式都是暂时写死的!
	HI_S32 Ret;

	if(Freq < 900 && Freq > 500)
	{
		g_TunerFreq  = Freq;
		g_TunerSrate = 6875;
		g_TunerQam   = 64;
		g_enDefaultFmt = HI_UNF_ENC_FMT_1080i_50;
	}

	printf("TUNER_USE = %d, g_TunerFreq = %d, g_TunerSrate = %d, g_TunerQam = %d\n",TUNER_USE,g_TunerFreq,g_TunerSrate,g_TunerQam);
	Ret = HIADP_Tuner_Connect(TUNER_USE,g_TunerFreq,g_TunerSrate,g_TunerQam);
	if (HI_SUCCESS != Ret)
	{
		printf("call HIADP_Tuner_Connect failed.\n");
		return  Ret;	
	}

	return HI_SUCCESS;
}

HI_S32 DSP_DVBplay_search(HI_U16 pnid)
{
	HI_S32 Ret;
	HI_S32 index, done = -1;

	HIADP_Search_Init();
	Ret = HIADP_Search_GetAllPmt(0,&g_pProgTbl);
        if (HI_SUCCESS != Ret)   
	{
		printf("call HIADP_Search_GetAllPmt failed\n");
		goto FAILURE;
	}

	if (g_pProgTbl->prog_num == 0)
	{
		printf("call HIADP_Search_GetAllPmt failed, program num = 0\n");
		goto FAILURE;
	}
	
	for (index = 0; index < g_pProgTbl->prog_num; index++)
	{
		if (g_pProgTbl->proginfo[index].ProgID == pnid)
		{
			done = HI_SUCCESS;
			break;
		}
	}
	if (done == HI_SUCCESS)
		return HI_SUCCESS;
	printf("call HIADP_Search_GetAllPmt failed, dont have pnid %d\n", pnid);
FAILURE:
	if (g_pProgTbl)
	{
		HIADP_Search_FreeAllPmt(g_pProgTbl);
		g_pProgTbl = HI_NULL;
	}
	return HI_FAILURE;
}

HI_S32 DSP_DVBplay(HI_HANDLE hAvplay, HI_HANDLE hWin, HI_U32  ProgNum, HI_U8 *cas_key)
{
	HI_S32 Ret;
	HI_S32 index;
	HI_S32 done = -1;
	HI_UNF_DMX_DESCRAMBLER_ATTR_S  stAttr;

	for (index = 0; index < g_pProgTbl->prog_num; index++)
	{
		if (g_pProgTbl->proginfo[index].ProgID == ProgNum)
		{
			done = HI_SUCCESS;
			break;
		}
	}
	if (done == HI_SUCCESS)
	{
		if (cas_key != HI_NULL)
		{
			printf("start to descramble\n");
			printf("scramble video, key %02x %02x %02x %02x %02x %02x %02x %02x\n",
				cas_key[0], cas_key[1], cas_key[2], cas_key[3], 
				cas_key[4], cas_key[5], cas_key[6], cas_key[7]);
			g_isScramble = HI_SUCCESS;
			stAttr.enCaType = HI_UNF_DMX_CA_NORMAL;
			stAttr.enDescramblerType = HI_UNF_DMX_DESCRAMBLER_TYPE_CSA2;
			stAttr.enEntropyReduction = HI_UNF_DMX_CA_ENTROPY_REDUCTION_OPEN;
			Ret = HI_UNF_DMX_CreateDescramblerExt(0, &stAttr, &g_phKey);
			if (Ret != HI_SUCCESS)    
			{        
				printf("call HI_UNF_DMX_CreateDescramblerExt failed.\n");
			}
			else
			{
				//设置偶密钥
				printf("CAS : set even and odd key\n");
				Ret = HI_UNF_DMX_SetDescramblerEvenKey(g_phKey, cas_key);
				if (Ret != HI_SUCCESS)
				{
					printf("call HI_UNF_DMX_SetDescramblerEvenKey failed.\n");
				}
				//设置奇密钥
				Ret = HI_UNF_DMX_SetDescramblerOddKey(g_phKey, cas_key);
				if (Ret != HI_SUCCESS)
				{
					printf("call HI_UNF_DMX_SetDescramblerOddKey failed.\n");
				}
				//获取音频通道并绑定密钥区
				printf("CAS : get dmx video/audio channel,  set  scrambler\n");
				Ret = HI_UNF_AVPLAY_GetDmxAudChnHandle(hAvplay, &g_phDmxAudChn);
				if (Ret != HI_SUCCESS)
				{
					printf("call HI_UNF_AVPLAY_GetDmxAudChnHandle failed.\n");
				}
				else
				{
					Ret = HI_UNF_DMX_AttachDescrambler(g_phKey, g_phDmxAudChn);
					if (Ret != HI_SUCCESS)
					{
						printf("call HI_UNF_DMX_AttachDescrambler failed.\n");
					}
				}
				//获取视频通道并绑定密钥区
				Ret = HI_UNF_AVPLAY_GetDmxVidChnHandle(hAvplay, &g_phDmxVidChn);
				if (Ret != HI_SUCCESS)
				{
					printf("call HI_UNF_AVPLAY_GetDmxVidChnHandle failed.\n");
				}
				else
				{
					Ret = HI_UNF_DMX_AttachDescrambler(g_phKey, g_phDmxVidChn);
					if (Ret != HI_SUCCESS)
					{
						printf("call HI_UNF_DMX_AttachDescrambler failed.\n");
					}
				}
			}
			
			
		}
		else
		{
			printf("cas_key == NULL\n");
		//	return -1;
		}
		Ret = HIADP_AVPlay_PlayProg(hAvplay,g_pProgTbl,index,HI_TRUE);
		if (Ret != HI_SUCCESS)
		{
	
			printf("HIADP_AVPlay_PlayProg failed!!!.\n");     //上层调用DSP_DVBplay_close
			return Ret;
		}
	}
	else
	{
		printf("don't have program %d\n", ProgNum);
		return -1;
	}

	return HI_SUCCESS;
}

HI_S32 DSP_DVBplay_setwindow(HI_S32 x,HI_S32 y, HI_S32 h,HI_S32 w,HI_HANDLE hWin)
{
	HI_S32 vmode;
	HI_S32 ret;
	char video_resolution[32];
	char video_mode[32];
	HI_S32 osdh, osdw;
	HI_UNF_WINDOW_ATTR_S winattr;
	HI_UNF_VO_GetWindowAttr(hWin, &winattr);
	printf("DSP_DVBplay_setwindow  x=%d,y=%d,h=%d,w=%d\n",x,y,h,w);
	
	vmode = get_videosetmode();
	
	if (0 == vmode)
	{
		ret = Read_Xml_Setting(SETTINGXMLNAME, "videoresolution", video_resolution);
		if (-1 == ret)
   	{
    	printf("read video_resolution failed\n");
      osdw = 1280;
      osdh = 720;
    }
    printf("video_resolution = %s\n", video_resolution);
		if(strstr(video_resolution,"720x480i") || strstr(video_resolution,"720x480p"))
		{
			osdw = 720;
			osdh = 480;
		}
		else if(strstr(video_resolution,"720x576i") || strstr(video_resolution,"720x576p"))
		{
			osdw = 720;
			osdh = 576;
		}
		else if(strstr(video_resolution,"1280x720i") || strstr(video_resolution,"1280x720p"))
		{
			osdw = 1280;
			osdh = 720;
		}
		else if(strstr(video_resolution,"1920x1080i") || strstr(video_resolution,"1920x1080p"))
		{
			osdw = 1920;
			osdh = 1080;
		}
	}
	else if (1 == vmode)
	{
		ret = Read_Xml_Setting(SETTINGXMLNAME, "videomode", video_mode);
		if (-1 == ret)
		{
			printf("read videomode failed\n");
			strcpy(video_mode, "PAL");
		}
		if (!strcmp(video_mode, "PAL"))
		{
			osdw = 720;
			osdh = 576;
		}
		else if (!strcmp(video_mode, "NTSC"))
		{
			osdw = 720;
			osdh = 480;
		}
	}
	
	//xuc: osd层分辨率为1280*720, 视频层分辨率为1920*1080
	/*
	x = x*1920/1280;
	y = y*1080/720;
	h = h*1080/720;
	w = w*1920/1280;
	*/
	/*
	x = x*1920/osdw;
	y = y*1080/osdh;
	h = h*1080/osdh;
	w = w*1920/osdw;
	printf("->DSP_Dvbplay x=%d,y=%d,h=%d,w=%d\n",x,y,h,w);
*/
//XUC 0816 视频位置都是按照720p osd的位置传的
	printf("osdw = %d, osdh = %d\n", osdw, osdh);
	x = x*osdw/1280;
	y = y*osdh/720;
	h = h*osdh/720;
	w = w*osdw/1280;
	printf("->DSP_Dvbplay(720P) x=%d,y=%d,h=%d,w=%d\n",x,y,h,w);
	
	winattr.stOutputRect.s32X = x;
	winattr.stOutputRect.s32Y= y;
	winattr.stOutputRect.s32Height= h;
	winattr.stOutputRect.s32Width= w;
	winattr.enAspectRatio = HI_UNF_ASPECT_RATIO_4TO3;
	HI_UNF_VO_SetWindowAttr(hWin, &winattr);

	return HI_SUCCESS;
}

HI_S32 DSP_DVBplay_stop(HI_HANDLE hAvplay)
{
	HI_UNF_AVPLAY_STOP_OPT_S    Stop;
	HI_S32 Ret;
	
	printf("[DBG]init\n");

	Stop.enMode = HI_UNF_AVPLAY_STOP_MODE_STILL;
	Stop.u32TimeoutMs = 0;

	if (g_pProgTbl != HI_NULL)
	{
		printf("[DBG]HIADP_Search_FreeAllPmt start\n");
		HIADP_Search_FreeAllPmt(g_pProgTbl);
		printf("[DBG]HIADP_Search_FreeAllPmt end\n");
	}
	g_pProgTbl = HI_NULL;
	

	if (g_isScramble == HI_SUCCESS)
	{
		printf("close descramble\n");
		Ret = HI_UNF_DMX_DetachDescrambler(g_phKey, g_phDmxVidChn);
		if (Ret != HI_SUCCESS)
		{
			printf("call HI_UNF_DMX_DetachDescrambler failed.\n");
		}
		Ret = HI_UNF_DMX_DetachDescrambler(g_phKey, g_phDmxAudChn);
		if (Ret != HI_SUCCESS)
		{
			printf("call HI_UNF_DMX_DetachDescrambler failed.\n");
		}
		Ret = HI_UNF_DMX_DestroyDescrambler(g_phKey);
		if (Ret != HI_SUCCESS)
		{
			printf("call HI_UNF_DMX_DestroyDescrambler failed.\n");
		}
		g_isScramble = HI_FAILURE;
	}
	
	printf("[DBG]HI_UNF_AVPLAY_Stop start\n");

	HI_UNF_AVPLAY_Stop(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD, &Stop);
	printf("[DBG]HI_UNF_AVPLAY_Stop exit\n");

	return HI_SUCCESS;
}

//I帧播放
HI_S32 DSP_DVBplay_tplay(HI_HANDLE hAvplay)
{
	HI_S32 ret;
	ret = HI_UNF_AVPLAY_SetDecodeMode(hAvplay, HI_UNF_VCODEC_MODE_I);
	if(ret!=HI_SUCCESS)
	{
		  printf("HI_UNF_AVPLAY_SetDecodeMode error!\n");
	        return HI_FAILURE;
	}
	printf("SetDecodeMode = MODE_I\n");

	ret = HI_UNF_AVPLAY_Reset(hAvplay, NULL);
	if(ret!=HI_SUCCESS)
	{
		  printf("HI_UNF_AVPLAY_Reset error!\n");
	        return HI_FAILURE;
	}	
	ret = HI_UNF_AVPLAY_Tplay(hAvplay, NULL);
	if(ret!=HI_SUCCESS)
	{
		  printf("HI_UNF_AVPLAY_Tplay error!\n");
	        return HI_FAILURE;
	}
	printf("DSP_DVBplay_tplay success\n");
	return HI_SUCCESS;

}

//恢复正常播放
//由于不使用I帧播放，DSP_DVBplay_resume的作用修改为reset播放器
HI_S32 DSP_DVBplay_resume(HI_HANDLE hAvplay)
{
	
	HI_S32 ret;

	ret = HI_UNF_AVPLAY_SetDecodeMode(hAvplay, HI_UNF_VCODEC_MODE_NORMAL);
	if(ret!=HI_SUCCESS)
	{
		  printf("HI_UNF_AVPLAY_SetDecodeMode error!\n");
	        return HI_FAILURE;
	}
	printf("SetDecodeMode = NORMAL\n");

	ret = HI_UNF_AVPLAY_Reset(hAvplay, NULL);
	if(ret!=HI_SUCCESS)
	{
		  printf("HI_UNF_AVPLAY_Reset error!\n");
	        return HI_FAILURE;
	}	
	
	ret = HI_UNF_AVPLAY_Resume(hAvplay, NULL);
	if(ret!=HI_SUCCESS)
	{
		  printf("HI_UNF_AVPLAY_Resume error!\n");
	        return HI_FAILURE;
	}
	printf("DSP_DVBplay_resume success\n");

	return HI_SUCCESS;
}

//不准确故不使用
HI_S32 DSP_DVBplay_check_stream_state(HI_HANDLE hAvplay)
{
	HI_BOOL pbIsEmpty;
	HI_S32 ret;

	ret = HI_UNF_AVPLAY_IsBuffEmpty(hAvplay, &pbIsEmpty);
	if(ret!= HI_SUCCESS)
	{	
		printf("HI_UNF_AVPLAY_IsBuffEmpty failed!\n");
		return HI_FAILURE;
	}
	return pbIsEmpty;
}
HI_S32 DSP_DVBplay_setvolume(HI_U32 u32Volume)
{
	HI_S32 Ret;
	Ret = HI_UNF_SND_SetVolume(HI_UNF_SND_0, u32Volume);
	if(Ret!= HI_SUCCESS)
	{	
		printf("DSP_DVBplay_setvolume failed!\n");
		return HI_FAILURE;
	}
	return HI_SUCCESS;
}

HI_S32 DSP_DVBplay_close(HI_HANDLE hAvplay, HI_HANDLE hWin)
{

	HI_UNF_AVPLAY_STOP_OPT_S    Stop;
	HI_S32 Ret;

	printf("DSP_DVBplay_close\n");

	Stop.enMode = HI_UNF_AVPLAY_STOP_MODE_STILL;
	Stop.u32TimeoutMs = 0;

	if (g_isScramble == HI_SUCCESS)
	{
		printf("close descramble\n");
		Ret = HI_UNF_DMX_DetachDescrambler(g_phKey, g_phDmxVidChn);
		if (Ret != HI_SUCCESS)
		{
			printf("call HI_UNF_DMX_DetachDescrambler failed.\n");
		}
		Ret = HI_UNF_DMX_DetachDescrambler(g_phKey, g_phDmxAudChn);
		if (Ret != HI_SUCCESS)
		{
			printf("call HI_UNF_DMX_DetachDescrambler failed.\n");
		}
		Ret = HI_UNF_DMX_DestroyDescrambler(g_phKey);
		if (Ret != HI_SUCCESS)
		{
			printf("call HI_UNF_DMX_DestroyDescrambler failed.\n");
		}
		g_isScramble = HI_FAILURE;
	}
	
	HI_UNF_AVPLAY_Stop(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD, &Stop);

	HI_UNF_SND_Detach(HI_UNF_SND_0, hAvplay);

	HI_UNF_VO_SetWindowEnable(hWin,HI_FALSE);
	HI_UNF_VO_DetachWindow(hWin, hAvplay);

	HI_UNF_AVPLAY_ChnClose(hAvplay, HI_UNF_AVPLAY_MEDIA_CHAN_VID | HI_UNF_AVPLAY_MEDIA_CHAN_AUD);

	HI_UNF_AVPLAY_Destroy(hAvplay);

	HI_UNF_AVPLAY_DeInit();
	if (g_pProgTbl != HI_NULL)
		HIADP_Search_FreeAllPmt(g_pProgTbl);
	g_pProgTbl = HI_NULL;
	HIADP_Search_DeInit();	

	HIADP_Tuner_DeInit(0);

	HI_UNF_DMX_DetachTSPort(0);
	HI_UNF_DMX_DeInit();

	HI_UNF_VO_DestroyWindow(hWin);
	HIADP_VO_DeInit();

//	HIADP_Disp_DeInit();

	HIADP_Snd_DeInit();
/*
	HI_UNF_HDMI_Stop(HI_UNF_HDMI_ID_0);
	HI_UNF_HDMI_DeInit();
	*/	//20111209 xuc for dongguan version

//	HI_SYS_DeInit();

	return HI_SUCCESS;
}


#if defined(__cplusplus)
}
#endif
