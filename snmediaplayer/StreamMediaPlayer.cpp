#include <pthread.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "StreamMediaPlayer.h"
#include "tsplayer.h"
//#include "ui_display.h"
//#include "timer_handler.h"

//#include "vod_def.h"
#include "string.h"
//#include "DSP_DVBplay.h"
#include "CCGWclient_IPC.h"
#include "shm_ipc.h"
#include "hi_unf_common.h"
#include "hi_unf_avplay.h"


#define SETTINGXMLNAME "/tango/IXApp/setting.xml"
#define ACCOUNTNAME "AccountName"
#define CCGW_TIMEOUT 10000   //10s
#define USE_DRM 1
#define CCGW_IPC 1
#define ESTABLISH_TIMES 3
#define SETUP_TIMES 3
#define VCR_TIMEOUT 1000 /*ms*/
#define PTS_MAX_OFFSET 300000 /*ms*/

#if USE_DRM
char *VOD_LICENSE_FILE = "/tmp/vod_license";
#define DRM_LICENSE_LEN 128 /* 128 bytes license, 256bytes 字符串 */
#endif

#define MAX_VBUF_EMPTY_TIMES 3
//#define MAX_SEARCH_RETRY_TIMES 10
#define MAX_SEARCH_RETRY_TIMES 100

#define VOD_SCRAMBLE 1
#define VOD_NO_SCRAMBLE 0

//RTSPClient_prv rtsp_session_server;
//RTSPClient_prv rtsp_streaming_server; 

using namespace std;

extern "C"
{
	extern int Read_Xml_Setting(char *xml_filename, char *type, char *value);
	extern int scan_sg(void); /* sg_scan, scan service group */
	extern void scan_sg_init(int tunerid, int dmxid); /* sg_scan, scan service group */
	extern int init_IPC_channel(); /*CCGW*/
#if USE_DRM
	extern void DRMCFuncInit(void);
	extern long DRMCFuncPreprocess(const char * licenseName);
	extern long DRMCFuncGetFilmSecInfo(char* licenseName, unsigned int localTime, int *cryptMode, unsigned char *key);
	extern long  DRMCDecreaseCount(char * licenseName);
	extern long  DRMCFuncTeminate(void);
#endif
}


int request_to_open_channel(void)
{
        int ret;
        AppID_t  my_appID = APP_SDV;
        Channel_type_t  my_channel = DVB_WITH_IP_TUN;
        Request_type_t my_request = REQUEST_START;
	  DSP_IPC_Msg_t send_command, recv_command;

	  char *CCGWclient_online = NULL;
	  DSP_DEBUG("init_IPC_channel..");

        init_IPC_channel();

	 DSP_DEBUG("GetShmAddr..");
        GetShmAddr(3,1,(void**)&CCGWclient_online);
        if(0 == *CCGWclient_online)
        {
                DSP_DEBUG("CCGWclient is not online!");
                DetachShm(CCGWclient_online);
                return 0;
        }
        DetachShm(CCGWclient_online);

	  memset(&send_command, 0, sizeof(DSP_IPC_Msg_t));  
        send_command.mtype = my_appID;
        send_command.word[0] = my_channel;
        send_command.word[1] = my_request;

	  DSP_DEBUG("send_msg_to_CCGWclient..");
        ret = send_msg_to_CCGWclient(&send_command);
        if(ret)
        {
                DSP_DEBUG("send_msg_to_CCGWclient failed!");
                return ret;
        }

        DSP_DEBUG("recv_msg_from_CCGWclient..");
        memset(&recv_command, 0, sizeof(DSP_IPC_Msg_t));
        ret = recv_msg_from_CCGWclient( APP_SDV, &recv_command, CCGW_TIMEOUT);
        if(ret)
        {
                DSP_DEBUG("recv_msg_from_CCGWclient failed!");
                return ret;
        }
	  DSP_DEBUG("return!");

        if( recv_command.word[0] == my_channel && recv_command.word[1] == my_request && recv_command.word[2] == 1 )
                return 0;
        else  return 2;
}

int request_to_close_channel(void)
{
        int ret;
        AppID_t  my_appID = APP_SDV;
        Channel_type_t  my_channel = DVB_WITH_IP_TUN;
        Request_type_t my_request = REQUEST_END;
	  DSP_IPC_Msg_t send_command, recv_command;
	  
	  char *CCGWclient_online = NULL;
        init_IPC_channel();
        GetShmAddr(3,1,(void**)&CCGWclient_online);
        if(0 == *CCGWclient_online)
        {
                DSP_DEBUG("CCGWclient is not online!");
                DetachShm(CCGWclient_online);
                return 0;
        }
        DetachShm(CCGWclient_online);

	  memset(&send_command, 0, sizeof(DSP_IPC_Msg_t));  
        send_command.mtype = my_appID;
        send_command.word[0] = my_channel;
        send_command.word[1] = my_request;

        ret = send_msg_to_CCGWclient(&send_command);
        if(ret)
        {
                DSP_DEBUG("send_msg_to_CCGWclient failed!");
                return ret;
        }

        memset(&recv_command, 0, sizeof(DSP_IPC_Msg_t));
        ret = recv_msg_from_CCGWclient( APP_SDV, &recv_command ,CCGW_TIMEOUT );
        if(ret)
        {
                DSP_DEBUG("recv_msg_from_CCGWclient failed!");
                return ret;
        }

        if( recv_command.word[0] == my_channel && recv_command.word[1] == my_request)
                return 0;
        else  return 2;

}


StreamMediaPlayer::StreamMediaPlayer(int instanceid,int videoDisplayMode, int playmode,
	int height, int width, int left, int top, int muteFlag,  int muteuiflag, int subtitleFlag, 	
	int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag,string json_str,	
	int useNativeUIFlag,int audioVolumeUIFlag,int audioTrackUIFlag,int progressBarUIFlag,	
	int channelNoUIFlag,int allowTrickmodeFlag,int speed,int volume,int pipmode,void *mpobject,
	MediaplayerStateChangeCallback playstate_func)
{
	real_instanceid=instanceid;
	real_json_str=json_str;	
	real_playmode=playmode;
	real_videoDisplayMode=videoDisplayMode;
	real_videoDisplayrect.x=left;
	real_videoDisplayrect.y=top;
	real_videoDisplayrect.height=height;
	real_videoDisplayrect.width=width;
	real_muteflag=muteFlag;//0:  设置为有声 (默认值);1:  设置为静音
	real_nativeUIFlag=useNativeUIFlag;//0: 不使能Player的本地UI显示功能;1: 使能Player的本地UI显示功能 (默认值)
	real_muteUIFlag=muteuiflag;//: 不使能静音提示的本地UI显示功能;1: 使能静音提示的本地UI显示功能（默认值）	
	real_audioVolumeUIFlag=audioVolumeUIFlag;//0: 不使能音量调节的本地UI显示功能;1: 使能音量调节的本地UI显示功能（默认值）
	real_audioTrackUIFlag=audioTrackUIFlag;//0: 不使能音轨选择的本地UI显示功能;1: 使能音轨选择的本地UI显示功能（默认值）
	real_progressBarUIFlag=progressBarUIFlag;//0: 不使能进度条的本地UI显示功能;1: 使能进度条的本地UI显示功能（默认值）	
	real_channelNoUIFlag=channelNoUIFlag;//0: 不使能频道号的本地UI显示功能;1: 使能频道号的本地UI显示功能（默认值）
	real_subtitileFlag=subtitleFlag;//0:	不显示字幕 (默认值);   1:  显示字幕
	real_videoAlpha=videoAlpha;//0－100之间的整数值，0表示不透明，100表示完全透明(默认值为0)
	real_allowTrickmodeFlag=allowTrickmodeFlag;//0－100之间的整数值，0表示不透明，100表示完全透明。(默认值为0)
	real_cycleFlag=cycleFlag;	
	real_randomFlag=randomFlag;
	real_autoDelFlag=autoDelFlag;
	real_speed=speed;//-32,-16,-8,-4,-2,0,...32	
	real_volume=volume;//0-100，表示音量，0为静音，100为最大音量
	real_pipmode=pipmode;
	real_mpobject = mpobject;
	real_playstate_func=playstate_func;
	trans_time=0;
	media_start_time = 0;

	//getmediaurlfromjson();
	msgid = msgget(MEDIAPLAYER_QUEUE_KEY,IPC_CREAT|0666);
	Init_StreamMediaPlayer();
	mediaplayer_thread = 0;
	duration = 0;
	volumesize=volume;
	mspeed=1;
	if(real_videoDisplayMode==1||
		(real_videoDisplayrect.height==DFB_OSD_HEIGHT&&real_videoDisplayrect.width==DFB_OSD_WIDTH))
		iftvfullscreen=1;
	else
		iftvfullscreen=0;
	session_starttime = time(NULL);
	printf("8iftvfullscreen is %d\n",iftvfullscreen);
	printf("msgid is %d\n",msgid);

}

StreamMediaPlayer::~StreamMediaPlayer()
{
	int ret;
	printf("~StreamMediaPlayer\n\n");
	if(mediaplayer_thread)
	{
		sendto_mediaplayermsg(STREAMMEDIAPLAYER_QUIT);
		pthread_join(mediaplayer_thread, NULL);
		mediaplayer_thread =0;
	}
	
	Release_StreamMediaPlayer();
	msgctl(msgid,IPC_RMID,NULL);
	//DSP_graphics_unSetScreen();

	printf("~StreamMediaPlayer done\n\n");
}

void StreamMediaPlayer::player_setvolume(int volumeindex)
{	
	volumesize = volumeindex;
	real_volume=volumeindex;
	printf("StreamMediaPlayer,volume is %d\n",volumesize);
	sendto_mediaplayermsg(STREAMMEDIAPLAYER_VOLUMECHANGE);
	return;
}

int StreamMediaPlayer::joinChannel(int userchannelid, string& json_str)  //直播接口
{	
	real_json_str = json_str;
	cout<<"real_json_str:\n"<<real_json_str<<endl;
	//cout<<"json_str:\n"<<json_str<<endl;

	channel = userchannelid;
	
	if(getchannelurlfromjson())
	{
		printf("invalied json!\n\n");        //直播节目无授权或json格式错误
		return -1;
	}

	stype = MEDIA_TVPLAY;
	if(mediaplayer_thread == 0)
	{	 
		printf("thread just create\n");
		pthread_create(&mediaplayer_thread,NULL,( void *(*)(void*))StreamMediaPlayer::stream_player_thread,this);
	}
	else
	{
		printf("thread already create\n");
		sendto_mediaplayermsg(STREAMMEDIAPLAYER_URLCHANGE);
	}
	return 0;
}

