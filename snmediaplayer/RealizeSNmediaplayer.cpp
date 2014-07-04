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
	real_muteflag=muteFlag;//0:  ����Ϊ���� (Ĭ��ֵ);1:  ����Ϊ����
	real_nativeUIFlag=useNativeUIFlag;//0: ��ʹ��Player�ı���UI��ʾ����;1: ʹ��Player�ı���UI��ʾ���� (Ĭ��ֵ)
	real_muteUIFlag=muteuiflag;//: ��ʹ�ܾ�����ʾ�ı���UI��ʾ����;1: ʹ�ܾ�����ʾ�ı���UI��ʾ���ܣ�Ĭ��ֵ��
	real_audioVolumeUIFlag=audioVolumeUIFlag;//0: ��ʹ���������ڵı���UI��ʾ����;1: ʹ���������ڵı���UI��ʾ���ܣ�Ĭ��ֵ��
	real_audioTrackUIFlag=audioTrackUIFlag;//0: ��ʹ������ѡ��ı���UI��ʾ����;1: ʹ������ѡ��ı���UI��ʾ���ܣ�Ĭ��ֵ��
	real_progressBarUIFlag=progressBarUIFlag;//0: ��ʹ�ܽ������ı���UI��ʾ����;1: ʹ�ܽ������ı���UI��ʾ���ܣ�Ĭ��ֵ��
	real_channelNoUIFlag=channelNoUIFlag;//0: ��ʹ��Ƶ���ŵı���UI��ʾ����;1: ʹ��Ƶ���ŵı���UI��ʾ���ܣ�Ĭ��ֵ��
	real_subtitileFlag=subtitleFlag;//0:	����ʾ��Ļ (Ĭ��ֵ);   1:  ��ʾ��Ļ
	real_videoAlpha=videoAlpha;//0��100֮�������ֵ��0��ʾ��͸����100��ʾ��ȫ͸��(Ĭ��ֵΪ0)
	real_allowTrickmodeFlag=allowTrickmodeFlag;//0: ����TrickMode����;1: ������TrickMode���� (Ĭ��ֵ)
	real_cycleFlag=cycleFlag;
	real_randomFlag=randomFlag;
	real_autoDelFlag=autoDelFlag;
	real_speed=speed;//-32,-16,-8,-4,-2,0,...32
	real_volume=volume;//0-100����ʾ������0Ϊ������100Ϊ�������
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
	/*0: ��setVideoDisplayArea()���趨��Height, Width, Left, Top������ָ����λ�úʹ�С����ʾ��Ƶ
      	   1: ȫ����ʾ����ȫ���߶ȺͿ����ʾ(Ĭ��ֵ)
      	   2: �������ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ�������ʾ
      	   3: ���߶���ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ���߶���ʾ
      	   255: ��Ƶ��ʾ���ڽ����رա������ڱ���ý�������ӵ�ǰ���£�������Ƶ���ڡ������ý�岥��û�б���ͣ��������������Ƶ��
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



