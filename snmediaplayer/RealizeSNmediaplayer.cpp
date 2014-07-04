#include "RealizeSNmediaplayer.h"
//#include "common_setting.h"
#include "iostream"
RealizeSNmediaplayer::	RealizeSNmediaplayer()
{

}

RealizeSNmediaplayer::	~RealizeSNmediaplayer()
{

}
RealizeSNmediaplayer::RealizeSNmediaplayer(int instanceid,int videoDisplayMode, int playmode,
	int height, int width, int left, int top, int muteFlag,  int muteuiflag, int subtitleFlag, 
	int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag,string json_str,
	int useNativeUIFlag,int audioVolumeUIFlag,int audioTrackUIFlag,int progressBarUIFlag,
	int channelNoUIFlag,int allowTrickmodeFlag,int speed,int volume,int pipmode,
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
	real_allowTrickmodeFlag=allowTrickmodeFlag;//0: 允许TrickMode操做;1: 不允许TrickMode操作 (默认值)
	real_cycleFlag=cycleFlag;
	real_randomFlag=randomFlag;
	real_autoDelFlag=autoDelFlag;
	real_speed=speed;//-32,-16,-8,-4,-2,0,...32
	real_volume=volume;//0-100，表示音量，0为静音，100为最大音量
	real_pipmode=pipmode;
	real_playstate_func=playstate_func;
	if(real_videoDisplayMode==1||
		(real_videoDisplayrect.height==DFB_OSD_HEIGHT&&real_videoDisplayrect.width==DFB_OSD_WIDTH))
		iftvfullscreen=1;
	else
		iftvfullscreen=0;
	printf("1iftvfullscreen is %d\n",iftvfullscreen);
}


char *RealizeSNmediaplayer::getAudioTrack()
{
	return "undefined";
}

char *RealizeSNmediaplayer::getCurrentAudioChannel()
{
	return "undefined";
}

char *RealizeSNmediaplayer::getMediaCode()
{
	return "undefined";
}

char *RealizeSNmediaplayer::getSubtitle()
{
	return "undefined";
}


void RealizeSNmediaplayer::setMuteFlag(int mute_flag)
{
/*
	read_volume_index(&real_volume);
	printf("2real_volume is %d\n",real_volume);
	if(mute_flag)
		player_setvolume(MIN_VOLUME_SIZE);
	else 
		player_setvolume(real_volume);
	real_muteflag = mute_flag;
	return;
*/
}


int RealizeSNmediaplayer::getAudioPID()
{
	return 0;
	
}


pid_info * RealizeSNmediaplayer::getAudioPIDs()
{

	return NULL;
}


void  RealizeSNmediaplayer::setAudioPID(int pid)
{

	return;
}


int  RealizeSNmediaplayer::getSubtitlePID()
{

	return 0;
}


void RealizeSNmediaplayer::setSubtitlePID(int pid)
{

	return;
}


void RealizeSNmediaplayer::setVolume(int volume_value)//not use
{
	if(volume_value >= MIN_VOLUME_SIZE&& volume_value <= MAX_VOLUME_SIZE)
	{
		player_setvolume(volume_value);
		real_volume = volume_value;
		printf("1real_volume is %d\n",real_volume);
	}
	return;
}


int RealizeSNmediaplayer::refreshVideoDisplay()
{
	PlayerRect tmprect;
	int ret;
	/*0: 按setVideoDisplayArea()中设定的Height, Width, Left, Top属性所指定的位置和大小来显示视频
      	   1: 全屏显示，按全屏高度和宽度显示(默认值)
      	   2: 按宽度显示，指在不改变原有图像纵横比的情况下按全屏宽度显示
      	   3: 按高度显示，指在不改变原有图像纵横比的情况下按全屏高度显示
      	   255: 视频显示窗口将被关闭。它将在保持媒体流连接的前提下，隐藏视频窗口。如果流媒体播放没有被暂停，将继续播放音频。
      	*/
      	printf("real_videoDisplayMode(1:full) is %d\n",real_videoDisplayMode);
	printf(".x(%d), .y(%d),.width(%d), .height(%d) \n",
		real_videoDisplayrect.x, real_videoDisplayrect.y,real_videoDisplayrect.width, real_videoDisplayrect.height);
	if(real_videoDisplayMode==0||real_videoDisplayMode==2||real_videoDisplayMode==3)
		{
		ret =  player_setwindowparms(real_videoDisplayrect.x, real_videoDisplayrect.y,real_videoDisplayrect.width, real_videoDisplayrect.height);
		printf("xuc: refreshVideoDisplay exit!mode 0,2,3,  player_setwindowparms ret = %d\n",ret);
		return ret ;
		}
	else if(real_videoDisplayMode==1)
		{
		ret =  player_setwindowparms(0, 0,DFB_OSD_WIDTH, DFB_OSD_HEIGHT);
		printf("xuc: refreshVideoDisplay exit!mode 1, player_setwindowparms ret = %d\n",ret);
		return ret ;
		}
	else if(real_videoDisplayMode==255)
		{
		ret =  player_setwindowparms(0, 0,0, 0);
		printf("xuc: refreshVideoDisplay exit!mode 255, player_setwindowparms ret = %d\n",ret);
		return ret ;
		}

}



MediaPlayErrorType RealizeSNmediaplayer::eventtype;

int RealizeSNmediaplayer::getevent()
{
	return (int)eventtype;
}


void RealizeSNmediaplayer::setVideoDisplayMode(int videoDisplayMode)
{
	real_videoDisplayMode=videoDisplayMode;
	printf("1real_videoDisplayMode is %d\n",real_videoDisplayMode);
	return;
}