int StreamMediaPlayer::leaveChannel()
{
	if(mediaplayer_thread)
	{		
		printf("StreamMediaPlayer leaveChannel!\n");
		sendto_mediaplayermsg(STREAMMEDIAPLAYER_QUIT);
		pthread_join(mediaplayer_thread, NULL);
		mediaplayer_thread = 0;
		if (real_playstate_func)
			(*real_playstate_func)(real_mpobject, UDP_STATUS_END_OF_STREAM, NULL);
	}
	return 0;
}


void StreamMediaPlayer::setAllowTrickmodeFlag (int allowTrickmode)
{
	sallowTrickmode = allowTrickmode;
}

 int StreamMediaPlayer::playFromStart()
 {
 	//playstarttime = 0;
	
	trans_time=0;	
	pause_into_tvs = backward_into_tvs = PAUSE_INIT;
	if_sm_setup = 0;
	cout<<"xuc: StreamMediaPlayer::playFromStart() entered"<<endl;
	cout<<"playFromStart: real_json_str"<<real_json_str<<endl;

	playing_time = media_start_time;
	playerstate = PLAYERSTATE_PREINIT;

	cout<<"xuc mediaplayer_thread = "<<mediaplayer_thread<<endl;

	if(mediaplayer_thread == 0)
	{	
		getmediaurlfromjson();
		session_starttime = time(NULL);
		printf("playFromStart create mediaplayer thread\n");
		pthread_create(&mediaplayer_thread,NULL,( void *(*)(void*))StreamMediaPlayer::stream_player_thread,this);
	}
	else
	{
		if(stype == MEDIA_TVPLAY)
		{
			sendto_mediaplayermsg(STREAMMEDIAPLAYER_REALTIMETV);
		}
		else
			sendto_mediaplayermsg(STREAMMEDIAPLAYER_SEEK);
	}
	return 0;
 }

int StreamMediaPlayer::playByTime(int type, int timestamp,int speed)
{
	//playstarttime = timestamp;
	cout<<"playByTime"<<endl;

	playspeed = speed;
	if(mediaplayer_thread == 0)
	{	 	
		getmediaurlfromjson();
		pthread_create(&mediaplayer_thread,NULL,( void *(*)(void*))StreamMediaPlayer::stream_player_thread,this);
	}
	else
	{
		sendto_mediaplayermsg(STREAMMEDIAPLAYER_SEEK);
	}
	return 0;
}

int StreamMediaPlayer::pause()
{
	int timeout = 2*VCR_TIMEOUT;
	trick_ok_flag = 0;
	printf("StreamMediaPlayer::pause\n");

	sendto_mediaplayermsg(STREAMMEDIAPLAYER_PAUSE);
	if(pause_into_tvs == PAUSE_INIT)
	{
		printf("StreamMediaPlayer::PAUSE firsttime, set PAUSE_TRIG\n");
		pause_into_tvs = PAUSE_TRIG;			//所有业务的第一次PAUSE
	
		while(!trick_ok_flag && timeout--) 
		{
			usleep(1000);
		}
		printf("StreamMediaPlayer::PAUSE firsttime return\n");
		return 0;
	}
	else
		pause_into_tvs = PAUSE_OTHERS;
	
	timeout = VCR_TIMEOUT;
	while(!trick_ok_flag && timeout--)  /* using timeout in case dead lock */
	{
		usleep(1000);
	}
	printf("StreamMediaPlayer::pause return\n");
	return 0;
}

int StreamMediaPlayer::fastForward(int speed)
{
	int timeout = VCR_TIMEOUT;
	printf("streamMediaPlayer::fastForward,speed is %d\n",speed);
	playspeed = speed;
	sendto_mediaplayermsg(STREAMMEDIAPLAYER_FASTFORWARD);
	trick_ok_flag=0;
	while(!trick_ok_flag && timeout--)  /* using timeout in case dead lock */
	{
		usleep(1000);
	}
	return 0;
}

int StreamMediaPlayer::fastRewind(int speed)
{
	int timeout = VCR_TIMEOUT;
	printf("streamMediaPlayer::fastRewind,speed is %d\n",speed);
	playspeed = speed;
	sendto_mediaplayermsg(STREAMMEDIAPLAYER_FASTBACKWARD);
	backward_into_tvs = 1;
	pause_into_tvs = PAUSE_OTHERS;    //backward_into_tvs情况下不触发pause_into_tvs
	trick_ok_flag=0;
	while(!trick_ok_flag && timeout--)  /* using timeout in case dead lock */
	{
		usleep(1000);
	}
	return 0;
}

int StreamMediaPlayer::resume()
{
	//playspeed = 1;
	int timeout = VCR_TIMEOUT;
	printf("StreamMediaPlayer::resume\n");
	sendto_mediaplayermsg(STREAMMEDIAPLAYER_PLAY);
	trick_ok_flag=0;
	while(!trick_ok_flag && timeout--) /* using timeout in case dead lock */
	{
		usleep(1000);
	}
	return 0;
}

int StreamMediaPlayer::update_playtime(unsigned long newplaytime)
{/*
	struct timeval tv;
	struct timezone tz;
	playing_time = newplaytime;
	gettimeofday (&tv , &tz);
	playing_time_last_update_sec = tv.tv_sec;
	playing_time_last_update_usec = tv.tv_usec;
	*/
	return 0;
}

void StreamMediaPlayer::update_play_pts(void)
{
	int ret;
	HI_UNF_AVPLAY_STATUS_INFO_S pstStatusInfo;
	ret = HI_UNF_AVPLAY_GetStatusInfo(havplay, &pstStatusInfo);
	if (ret == HI_SUCCESS)
	{
		printf("pts first %u, last %u\n", pstStatusInfo.stSyncStatus.u32FirstVidPts, pstStatusInfo.stSyncStatus.u32LastVidPts);
	
		cout<<"BufSize="<<pstStatusInfo.stBufStatus[0].u32BufSize<<endl;
		cout<<"UsedSize="<<pstStatusInfo.stBufStatus[0].u32UsedSize<<endl;
		cout<<"BufRptr="<<pstStatusInfo.stBufStatus[0].u32BufRptr<<endl;
		cout<<"BufWptr="<<pstStatusInfo.stBufStatus[0].u32BufWptr<<endl;	
		
		cout<<"VidFrameCount="<<pstStatusInfo.u32VidFrameCount<<endl;
		cout<<"AuddFrameCount="<<pstStatusInfo.u32AuddFrameCount<<endl;
		cout<<"VidErrorFrameCount="<<pstStatusInfo.u32VidErrorFrameCount<<endl;

		if (pstStatusInfo.stSyncStatus.u32FirstVidPts != ~0) /* valid pts value */
		{
			if (play_first_pts == ~0 && pstStatusInfo.stSyncStatus.u32FirstVidPts != 0)
			{
				play_first_pts = pstStatusInfo.stSyncStatus.u32FirstVidPts;
				play_last_pts = pstStatusInfo.stSyncStatus.u32LastVidPts;
			}
			else
			{
				if (pstStatusInfo.stSyncStatus.u32LastVidPts - play_last_pts <= PTS_MAX_OFFSET || play_last_pts - pstStatusInfo.stSyncStatus.u32LastVidPts <= PTS_MAX_OFFSET)
					play_last_pts = pstStatusInfo.stSyncStatus.u32LastVidPts;
			}
		}
	}
}

int StreamMediaPlayer::gotoEnd()
{
	return 0;
}

int StreamMediaPlayer::gotoStart()
{
	return 0;
}

int StreamMediaPlayer::stop()
{
	printf("StreamMediaPlayer::stop\n");	
	printf("now time %ld starttime %ld\n",time(NULL),session_starttime);
	/*if(time(NULL) - session_starttime <5)
	{
		printf("neglect this value\n");
		return 0;
	}*/
	
	if (mediaplayer_thread)
	{
		printf("sendto_mediaplayermsg STREAMMEDIAPLAYER_QUIT\n");
		sendto_mediaplayermsg(STREAMMEDIAPLAYER_QUIT);
		pthread_join(mediaplayer_thread, NULL);
		mediaplayer_thread = 0;
	}
	if (real_playstate_func)
		(*real_playstate_func)(real_mpobject, RTSP_STATUS_END_OF_STREAM, NULL);
	printf("stop out\n");
	return 0;
}

int StreamMediaPlayer::setSingleMedia(string mediaStr)   //点播、时移接口
{
	cout<<"xuc: StreamMediaPlayer::setSingleMedia() entered"<<endl;

	real_json_str = mediaStr;
	getmediaurlfromjson();
	
	cout<<"xuc mediaplayer_thread = "<<mediaplayer_thread<<endl;

	if(mediaplayer_thread == 0)
	{	
		stype= MEDIA_VODPLAY;
		session_starttime = time(NULL);
		getmediaurlfromjson();
		printf("setSingleMedia create mediaplayer thread\n");
		pthread_create(&mediaplayer_thread,NULL,( void *(*)(void*))StreamMediaPlayer::stream_player_thread,this);
	}
	else	
	{
		sendto_mediaplayermsg(STREAMMEDIAPLAYER_URLCHANGE);
	}
	return 0;
}

int StreamMediaPlayer::player_setwindowparms(int top, int left, int width, int height)
{
	printf("real_videoDisplayMode %d\n",real_videoDisplayMode);
	if(real_videoDisplayMode == 1)//全屏显示
	{
		real_videoDisplayrect.x = 0;
		real_videoDisplayrect.y = 0;
		real_videoDisplayrect.width = DFB_OSD_WIDTH;
		real_videoDisplayrect.height = DFB_OSD_HEIGHT;
		iftvfullscreen=1;
	}
	else
	{
		real_videoDisplayrect.x = top;
		real_videoDisplayrect.y = left;
		real_videoDisplayrect.width = width;
		real_videoDisplayrect.height = height;
		if(height==DFB_OSD_HEIGHT&&width==DFB_OSD_WIDTH)
			iftvfullscreen=1;
		else
			iftvfullscreen=0;
	}

	if(!top&&!left&&!width&&!height)
	{
		real_videoDisplayrect.x = 0;
		real_videoDisplayrect.y = 0;
		real_videoDisplayrect.width = DFB_OSD_WIDTH;
		real_videoDisplayrect.height = DFB_OSD_HEIGHT;
		iftvfullscreen=1;
	}
	printf("2iftvfullscreen is %d\n",iftvfullscreen);

	Set_Player_Window();
	return 0;
}

int StreamMediaPlayer::switchAudioChannel()
{
	return 0;	
}

int StreamMediaPlayer::switchAudioTrack()
{
	return 0;	
}

int StreamMediaPlayer::switchSubtitle()
{
	return 0;	
}

int StreamMediaPlayer::getMediaDuration()    
{
	printf("duration is %f\n",duration);
	return duration;
}
unsigned long StreamMediaPlayer::getCurrentPlayTime()
{
	unsigned int pts_offset;
	printf("here\n");
	/*if(if_sm_setup)
	{
		rtsp_streaming_server->rtspGETPARAMETER_gettime();
		playing_time = rtsp_streaming_server->playing_time;
	}
	printf("\n******\nplaying_time is %lu\n******\n",playing_time);
	return playing_time; */
	if (play_first_pts != ~0)
	{
		pts_offset = (play_last_pts - play_first_pts) / 1000;
		return pts_offset;
	}
	return 0;
}

char * StreamMediaPlayer::getPlaybackMode()
{
	
	cout<<"getPlaybackMode,mspeed is"<<mspeed<<endl;
	switch(playerstate)
	{
		case PLAYERSTATE_PAUSED:
			playbackmode = "{PlayMode:\"Pause\",Speed:1x}";
			break;
			
		case PLAYERSTATE_FASTBACKWARD:
			playbackmode = "{PlayMode:\"TrickMode\",Speed:";
			if(mspeed==-2)
				playbackmode.append("-2x}");
			else if(mspeed==-4)
				playbackmode.append("-4x}");
			else if(mspeed==-8)
				playbackmode.append("-8x}");
			else if(mspeed==-16)
				playbackmode.append("-16x}");
			else if(mspeed==-32)
				playbackmode.append("-32x}");
			else
				playbackmode.append("-32x}");
			break;
			
		case PLAYERSTATE_FASTFORWARD:
			playbackmode = "{PlayMode:\"TrickMode\",Speed:";
			if(mspeed==2)
				playbackmode.append("2x}");
			else if(mspeed==4)
				playbackmode.append("4x}");
			else if(mspeed==8)
				playbackmode.append("8x}");
			else if(mspeed==16)
				playbackmode.append("16x}");
			else if(mspeed==32)
				playbackmode.append("32x}");
			else
				playbackmode.append("32x}");
			break;
			
		default:
			playbackmode = "{PlayMode:\"Normal Play\",Speed:1x}";
			break;
	}

	cout<<"playback mode"<<playbackmode<<endl;
	return (char *)playbackmode.c_str();
	
}

int StreamMediaPlayer::GetLastError()
{
	return eventtype;
}


int StreamMediaPlayer::SetLastEvent(MediaPlayErrorType inerrortype)
{
	eventtype = inerrortype;
	return 0;
}

int StreamMediaPlayer::getChannelNum()
{
	return channel;
} 

int StreamMediaPlayer::Network_Init ()
{
	int ret;
	int i;
	bool connectionStatus;
			
	/*初始化网络接口*/
	/*现在的json传的均为会话管理服务器的ip.*/

      if(setup_type == SETUP_TVS ||setup_type == SETUP_TVOD)
      	{
		rtsp_session_server->media_start_time = media_start_time;
		rtsp_session_server->media_stop_time = media_stop_time;
		printf("rtsp_session_server->media_start_time = %ld\n",media_start_time);	
		printf("rtsp_session_server->media_stop_time = %ld\n",media_stop_time);	
	}
	
Redirect:
	rtsp_session_server->init();
	for (i = 0; i < ESTABLISH_TIMES; i++)
	{
		connectionStatus = rtsp_session_server->establishConnection();
		if (!connectionStatus)
		{
			printf("retry establish network %d\n", i+1);
			continue;
		}
		break;
	}
	if (!connectionStatus)
	{
		printf("connet %s:%d failed\n",rtsp_session_server->server_infor.ip,rtsp_session_server->server_infor.port);
		return -1;
	}
	for (i = 0; i < SETUP_TIMES; i++)
	{
		ret = rtsp_session_server->rtspSETUP(setup_type);
		if(ret == 0)
			break;
		if(ret == 302)
		{
			printf("Network_Init goto Redirect\n");			
			rtsp_session_server->destroyConnection(); /* close curren socket first */
			goto Redirect;
		}
		if (PlayerRecvStopKey() == 1)
		{
			printf("recv quit key, return -1, so go to cleanup\n");
			goto SM_SETUP_FAIL;
		}
	}
	if(ret != 0)
	{
		printf("rtsp_session_server->setup failed\n");
		goto SM_SETUP_FAIL;
	}
	
//xuc strip 2012.10.26
	memset(rtsp_streaming_server->ms_sessionID, 0 ,32);
	memset(rtsp_streaming_server->ms_GlobalSessionID, 0 ,32);
	rtsp_streaming_server->session_timeout = DEFAULT_MS_TIMEOUT;

	strcpy(rtsp_streaming_server->ms_GlobalSessionID, rtsp_session_server->ms_GlobalSessionID);
	strcpy(rtsp_streaming_server->ms_sessionID, rtsp_session_server->ms_sessionID);
	rtsp_streaming_server->session_timeout =  rtsp_session_server->ms_session_timeout;
	printf("Network_Init:ms_sessionID = %s\n", rtsp_streaming_server->ms_sessionID);
	printf("Network_Init:ms_GlobalSessionID = %s\n", rtsp_streaming_server->ms_GlobalSessionID);
	printf("Network_Init:ms_session_timeout =%d\n", rtsp_streaming_server->session_timeout);

	KeepAliveArgs_t sm_ka_args,ms_ka_args;
	sm_ka_args.pRTSPClient = rtsp_session_server;
	sm_ka_args.timeout = rtsp_session_server->session_timeout;
	ms_ka_args.pRTSPClient = rtsp_streaming_server;
	ms_ka_args.timeout = rtsp_streaming_server->session_timeout;
		
	ret = rtsp_session_server->ss_keep_alive_begin((void *)&sm_ka_args);
	if(ret != 0)
	{
		printf("rtsp_session_server->ss_keep_alive_begin failed\n");
		rtsp_session_server->rtspTEARDOWN();
		return -1;
	}
	if_sm_setup= 1;

	if (setup_type == SETUP_VOD || setup_type == SETUP_TVS||setup_type == SETUP_TVOD)
	{
		/*cp ms info*/
		strcpy(rtsp_streaming_server->server_infor.ip, rtsp_session_server->ms_ip);
		rtsp_streaming_server->server_infor.port = rtsp_session_server->ms_port;

		/*init ms rtsp*/
		rtsp_streaming_server->init();
		for (i = 0; i < ESTABLISH_TIMES; i++)
		{
			connectionStatus = rtsp_streaming_server->establishConnection();
			if (!connectionStatus)
			{
				printf("retry establish network %d\n", i+1);
				continue;
			}
			break;
		}
		if(!connectionStatus)
		{
			printf("rtsp_streaming_server->establishConnection failed\n");
			goto MS_FAIL;
		}
//		if_ms_connect = 1;

		ret = rtsp_streaming_server->ms_keep_alive_begin((void *)&ms_ka_args);
		if(ret != 0)
		{
			printf("rtsp_streaming_server->ms_keep_alive_begin failed\n");
			return -1;
		} 
	}


	return 0;
SM_SETUP_FAIL:
	rtsp_session_server->destroyConnection();
	return -1;

MS_FAIL:
	if (if_sm_setup)
	{	
		rtsp_session_server->ss_keep_alive_end();
		rtsp_session_server->rtspTEARDOWN();
		if_sm_setup =0;
	}
	return -1;
}

int StreamMediaPlayer::Network_Release ()
{
	int ret = -1;
	
	delete rtsp_session_server;
	delete rtsp_streaming_server;

	if(if_multicast)
		ret = Quit_Multicast();
	else 
		ret = Quit_Unicast();
	status_network = 0;
	return ret;
}


int StreamMediaPlayer::Init_StreamMediaPlayer()
{
	int ret = -1;
	int ser_grp = -1;

//	Init_JD_Handler();
	rtpheader_len = 0;
	first_show = 1;
	prebuf_max = 512*1024;
	volumesize= real_volume;

	nowtime = time_ptr =0;
	status_decoder = status_network =0;

//   每一个streamplayer需要实例化两个rtspclient。 
	rtsp_session_server = new RTSPClient_prv;
	rtsp_streaming_server = new RTSPClient_prv;
	
	if(!rtsp_session_server ||!rtsp_streaming_server)
	{
		printf("new rtsp_server failed\n");
		return -1;
	}
	
//申请隧道	
//初始化解码器
//从setting.xml读取userID
//读取service group
#if CCGW_IPC

	ret = request_to_open_channel();
	if(ret)
		printf("request_to_open_channel failed\n");
	else
		printf("request_to_open_channel success\n");
#endif

	 if(DSP_DVBplay_init(&havplay,&hwin))
	 {
		printf("DSP_DVBplay_init failed!ret =%d\n",ret);
		return -1;
	 }
	printf("DSP_DVBplay_init success, havplay = %d, hwin = %d\n", havplay,hwin);

	cur_freq= 0;

	printf("Read_Xml_Setting:\n");
	ret=Read_Xml_Setting((char *)SETTINGXMLNAME, (char *)ACCOUNTNAME, rtsp_session_server->userID);
	if(ret)
	{
	      printf("Read_Xml_Setting failed!!!!\n");
		return -1;
	}
	printf("xuc:userID = %s\n",rtsp_session_server->userID );

#if 1
	scan_sg_init(0, 0);
	ser_grp = scan_sg();
	//ser_grp = 12345;
	if(ser_grp == -1)
	{
		printf("get service group error!\n");   
// TODO:
	}	
	else
		rtsp_session_server->ServiceGroup = ser_grp;
	
	printf("get service group = %d\n",ser_grp);
#endif



	
//end
	status_decoder = 1;

//	net_conf_tv.packet_type = 1;

	//DSP_graphics_unSetBackGround();
	return 0;	

}

int StreamMediaPlayer::Release_StreamMediaPlayer()
{
	int ret;
	printf("Release_StreamMediaPlayerRelease_StreamMediaPlayer %d\n",status_decoder);
	Network_Release();

	if(status_decoder == 1)
	{
		DSP_DVBplay_close(havplay,hwin);
		status_decoder = 0;
	}

	/*xuc*/
	//DSP_graphics_resumeBackGround();
#if CCGW_IPC
	ret = request_to_close_channel();
	if(ret)
		printf("request_to_close_channel failed\n");
	else
		printf("request_to_close_channel success\n");
#endif

	return 0;
}

int StreamMediaPlayer::getchannelurlfromjson()
{
	size_t fstart, fend;
	size_t fecode,fport;
	string contentID;
	unsigned int port;
	//static int cnt = 0;
	
	cout<<"in get media url json_str:\n"<<real_json_str<<endl;
	//上层通过UserChannelID->EntitlementCode,一一对应
	/*xuc: json test*/
/*
	if(cnt%3==0)
	{
	real_json_str = "ChannelID=\"1\", ChannelName=\"Channel1\", UserChannelID=\"1\", ChannelURL=\"rtsp://192.168.10.60:554/?EntitlementCode=D199A258-4379-45E6-477F-3ED9FDB139A9\", TimeShift=\"1\", ChannelSDP=\"video=mpeg\", TimeShiftURL=\"\", ChannelLogURL=\"\", PositionX=\"\", PositionY=\"\", BeginTime=\"\", Interval=\"\", Lasting=\"\", ChannelType=\"1\", ChannelPurchased=\"1\"";
	}
	if(cnt%3 ==1)	
	{
	real_json_str = "ChannelID=\"2\", ChannelName=\"Channel2\", UserChannelID=\"2\", ChannelURL=\"rtsp://192.168.10.60:554/?EntitlementCode=D199A258-4379-45E6-477F-3ED9FDB139A0\", TimeShift=\"1\", ChannelSDP=\"video=mpeg\", TimeShiftURL=\"\", ChannelLogURL=\"\", PositionX=\"\", PositionY=\"\", BeginTime=\"\", Interval=\"\", Lasting=\"\", ChannelType=\"1\", ChannelPurchased=\"1\"";
	}
	if(cnt%3 ==2)	
	{
	real_json_str = "ChannelID=\"3\", ChannelName=\"Channel3\", UserChannelID=\"3\", ChannelURL=\"rtsp://192.168.10.60:554/?EntitlementCode=D199A258-4379-45E6-477F-3ED9FDB139A1\", TimeShift=\"1\", ChannelSDP=\"video=mpeg\", TimeShiftURL=\"\", ChannelLogURL=\"\", PositionX=\"\", PositionY=\"\", BeginTime=\"\", Interval=\"\", Lasting=\"\", ChannelType=\"1\", ChannelPurchased=\"1\"";
	}

	cout<<"in fake media url json_str"<<real_json_str<<endl;
	cnt++;

	*/
	/*end*/
	setup_type = SETUP_SDV;
	cout<<"getchannelurlfromjson\nsetup_type == SETUP_SDV"<<endl;		
	
	
	fstart = real_json_str.find("UserChannelID");
	if (fstart == string::npos)
		goto ERROR;
	fstart += strlen("UserChannelID=\"");
	fend=real_json_str.find("\"",fstart);	
	cout<<"UserChannelID: fend-fstart = "<<fend-fstart<<endl;    		
	if(fend-fstart == 0)							  //UserChannelID为空
			goto ERROR;	

	fstart=real_json_str.find("ChannelURL=\"");
	if (fstart == string::npos)
		goto ERROR;
	if (fstart!=string::npos)
	{	
		fstart += strlen("ChannelURL=\"");
		fend=real_json_str.find("\"",fstart);	

		cout<<"ChannelURL: fend-fstart = "<<fend-fstart<<endl;    		
		if(fend-fstart == 0)						  //ChannelURL为空
			goto ERROR;
		
		//ip,port,contentID
		
		fecode = real_json_str.find("EntitlementCode");
		fecode += strlen("EntitlementCode=");
		contentID = real_json_str.substr(fecode,fend-fecode);

		fport = real_json_str.find(":", fstart+5) +1;
		port = atoi(real_json_str.substr(fport,fecode-fport-2).c_str());		

		if (fend!=string::npos)
		{
			mediaurl = real_json_str.substr(fstart+7, fport-fstart-1-7);//minus rtsp://		
			cout<<"sdv json:\nURL="<<mediaurl<<endl;
			cout<<"port="<<port<<"\nEntitlementCode="<<contentID<<endl;

			//mediaurl = "192.168.10.158"; //会话管理服务器地址

			strcpy(rtsp_session_server->server_infor.ip, mediaurl.c_str()); //实际是会话管理服务器地址
			rtsp_session_server->server_infor.port = port;
			strcpy(rtsp_session_server->contentID, contentID.c_str());			
			return 0;
		}
	}

ERROR:
	printf("getchannelurlfromjson error!\n");
	return -1;
}

int StreamMediaPlayer::getmediaurlfromjson()
{
	size_t fstart, fend;
	size_t fecode,fport,flcs, flcs_end, fstarttime, fendtime,frate, frate_end;
	string tmp_url,contentID, license;
	unsigned int port;
	unsigned long bit_rate;
	unsigned long long json_start_time = 0;   //秒数+毫秒数
	unsigned long long json_stop_time = 0;   //秒数+毫秒数
	media_start_time = 0;
	cout<<"in get media url json_str"<<real_json_str<<endl;
	
	/*xuc: json test*/
	//string real_json_str("[{mediaUrl:\"rtsp://192.168.254.159:554/?EntitlementCode=D199A258-4379-45E6-477F-3ED9FDB139A9\", License:\"1234567\", mediaCode: \"code1\",mediaType:2,audioType:1,videoType:1,streamType:1,drmType:1,drmType:1,fingerPrint:0,copyProtection:1,allowTrickmode:1,startTime:0,endTime:5000,entryID:\"entry1\"}] ");
	//cout<<"in fake media url json_str"<<real_json_str<<endl;
	/*end*/

    //stype = MEDIA_VODPLAY;
	
	fstart=real_json_str.find("mediaUrl:\"");

	if (fstart!=string::npos)
	{	
		//ip, port, contentID, startTime, license

		fstart += strlen("mediaUrl:\"");
		fend = real_json_str.find("&startTime");
		if (fend == string::npos)                    //vod
		{
			cout<<"get no &startTime"<<endl;		
			cout<<"setup_type == SETUP_VOD"<<endl;			
			setup_type = SETUP_VOD;
			pause_into_tvs = 2;              //不需要特殊处理
			fend=real_json_str.find("\"",fstart);
			media_start_time = 0;
			
			fstarttime = real_json_str.find("endTime");
			if (fstarttime == string::npos)
			{
				cout<<"get no endTime!"<<endl;
				goto ERROR;
			}
			fstarttime += strlen("endTime:");	
			fendtime = real_json_str.find(",", fstarttime);
			json_stop_time = atoll(real_json_str.substr(fstarttime,fendtime-fstarttime).c_str());
			media_stop_time = json_stop_time/1000;
			/*毫秒数未处理*/

		 }
		else 					              //tvs/tvod
		{
			cout<<"get &startTime"<<endl;		
			//解析时移绝对时间,注意，均是寻找第一个startime/endtime
			fstarttime = fend + strlen("&startTime=");		
			fendtime = real_json_str.find("\"",fstart);
			json_start_time = atoll(real_json_str.substr(fstarttime,fendtime-fstarttime).c_str());
			media_start_time = json_start_time/1000;			
			/*毫秒数未处理*/
			
			fstarttime = real_json_str.find("endTime");
			if (fstarttime == string::npos)    //总应找到endTime. tvs时endTime=0，tvod时!=0
			{
				cout<<"get no endTime!"<<endl;
				goto ERROR;
			}
			cout<<"get endTime!"<<endl;
			fstarttime += strlen("endTime:");	
			fendtime = real_json_str.find(",", fstarttime);	
			json_stop_time = atoll(real_json_str.substr(fstarttime,fendtime-fstarttime).c_str());
			media_stop_time = json_stop_time/1000;
			
			if(media_stop_time == 0)
			{
				setup_type = SETUP_TVS;
				cout<<"setup_type == SETUP_TVS"<<endl;			
			}
			else
			{
				setup_type = SETUP_TVOD;
				cout<<"setup_type == SETUP_TVOD"<<endl;			
			}

			/*毫秒数未处理*/
			
		}

		fecode = real_json_str.find("EntitlementCode");
		fecode += strlen("EntitlementCode=");
		contentID = real_json_str.substr(fecode,fend-fecode);

		flcs = real_json_str.find("License");
		if (setup_type == SETUP_VOD)
		{
			if(flcs != string::npos)
			{
				flcs += strlen("License:\"");
				flcs_end = real_json_str.find("\"",flcs);
				license = real_json_str.substr(flcs,flcs_end-flcs);
			}
			else
			{		
				cout<<"vod, can not find license!"<<endl;
				goto ERROR;
			}
		}
		else
		{
			license = "0";                       //否则均是填0代表不需解扰
		}
		
		frate = real_json_str.find("bit_rate");
		if (setup_type == SETUP_VOD)
		{
			if(frate != string::npos)
			{
				frate += strlen("bit_rate:\"");
				frate_end = real_json_str.find("\"",frate);
				bit_rate = atol(real_json_str.substr(frate,frate_end-frate).c_str());
				cout<<"vod, bit_rate = "<<bit_rate<<endl;
			}
			else
			{		
				cout<<"vod, bit_rate not found!"<<endl;
				bit_rate = 0;  
			}
		}
		else
		{
			bit_rate = 0;                       //否则均是填0，代表不需要传输节目带宽
		}	

		fport = real_json_str.find(":", fstart+5) +1;
		port = atoi(real_json_str.substr(fport,fecode-fport-2).c_str());

		//printf("%d, %d, %d, %d\n",fstart, fend, fecode, fport);
		if (fend!=string::npos)
		{

			mediaurl = real_json_str.substr(fstart+7,fport-fstart-1-7); //minus rtsp://
			vodurl = mediaurl;	

			cout<<"vodurl="<<vodurl<<endl;
			cout<<"port="<<port<<"\nEntitlementCode="<<contentID<<endl;
			cout<<"license="<<license<<endl;
			cout<<"json_start_time="<<json_start_time<<endl;			
			cout<<"json_stop_time="<<json_stop_time<<endl;				
			cout<<"media_start_time="<<media_start_time<<endl;		
			cout<<"media_stop_time="<<media_stop_time<<endl;		

//test!!!
		//	vodurl = "192.168.10.120"; 
	//		port = 5542;
			//	media_start_time -= 8*3600 ;
			//media_start_time = 1314049400;
			//contentID = "F271102462716324";
			//media_start_time = 1315215551;
	
			strcpy(rtsp_session_server->server_infor.ip, vodurl.c_str()); //实际是会话管理服务器地址
			rtsp_session_server->server_infor.port = port;
			strcpy(rtsp_session_server->contentID, contentID.c_str());
			strcpy(rtsp_session_server->license, license.c_str());
			rtsp_session_server->bit_rate = bit_rate;
			return 0;
		}
	}

ERROR:
	printf("getmediaurlfromjson error!\n");
	return -1;

}


//处理组播时的消息
void StreamMediaPlayer::process_multicast_msg(int cmd, PLAYERProcess_cmd_goto *gotocmd)
{			
	int ret;
	struct timeval tv;
//xuc: 查询心跳是否正常
	if(rtsp_session_server->if_keep_alive_error)
	{
		printf("process_multicast_msg: keep alive error!\n");
		cmd = STREAMMEDIAPLAYER_QUIT;
	}
//end

	switch(cmd)																	 
	{																			 
		case STREAMMEDIAPLAYER_QUIT:
			printf("got quit msg in multicast\n");

			rt_value = RET_PLAYER_QUIT;
			*gotocmd = PLAYERProcess_cmd_goto_cleanup;
			break;	

		case STREAMMEDIAPLAYER_URLCHANGE:	

			printf("STREAMMEDIAPLAYER_URLCHANGE\n\n");
			Network_Release();
			DSP_TSPlayer_Pause(TSPlayerHandle);			
			ret = Network_Init();	
			printf("STREAMMEDIAPLAYER_URLCHANGE222\n\n");

			if(!if_multicast)
			{															
				//initrecv_session(session);
			}
			*gotocmd = PLAYERProcess_cmd_goto_mainloop;
			break;		

		default:
			trick_ok_flag=1;                  //防止tvs失败返回sdv时pause挂起
			break;

	}																			 
}

//处理单播时的消息
void StreamMediaPlayer::process_unicast_msg(int cmd, PLAYERProcess_cmd_goto *gotocmd)
{	
	int ret;
//xuc: 先查询是否由接收ANNOUCE 改变了状态
	switch(rtsp_streaming_server->player_status)
	{
		case PLAYER_PLAY:
			cout<<"playerstate change to PLAY"<<endl;
			playerstate = PLAYERSTATE_PLAYING;
		       cmd = STREAMMEDIAPLAYER_PLAY;
			if (real_playstate_func)
				(*real_playstate_func)(real_mpobject,EVENT_BEGIN_OF_STREAM, NULL);
			 rtsp_streaming_server->player_status = PLAYER_NONE;
			break;
		case PLAYER_QUIT:
			cout<<"playerstate change to EOS, goto cleanup!"<<endl;
			cmd = STREAMMEDIAPLAYER_QUIT;
			rtsp_streaming_server->player_status = PLAYER_NONE;
			break;
		default:
			break;
	}

	//TVS的第一次不是由快退进入的PAUSE , 调用tplay
	if(pause_into_tvs == PAUSE_TRIG && setup_type == SETUP_TVS)  
	{
		cout<<"\nPAUSE INTO TVS.\n"<<endl;
		pause_into_tvs = PAUSE_OTHERS;
		cmd = STREAMMEDIAPLAYER_PAUSE;

		ret = DSP_DVBplay_tplay(havplay);
		if(ret)
		{
				printf("DSP_DVBplay_tplay failed!\n\n\n\n");
				return;
		}
	}
	else
		pause_into_tvs = PAUSE_OTHERS;        

	if(backward_into_tvs)
	{
		backward_into_tvs = 0;
		cmd = STREAMMEDIAPLAYER_FASTBACKWARD;
	}
	
	if(rtsp_session_server->if_keep_alive_error || rtsp_streaming_server->if_keep_alive_error)
	{
		printf("process_unicast_msg: keep alive error!\n");
		cmd = STREAMMEDIAPLAYER_QUIT;
	}
	
//end	
	switch(cmd)																    
	{																		    
		case STREAMMEDIAPLAYER_QUIT:         //退出
			printf("STREAMMEDIAPLAYER_QUIT\n");
			rt_value = RET_PLAYER_QUIT;
			*gotocmd = PLAYERProcess_cmd_goto_cleanup;
			break;

		case  STREAMMEDIAPLAYER_PAUSE:		//暂停												    
			printf("STREAMMEDIAPLAYER_PAUSE\n");
			if(rtsp_streaming_server->rtspPAUSE())
			{
				if(rtsp_streaming_server->rtspPAUSE())
					printf("VCR FAILED\n");
				//*gotocmd = PLAYERProcess_cmd_goto_cleanup;		
			}			

			//xuc 20120704 第一次失败，第二次成功时，
			//也需要设置状态。
			playerstate = PLAYERSTATE_PAUSED;		
			mspeed=1;
			/*
			else
			{
				playerstate = PLAYERSTATE_PAUSED;		
				mspeed=1;
			}
			*/
			printf("STREAMMEDIAPLAYER_PAUSE done\n");
			trick_ok_flag=1;
			break;
		case  STREAMMEDIAPLAYER_PLAY:						                                                               
			cout<<"STREAMMEDIAPLAYER_PLAY play time is "<<playing_time<<endl;
			if(playerstate == PLAYERSTATE_PAUSED)
			{
				printf("STREAMMEDIAPLAYER_PLAY!\n");
				if(rtsp_streaming_server->rtspPLAY(-1) )                                            
				{
					if(rtsp_streaming_server->rtspPLAY(-1) )                                      
						printf("VCR FAILED\n");
				//	*gotocmd = PLAYERProcess_cmd_goto_cleanup;	
				//	break;
				}	
				//*gotocmd = PLAYERProcess_cmd_goto_rstart;//恢复播放时要rstart
			}
			else if((playerstate == PLAYERSTATE_FASTBACKWARD)||(playerstate == PLAYERSTATE_FASTFORWARD))
			{
				printf("STREAMMEDIAPLAYER_PAUSE and PLAY!\n");
				if(rtsp_streaming_server->rtspPAUSE())
				{
					if(rtsp_streaming_server->rtspPAUSE())
						printf("VCR FAILED\n");
					//*gotocmd = PLAYERProcess_cmd_goto_cleanup;
					//break;
				}	
				cout<<"after stop playing time is "<<playing_time<<endl;
				if(rtsp_streaming_server->rtspPLAY(-1))
				{
					if(rtsp_streaming_server->rtspPLAY(-1))
						printf("VCR FAILED\n");				
				//	*gotocmd = PLAYERProcess_cmd_goto_cleanup;	
				//	break;
				}
				
				trans_time=0;
				//*gotocmd = PLAYERProcess_cmd_goto_rstart;
			}	
			else
				printf("WRONG playerstate\n");
			
			//2012.4.10 xuc  VCR关闭声音
			if( (real_volume>MAX_VOLUME_SIZE)||(real_volume<MIN_VOLUME_SIZE))
				printf("error: volume out of range!\n");
			DSP_DVBplay_setvolume(real_volume);
			printf("resume volume = %d\n", real_volume);
			
			*gotocmd = PLAYERProcess_cmd_goto_rstart;//恢复播放时要rstart
			printf("STREAMMEDIAPLAYER_PLAY done\n");
			trick_ok_flag=1;
			playerstate = PLAYERSTATE_PLAYING;
			mspeed=1;
			playspeed=1;
			break;	

		case STREAMMEDIAPLAYER_URLCHANGE:			//url重定向，不使用											 								 
			printf("STREAMMEDIAPLAYER_URLCHANGE\n");
			Network_Release();															 
			DSP_TSPlayer_Pause(TSPlayerHandle);			
			Network_Init();	
			//if(rtsp_streaming_server->rtspPLAY(media_start_time) )                      
			if(0)
			{
				rt_value = RET_PLAYER_LOCAL;
				*gotocmd = PLAYERProcess_cmd_goto_cleanup;	                                                                                         
			}	
			else
			{
				trans_time=0;
				//initrecv_session(session);
				*gotocmd = PLAYERProcess_cmd_goto_rstart;
			}
			
			break;

		case STREAMMEDIAPLAYER_REALTIMETV:	             //切换到实时节目，不使用												
			printf("STREAMMEDIAPLAYER_REALTIMETV\n");
//			UI_JD_display(SHOW_NOTICE_INFO, 0, (char *)"实时节目");     				
			Quit_Unicast();															
			Init_Multicast();															
			playerstate = PLAYERSTATE_PLAYING;	
			mspeed=1;
			if_multicast = 1;			
			*gotocmd = PLAYERProcess_cmd_goto_rstart;	
			break;

		case STREAMMEDIAPLAYER_FASTBACKWARD: 
			printf("STREAMMEDIAPLAYER_FASTBACKWARD\n");
			cout<<"backward playspeed is "<<playspeed<<endl;
			cout<<"playing_time is "<<playing_time<<endl;
#if 1		
			ret = rtsp_streaming_server->rtspPAUSE();
			if(ret)
			{	
				if(rtsp_streaming_server->rtspPAUSE());
				printf("VCR FAILED,ret = %d\n",ret);				
				//*gotocmd = PLAYERProcess_cmd_goto_cleanup;	
				//break;
			}
#endif
			if(rtsp_streaming_server->rtsp_backward(playspeed)) 
			{	
				if(rtsp_streaming_server->rtsp_backward(playspeed)) 
				printf("VCR FAILED\n");
			//	*gotocmd = PLAYERProcess_cmd_goto_cleanup;	
				//break;
			}	
			
			//xuc 20120704 第一次失败，第二次成功时，
			//也需要设置状态。
			playerstate = PLAYERSTATE_FASTBACKWARD;
			mspeed=playspeed;
			*gotocmd = PLAYERProcess_cmd_goto_rstart;	
			/*
			else
			{
				playerstate = PLAYERSTATE_FASTBACKWARD;
				mspeed=playspeed;
				*gotocmd = PLAYERProcess_cmd_goto_rstart;	
			}
			*/
			//2012.4.10 xuc  VCR关闭声音
			if( (real_volume>MAX_VOLUME_SIZE)||(real_volume<MIN_VOLUME_SIZE))
				printf("error: volume out of range!\n");
			DSP_DVBplay_setvolume(0);
			printf("set volume 0 from %d\n", real_volume);
			
			printf("STREAMMEDIAPLAYER_FASTBACKWARD done\n");
			trick_ok_flag=1;
			break;	

		case STREAMMEDIAPLAYER_FASTFORWARD: 
			printf("STREAMMEDIAPLAYER_FASTFORWARD\n");
			cout<<"forward playspeed, "<<playspeed<<",playing_time is "<<playing_time<<endl;

			//if(rtspclient.rtspVcrRequest(playspeed + SPEED_OFFSET,playing_time))  
			//rtsp_streaming_server->rtsp_jump(20.0);
#if 1
			ret = rtsp_streaming_server->rtspPAUSE();
			if(ret)
			{	
				if(rtsp_streaming_server->rtspPAUSE())
				printf("VCR FAILED,ret = %d\n",ret);				
			//	*gotocmd = PLAYERProcess_cmd_goto_cleanup;	
			//	break;
			}
#endif	
			if(rtsp_streaming_server->rtsp_forward(playspeed)) 
			{	
				if(rtsp_streaming_server->rtsp_forward(playspeed))
				printf("VCR FAILED\n");			
			//	*gotocmd = PLAYERProcess_cmd_goto_cleanup;	
			//	break;				
			}	
			
			//xuc 20120704 第一次失败，第二次成功时，
			//也需要设置状态
			
			playerstate = PLAYERSTATE_FASTFORWARD;
			mspeed=playspeed;
			*gotocmd = PLAYERProcess_cmd_goto_rstart;	
			/*
			else
			{
				playerstate = PLAYERSTATE_FASTFORWARD;
				mspeed=playspeed;
				*gotocmd = PLAYERProcess_cmd_goto_rstart;	
			}*/
			
			//2012.4.10 xuc  VCR关闭声音
			if( (real_volume>MAX_VOLUME_SIZE)||(real_volume<MIN_VOLUME_SIZE))
				printf("error: volume out of range!\n");
			DSP_DVBplay_setvolume(0);
			printf("set volume 0 from %d\n", real_volume);
				
			printf("STREAMMEDIAPLAYER_FASTFORWARD done\n");
			trick_ok_flag=1;
			break;														 
	}	

	return;
}
//处理消息接口
PLAYERProcess_cmd_goto StreamMediaPlayer::process_tvod_msg()
{
	PLAYERProcess_cmd_goto gotocmd = PlAYERProcess_cmd_goto_none;
	int  cmd = STREAMMEDIAPLAYER_NONE;
	int processed =0;
	//	int getkey =0;	//是否需要处理键值

	//处理通用消息，
	process_common_msg(&cmd, &processed);   

	if(processed)
		return gotocmd;

	if(setup_type == SETUP_SDV)			                                                							  
	{												      							  
		process_multicast_msg(cmd, &gotocmd);																						 

	}																					 
	else	if(setup_type == SETUP_VOD ||setup_type == SETUP_TVOD ||setup_type == SETUP_TVS)																				 
	{																					 
		process_unicast_msg(cmd, &gotocmd);
	}		
	else
	{
		printf("process_msg: setup_type error!\n");
	}

	return gotocmd;
}

int StreamMediaPlayer::Read(unsigned char * buf, unsigned long len )
{
	int count;

	/*初始化网络接口*/
	if(if_multicast)
		count = TVRead(buf, len );
	else 
		count = VODRead(buf, len );

	return count;
}

void StreamMediaPlayer::main_stream_player()
{	
	//unsigned long playing_time=0,playing_time_ptr=0;
//	unsigned long playing_time_ptr=0;
	//SDSPFP_RETVALUE dspfp_rt_value;
	//int reset_black = 0;
//	unsigned char *databuf;
	//unsigned int bufsize;
//	int state_signal;
//	int need_resetdemuxinfo=0;
	//int type = TVOD_T;
//	time_t basetime,readdata_basetime;

	int ret;
	//int firsttime;
	int search_retry_times = MAX_SEARCH_RETRY_TIMES;	
	PLAYERProcess_cmd_goto gotocmd;

	//struct timeval now;	
	//static struct timeval last;	
	//long elapsed; 	

	//printf("sdv json:\n%s\n%d\n%s\n",rtsp_session_server->server_infor.ip,rtsp_session_server->server_infor.port, rtsp_session_server->contentID);
	//xuc: 通过json判断播放的类型，移到解析json时

	ClearDummyKey();
	printf("sdv json test:\n%s\n%d\n%s\n%ld\n",rtsp_session_server->server_infor.ip,rtsp_session_server->server_infor.port, rtsp_session_server->contentID,media_start_time);

	//goto main_loop;     
	status_network = Network_Init();
	if(status_network)
	{
		//SetLastEvent(RTSP_CONNECT_FAILED);
		printf("main_stream_player: Network_Init()==-1 !\n");		
		mediaplayer_thread = 0; 	   // 2012.03.26  thread ID可能!=0
		if (real_playstate_func)
			(*real_playstate_func)(real_mpobject, EVENT_PLAYER_EXIT, NULL);
		return;
	}
	printf("Network_Init success!\n");

	/*ret=DSP_TSPlayer_SetVolumeIndex(TSPlayerHandle, volumesize*2);
	printf("before DSP_TSPlayer_SetVolumeIndex volumn is %d,ret is %d\n",volumesize,ret);
	printf("3iftvfullscreen is %d\n",iftvfullscreen);
	if(iftvfullscreen){
		//UI_JD_display(STATE_TV_CHANNEL, channel,NULL);		
		//UI_TVODLOG_disappear();
		//if(type == TVOD_T)
		UI_JD_display(SHOW_TVOD_LOG, 0, NULL);
	}*/
	
	//Set_Player_Window();   

	
	printf("xuc: dvbplay start!cur_freq = %d, frequency = %d\n",cur_freq,  rtsp_session_server->play_param.frequency);   
	if(cur_freq != rtsp_session_server->play_param.frequency)
	{
		 ret =  DSP_DVBplay_tune(rtsp_session_server->play_param.frequency);
		 if(ret)
		 {
			printf("DSP_DVBplay_tune failed!\n");
			rtsp_session_server->ss_keep_alive_end();
			rtsp_session_server->rtspTEARDOWN();
			cur_freq = 0;
			//测试页面对该消息的反应，暂时注释
			//SetLastEvent(RTSP_CONNECT_FAILED);
			mediaplayer_thread = 0; 	   // 2012.03.26  thread ID可能!=0
			if (real_playstate_func)
				(*real_playstate_func)(real_mpobject, EVENT_PLAYER_EXIT, NULL);
			return;
			
		 }		
		 printf("DSP_DVBplay_tune success!\n");
		cur_freq = rtsp_session_server->play_param.frequency;
	}

	//相同频点也要重新搜表
	while(search_retry_times--) 
	 {
		 ret = DSP_DVBplay_search(rtsp_session_server->play_param.pnID);	
		 if(!ret)
		 	break;
		 else
		 {
		 	if (PlayerRecvStopKey() == 1)
			{
				printf("recv quit key, break channel search, so go to cleanup\n");
				break;
			}
		 	usleep(200000);
	 	}	
	 }	
	 if(ret)
	 {
	 	printf("DSP_DVBplay_search failed!\n");
		rtsp_session_server->ss_keep_alive_end();
		rtsp_session_server->rtspTEARDOWN();
		//SetLastEvent(RTSP_CONNECT_FAILED);
		mediaplayer_thread = 0; 	   //  2012.03.26  thread ID可能!=0
		if (real_playstate_func)
			(*real_playstate_func)(real_mpobject, EVENT_PLAYER_EXIT, NULL);
		return;
	 } 
	 printf("DSP_DVBplay_search success!\n");
	if( (real_volume>MAX_VOLUME_SIZE)||(real_volume<MIN_VOLUME_SIZE))
		printf("volume out of range\n");
	if (real_muteflag)
		ret = DSP_DVBplay_setvolume(0);
	else
		ret = DSP_DVBplay_setvolume(real_volume);

restart:
	 ret = DSP_DVBplay(havplay,hwin,rtsp_session_server->play_param.pnID, NULL);
	 printf("[DBG]DSP_DVBplay start, %d\n", havplay);

	  if(ret)
	 {
		printf("DSP_DVBplay failed!\n");
		goto cleanup;
	 }  
main_loop:

	printf("into while(1)!\n");
	while(1)
	{
	//	Poll_JD_Timerhandler();
		gotocmd = process_tvod_msg(); 
		PROCESS_MSG_RESULT(gotocmd);	//goto cleanup
		usleep(10000);
	}				

cleanup:
	printf("main_stream_player: go to cleanup !\n");	
	rtsp_session_server->ss_keep_alive_end();
	printf("if_sm_setup = %d\n", if_sm_setup);
	if(if_sm_setup)
		rtsp_session_server->rtspTEARDOWN();

	printf("[DBG]rtspTEARDOWN exit\n");

	//Network_Release();
	/*UI_JD_disappear(NULL);
	UI_TVODLOG_disappear();
	UI_JD_displaytime_disappear();*/
	
	printf("[DBG]DSP_DVBplay_stop start, %d\n", havplay);
	DSP_DVBplay_stop(havplay);
	printf("[DBG]DSP_DVBplay_stop exit\n");
	mediaplayer_thread = 0; 	   // 2012.03.26  线程退出但fd未变成0，导致判断错误

	return;
}

int StreamMediaPlayer::parse_cw_from_license(char *license, unsigned char *key)
{
#if USE_DRM
	int cryptMode;
	int fp_license, drm_i;
	unsigned char license_hex[DRM_LICENSE_LEN] = {0};
	unsigned char first, second;
	int ret;
	int ret_val = -1;

	if (strcmp(license, "0"))
	{
		fp_license = open(VOD_LICENSE_FILE, O_RDWR | O_CREAT);
		if (fp_license != -1)
		{
			for (drm_i = 0; drm_i < DRM_LICENSE_LEN; drm_i++)
			{
				if (license[drm_i  * 2] >= '0' && license[drm_i  * 2] <= '9')
					first = license[drm_i  * 2] - '0';
				else if (license[drm_i  * 2] >= 'A' && license[drm_i  * 2] <= 'Z')
					first = license[drm_i  * 2] - 'A' + 0xA;
				else
					first = license[drm_i  * 2] - 'a' + 0xA;
				if (license[drm_i  * 2 + 1] >= '0' && license[drm_i  * 2 + 1] <= '9')
					second = license[drm_i  * 2 + 1] - '0';
				else if (license[drm_i  * 2 + 1] >= 'A' && license[drm_i  * 2 + 1] <= 'Z')
					second = license[drm_i  * 2 + 1] - 'A' + 0xA;
				else
					second = license[drm_i  * 2 + 1] - 'a' + 0xA;
				license_hex[drm_i] = ((first & 0xF) << 4) | (second  & 0xF);
			}
			ret = write(fp_license, license_hex, DRM_LICENSE_LEN);
			close(fp_license);
			if (ret == -1)
				printf("write %s error\n", VOD_LICENSE_FILE);
			else
			{
				chmod(VOD_LICENSE_FILE, S_IRWXU | S_IRWXG | S_IRWXO);
				printf("write tmp vod license file ok\n");
				DRMCFuncInit();
				ret = DRMCFuncPreprocess(VOD_LICENSE_FILE);
				if (ret == 0)
				{
					ret = DRMCFuncGetFilmSecInfo(VOD_LICENSE_FILE, 0, &cryptMode, key);
					if (ret == 0)
					{
						printf("key : %02x %02x %02x %02x %02x %02x %02x %02x\n",
							key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]); 
						switch(cryptMode)
						{
							case 0:
								printf("no scramble\n");
								ret_val = VOD_NO_SCRAMBLE;
								break;
							case 1:
								printf("no DVB scramble, must be error\n");
								break;
							case 2:
								printf("DVB scramble\n");
								ret_val = VOD_SCRAMBLE;
								break;
							default:
								printf("unrecognized scramble type, %d\n", cryptMode);
								break;
						}
					}
					else
					{
						printf("DRMCFuncGetFilmSecInfo error = %d\n", ret);
					}
						
				}
				else
				{
					printf("DRMCFuncPreprocess error = %d\n", ret);
				}
				printf("after get get key, remove vod license file\n");
			}
		}
		else
		{
			printf("open %s error\n", VOD_LICENSE_FILE);
		}
	}
	else
	{
		printf("license = 0, no scramble\n");
		ret_val = VOD_NO_SCRAMBLE;
	}
	return ret_val;
#else
	return VOD_NO_SCRAMBLE;
#endif
}
void StreamMediaPlayer::main_vod_player()
{
	//int basetime,getpara_basetime=0;		
	//SDSPFP_RETVALUE dspfp_rt_value;
	PLAYERProcess_cmd_goto gotocmd;

//	unsigned int i = 0;		
	int ret;
//	int need_resetdemuxinfo=0;
//	unsigned long long tmptime_t;
//	uint8_t buffer[RTP_MAX_PACKET_LEN];

	/*add for monitor decoder*/
	//struct timeval now;	
	//static struct timeval last;	
	int search_retry_times = MAX_SEARCH_RETRY_TIMES;
	int check_stream_times = 100;
	//	int vbuffer_empty_times = MAX_VBUF_EMPTY_TIMES;
#if USE_DRM
	unsigned char key[16] = {0};
	int scramble;
#endif
	printf("main_vod_player start!\n");
	play_first_pts = ~0;
	ClearDummyKey();  

	printf("vod json test:\n%s\n%d\n%s\n%ld\n",rtsp_session_server->server_infor.ip,rtsp_session_server->server_infor.port, rtsp_session_server->contentID,media_start_time);

	status_network = Network_Init();
	if(status_network) //所有的init network
	{
		printf("main_vod_player: Network_Init()==-1\n");
		trick_ok_flag = 1;
		mediaplayer_thread = 0;        // 2012.03.26  thread ID可能!=0
		if (real_playstate_func)
			(*real_playstate_func)(real_mpobject,EVENT_PLAYER_EXIT, "''vod");
		return;
	}
#if 0
	/*rtp test!!!*/
      printf("rtp test begin\n");
	tsplayer_buf.buf = buffer;
	tsplayer_buf.bufsize = RTP_MAX_PACKET_LEN;
	tsplayer_buf.readcount= 0;
	tsplayer_buf.bytesleft = tsplayer_buf.bufsize;
	while(tsplayer_buf.readcount < tsplayer_buf.bufsize) 
     {
read_noblockdata1:
			printf("VODRead\n");
			tsplayer_buf.count = VODRead(tsplayer_buf.buf + tsplayer_buf.readcount, tsplayer_buf.bytesleft);
			if(!tsplayer_buf.count)
			{
				printf("count == 0\n");
				goto read_noblockdata1;
			}	

			if(tsplayer_buf.count < 0) {
				fprintf(stderr, "Errno!!! = %d reading from multicast!!!!!\n");
				break;
			}
			printf("tsplayer_buf.readcount = %d\n",tsplayer_buf.readcount);
			printf("\n\n\ntsplayer_buf:\n\n");
			for(i = 0;i<tsplayer_buf.count; i++)
				{
				printf("0x%x ", *(tsplayer_buf.buf + i));
				}
			printf("\n\n\ntsplayer_buf:end\n\n");
			tsplayer_buf.readcount += tsplayer_buf.count;
			tsplayer_buf.bytesleft -= tsplayer_buf.count;
		}
while(1);
	/*rtp test  end!!!*/
#endif


	printf("main_vod_player:rtspPLAY\n");//vod从头播放，tvs从从json获取的绝对时间播放
//	printf("in:\nrtsp_streaming_server->ms_sessionID = %s\n",rtsp_streaming_server->ms_sessionID);
//	printf("rtsp_streaming_server->ms_GlobalSessionID = %s\n",rtsp_streaming_server->ms_GlobalSessionID);

	ret = rtsp_streaming_server->rtspPLAY(media_start_time);
	if (ret != 0)
	{
		printf("rtspPLAY failed!\n");
		trick_ok_flag = 1;
		rtsp_session_server->ss_keep_alive_end();
		rtsp_streaming_server->ms_keep_alive_end();
		printf("if_sm_setup = %d\n", if_sm_setup);
		if(if_sm_setup)
			rtsp_session_server->rtspTEARDOWN();
		//SetLastEvent(RTSP_CONNECT_FAILED);
		mediaplayer_thread = 0;        // 2012.03.26  thread ID可能!=0
		if (real_playstate_func)
			(*real_playstate_func)(real_mpobject,EVENT_PLAYER_EXIT, NULL);
		return;
	}
	trans_time=0;


/***********DVB play start************/
	
	printf("xuc: dvbplay start!cur_freq = %d, frequency = %d\n",cur_freq,  rtsp_session_server->play_param.frequency);   
	if(cur_freq != rtsp_session_server->play_param.frequency)
	{
		 ret =  DSP_DVBplay_tune(rtsp_session_server->play_param.frequency);
		 if(ret)
		 {
			printf("DSP_DVBplay_tune failed!\n");
			rtsp_session_server->ss_keep_alive_end();
			rtsp_streaming_server->ms_keep_alive_end();
			printf("if_sm_setup = %d\n", if_sm_setup);
			if(if_sm_setup)
				rtsp_session_server->rtspTEARDOWN();
			cur_freq = 0;
			trick_ok_flag = 1;
			//SetLastEvent(RTSP_CONNECT_FAILED);
			if (real_playstate_func)
				(*real_playstate_func)(real_mpobject, EVENT_PLAYER_EXIT, NULL);
			mediaplayer_thread = 0;        // 2012.03.26  thread ID可能!=0
			return;
			
		 }		
		 printf("DSP_DVBplay_tune success!\n");
		cur_freq = rtsp_session_server->play_param.frequency;
	}
	
	while(search_retry_times--) 	  //根据建流时间调整!待测试
	 {
		 ret = DSP_DVBplay_search(rtsp_session_server->play_param.pnID);	
		 if(!ret)
		 	break;
		 else
		 {
		 	if (PlayerRecvStopKey() == 1)
			{
				printf("recv quit key, break channel search, so go to cleanup\n");
				break;
			}
		 	usleep(200000);
	 	}	
	 }	
	 if(ret)
	 {
	 	printf("DSP_DVBplay_search failed!\n");
		rtsp_session_server->ss_keep_alive_end();
		rtsp_streaming_server->ms_keep_alive_end();
		printf("if_sm_setup = %d\n", if_sm_setup);
		if(if_sm_setup)
			rtsp_session_server->rtspTEARDOWN();
		trick_ok_flag = 1;
		//SetLastEvent(RTSP_CONNECT_FAILED);
		mediaplayer_thread = 0;        //  2012.03.26  thread ID可能!=0
		if (real_playstate_func)
			(*real_playstate_func)(real_mpobject, EVENT_PLAYER_EXIT, NULL);
		return;
	 } 
	 printf("DSP_DVBplay_search success!\n");

	if( (real_volume>MAX_VOLUME_SIZE)||(real_volume<MIN_VOLUME_SIZE))
		printf("volume out of range\n");
	if (real_muteflag)
		ret = DSP_DVBplay_setvolume(0);
	else
		ret = DSP_DVBplay_setvolume(real_volume);

HR_test:
#if USE_DRM
	scramble = parse_cw_from_license(rtsp_session_server->license, key);
	if (scramble == VOD_NO_SCRAMBLE)
	{
		printf("vod no scramble play\n");
		ret = DSP_DVBplay(havplay,hwin,rtsp_session_server->play_param.pnID, NULL);
	}
	else if (scramble == VOD_SCRAMBLE)
	{
		printf("vod scramble play\n");
		ret = DSP_DVBplay(havplay,hwin,rtsp_session_server->play_param.pnID, key);
	}
	else
	{
		printf("parse control word error, display as no scramble\n");
		ret = DSP_DVBplay(havplay,hwin,rtsp_session_server->play_param.pnID, NULL);
	}
#else
	 ret = DSP_DVBplay(havplay,hwin,rtsp_session_server->play_param.pnID, NULL);
#endif
	  if(ret)
	 {
		printf("DSP_DVBplay failed!\n");
		goto cleanup;
	 }  

/***********DVB play end************/

	if(0)
	{
//restart:	
		ret = -1;
		if (playerstate == PLAYERSTATE_FASTBACKWARD ||playerstate == PLAYERSTATE_FASTFORWARD )
		{
			ret = DSP_DVBplay_tplay(havplay);
		if(ret)
		{
				printf("DSP_DVBplay_tplay failed!\n");
				goto cleanup;
		}
		}
		else if(playerstate == PLAYERSTATE_PLAYING)
		{
restart:	
			ret = DSP_DVBplay_resume(havplay);
			if(ret)
			{
				printf("DSP_DVBplay_resume failed!\n");
				goto cleanup;
			}
		}
	}	
	
main_loop:
//restart:	
	printf("into while(1), playerstate = %d \n", playerstate);
//	usleep(400000);

	if (real_playstate_func)
				(*real_playstate_func)(real_mpobject,EVENT_LOAD_FINISH_AND_PLAY, NULL);
	while(1)
	{
	//	Poll_JD_Timerhandler();
		gotocmd = process_tvod_msg(); 
		PROCESS_MSG_RESULT(gotocmd);	//goto cleanup
		//playing_time = rtsp_streaming_server->
#if 0
		check_stream_times--;
		if(!check_stream_times)
		{
			check_stream_times = 100;	
			if(playerstate != PLAYERSTATE_PAUSED)
			{
				ret = DSP_DVBplay_check_stream_state(havplay);
				if(ret == -1)
				{
					printf("DSP_DVBplay_check_stream_state failed\n");
					continue;
				}
				else if(ret == 0)
				{
						printf("check_stream ret = 0\n");
						vbuffer_empty_times = 0;
				}
				else if(ret == 1)
				{
					printf("check_stream ret = 1\n");
					if (vbuffer_empty_times <= 0)
					goto cleanup;				
					else
						vbuffer_empty_times--;
								
				}
			}
		}
#endif		

#if 1
		check_stream_times--;
		if(!check_stream_times)
		{
			update_play_pts();
			check_stream_times = 100;	
			printf("playerstate = %d\n", playerstate);
			//2--PAUSE  3--PLAY  4--FF  5--BB
		}
#endif

		usleep(10000);
	}	
cleanup:
	trick_ok_flag=1;
	printf("main_vod_player: go to cleanup !\n");
#if USE_DRM
	if (scramble == VOD_SCRAMBLE)
	{
	DRMCDecreaseCount(VOD_LICENSE_FILE);
	DRMCFuncTeminate();
	ret = remove(VOD_LICENSE_FILE);
	if (ret == -1)
		printf("rm %s error\n", VOD_LICENSE_FILE);
	}
#endif
	trick_ok_flag = 1; /*防止进时移失败时，被vcr操作挂起*/
	rtsp_session_server->ss_keep_alive_end();
	rtsp_streaming_server->ms_keep_alive_end();
	rtsp_streaming_server->destroyConnection();
	printf("if_sm_setup = %d\n", if_sm_setup);
	if(if_sm_setup)
		rtsp_session_server->rtspTEARDOWN();
	Network_Release();
	if_sm_setup = 0;
	/*UI_JD_disappear(NULL);
	UI_TVODLOG_disappear();
	UI_JD_displaytime_disappear();*/
	printf("[DBG]DSP_DVBplay_stop start, %d\n", havplay);
	DSP_DVBplay_stop(havplay);
	printf("[DBG]DSP_DVBplay_stop start\n");

	if (real_playstate_func) /* 如果使用回调，需要页面修改，否则会两次调用析构函数*/
		(*real_playstate_func)(real_mpobject,EVENT_PLAYER_EXIT, "''vod");
	
	mediaplayer_thread = 0;       // 2012.03.26  线程退出但fd未变成0，导致判断错误
	return;
}


int StreamMediaPlayer::rcvfrom_mediaplayermsg(int *cmd, char *data)
{
	int ret=0;
	memset(&recvmsg,0,sizeof(recvmsg));
	ret=msgrcv(msgid, &recvmsg, sizeof(recvmsg)-sizeof(long), 0,IPC_NOWAIT);
	if(ret!=-1)
	{
		printf("Rcv_Msg %ld ret %d\n",recvmsg.mtype,ret);

		*cmd = recvmsg.mtype;
		printf("recv to id %d cmd %d\n",msgid,*cmd);

		if(ret)
		{
			memcpy(data,recvmsg.mdata,ret);
			data[ret]=0;
		}
	}
	//printf("rcvfrom_mediaplayermsg ret = %d\n",ret);
	return ret;
}


int StreamMediaPlayer::sendto_mediaplayermsg(int cmd,char *data, int datalen)
{
	int status = 0;
	int sendlen;

	memset(sendmsg.mdata,0,sizeof(sendmsg.mdata));

	sendlen = (datalen>MAX_MESSAGE_LEN)?MAX_MESSAGE_LEN:datalen;
	if(sendlen)
		memcpy(sendmsg.mdata,data,sendlen);

	sendmsg.mtype = cmd;

	printf("send to id %d cmd %d\n",msgid,cmd);
	status =  msgsnd(msgid, &sendmsg, sendlen, IPC_NOWAIT);
	if(status == -1)
	{
		perror("Send_Cmd_Browser:msgsnd");
		return 0;
	}
	return 1;
}

int StreamMediaPlayer::PlayerRecvStopKey()
{
	int msgtype;
	int ret = rcvfrom_mediaplayermsg(&msgtype); 

	if(ret >=0 && (msgtype == STREAMMEDIAPLAYER_QUIT))
		return 1;
	else
		return 0;
}

int StreamMediaPlayer::ClearDummyKey()
{
	int msgtype;

	int ret;
	do{
		ret = rcvfrom_mediaplayermsg(&msgtype); 
	}while(ret>=0);

	return 0;
}


void StreamMediaPlayer::process_common_msg(int *cmdtype, int *processed)
{
	int ret;

	ret = rcvfrom_mediaplayermsg(cmdtype);
	//printf("rcvfrom_mediaplayermsg ret2 = %d\n",ret);

	if(ret < 0) {
		return;
	}
	//printf("rcvfrom_mediaplayermsg ret3 = %d\n",ret);

	printf("recv a msg %d\n",*cmdtype);
	switch (*cmdtype)
	{
		case STREAMMEDIAPLAYER_VOLUMECHANGE:
			if( (volumesize>MAX_VOLUME_SIZE)||(volumesize<MIN_VOLUME_SIZE))
			{
				printf("volume out of range\n");
				break;
			}
			//ret=DSP_TSPlayer_SetVolumeIndex(TSPlayerHandle,volumesize*2);
			ret = DSP_DVBplay_setvolume(volumesize);
			printf("process_common_msg,vol is %d,ret is%d\n",volumesize,ret);

			*processed  = 1;
			break;

		case STREAMMEDIAPLAYER_VOLUMEMUTE:
			//DSP_TSPlayer_SetVolumeIndex(TSPlayerHandle,MIN_VOLUME_SIZE);
			DSP_DVBplay_setvolume(0);
			*processed  = 1;
			break;

		default:
			break;
	}

	return;
}
/*
void StreamMediaPlayer::Set_Player_Window()
{
	DSPRect rect;
	

	rect.x = real_videoDisplayrect.x*PLAYER_Full_Win_WIDTH/DFB_OSD_WIDTH;
	rect.y = real_videoDisplayrect.y*PLAYER_Full_Win_HEGHT/DFB_OSD_HEIGHT;
	rect.width = real_videoDisplayrect.width*PLAYER_Full_Win_WIDTH/DFB_OSD_WIDTH;
	rect.height = real_videoDisplayrect.height*PLAYER_Full_Win_HEGHT/DFB_OSD_HEIGHT;
      printf("xuc Set_Player_Window: rect x%d, y%d w%d h%d\n",real_videoDisplayrect.x, real_videoDisplayrect.y, real_videoDisplayrect.width, real_videoDisplayrect.height);
	//DSP_graphics_unSetScreen();
	//DSP_graphics_clearscreen(real_videoDisplayrect.x, real_videoDisplayrect.y, real_videoDisplayrect.width, real_videoDisplayrect.height);
	//printf("aa @@@@@@@@@@@@rect x%d, y%d w%d h%d\n",rect.x, rect.y, rect.width, rect.height);
	//printf("tsplayerHandle is %d pplayerwindow x%d, y%d w%d h%d\n",TSPlayerHandle,pplayerwindow->x, pplayerwindow->y, pplayerwindow->width, pplayerwindow->height);

      //视频输出与挖洞成对使用
      DSP_DVBplay_setwindow(real_videoDisplayrect.x, real_videoDisplayrect.y,real_videoDisplayrect.height, real_videoDisplayrect.width,hwin);
	//DSP_TSPlayer_Setlocation(TSPlayerHandle, rect.x, rect.y, rect.width, rect.height);
	printf("bbbbbbbbbbbbbb@@@@@@@@@@@@rect x%d, y%d w%d h%d\n",rect.x, rect.y, rect.width, rect.height);

	return;
}*/

void StreamMediaPlayer::check_prebuffering_state(unsigned int buffersize)
{
	bool quit_prebuf = false;
	static long long start_time;

	if (playerstate != PLAYERSTATE_BUFFERING)
		return;

	quit_prebuf = ((buffersize == 0) || (prebuf_level >= prebuf_max)) ? true : false;
	//quit_prebuf = ((buffersize == 0) || (get_ustime() - start_time > 400000)) ? TRUE : FALSE;

	prebuf_level += buffersize;

	if (quit_prebuf) 
	{
		printf("exit prebuffering state %d\n", prebuf_level);
		printf("setting play state\n");
		playerstate = PLAYERSTATE_PLAYING;
		mspeed=1;
		DSP_TSPlayer_Play(TSPlayerHandle, DSPPlaymode_PlayFwd, 1);
	}

	return;
}


