#ifndef _REALIZESNMEDIAPLAYER_H
#define _REALIZESNMEDIAPLAYER_H
#include <string>
#include <iostream>
#include <stdio.h>

using namespace std;

#define MIN_VOLUME_SIZE 0
#define MAX_VOLUME_SIZE 100
#define PLAYERVENDORINFO "3NMEDIAPLAYER"
#define MAX_PLAYER_NUM 2

#define DFB_OSD_WIDTH 1280
#define DFB_OSD_HEIGHT 720


#define RTP_HEADER_LEN 12

typedef enum
{
	STREAMMEDIAPLAYER_NONE,
	STREAMMEDIAPLAYER_URLCHANGE,
	STREAMMEDIAPLAYER_VOLUMECHANGE,
	STREAMMEDIAPLAYER_VOLUMEMUTE,
	STREAMMEDIAPLAYER_PLAY,
	STREAMMEDIAPLAYER_PAUSE,
	STREAMMEDIAPLAYER_FASTFORWARD,
	STREAMMEDIAPLAYER_FASTBACKWARD,
	STREAMMEDIAPLAYER_SEEK,
	STREAMMEDIAPLAYER_REALTIMETV,
	STREAMMEDIAPLAYER_QUIT
}MediaPlayCMD;



#define RET_PLAYER_QUIT     0     /*�˳������������������κβ���*/
#define RET_PLAYER_LOCAL   1   /*�ڱ�ҵ�����л�*/ 
#define RET_PLAYER_OTHER   2  /*�л�������ҵ��*/


#define SECONDS_ONEDAY 86400
#define SECONDS_EIGHTHOURS 28800
#define SPEED_OFFSET 100


#define PLAYER_Full_Win_X 0 
#define PLAYER_Full_Win_Y 0
#define PLAYER_Full_Win_WIDTH 10000
#define PLAYER_Full_Win_HEGHT 10000



typedef enum
{
	MEDIA_TVPLAY,
	MEDIA_VODPLAY
}StreamType;



//��ʱ���� ��Ϊ��λ
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





typedef void (*MediaplayerStateChangeCallback)(void *, int, char *);

typedef enum
{
	MEDIAPLAYER_ERRNONE=0,
	RTSP_CONNECT_FAILED = 1,
	RTSP_SEND_FAILED,
	RTSP_READ_FAILED,
	RTSP_CONNECTION_DIED,
	RTSP_STATUS_LINE,
	RTSP_STATUS_PLAYING,
	RTSP_STATUS_ANNOUNCE,
	RTSP_STATUS_END_OF_STREAM,
	RTSP_STATUS_NOT_FOUND,
	STATUS_NO_VIDEO,
	IGMP_STATUS_END_OF_STREAM,
	RTSP_CONNECT_STOPPED,
	RTSP_STATUS_AUTHENTICATE,
	RTSP_STATUS_REDIRECTED,
	IGMP_STATUS_PLAYING,
	UDP_STATUS_END_OF_STREAM,
	UDP_STATUS_PLAYING,
	MP3S_STATUS_END_OF_STREAM,
	AUD_DESCRIP_READY,
	RTSP_STATUS_START_OF_STREAM,
	MP3S_STATUS_START_OF_STREAM,
	RTSP_STATUS_ANNOUNCE_PARTIAL, 
	PVRS_STATUS_PLAYING,
	PVRS_STATUS_END_OF_STREAM,
	RTSP_STATUS_SET_PARAMETER,
	RTSP_STATUS_INVALID_RANGE
}MediaPlayErrorType;

//��Ϣ���� keyֵ
#define MEDIAPLAYER_QUEUE_KEY (6200L+131)

#define MAX_MESSAGE_LEN	2048
typedef struct msgbuff{
	long mtype;
	char mdata[MAX_MESSAGE_LEN];
}Message;

typedef struct {
	int x;
	int y;
	int width;
	int height;
}PlayerRect;	


typedef struct pidattr_
{
	int pid;
	char * language;
}pidattr;
typedef struct pid_info_
{
	int pid_count;
	pidattr * p_pidattr;
}pid_info;

typedef enum play_state_
{
	PLAYERSTATE_PREINIT=0,//wait for init
	PLAYERSTATE_STOPED,//stopped playback of the current media item is stopped
	PLAYERSTATE_PAUSED,//paused playback of the current media item is paused
	PLAYERSTATE_PLAYING,
	PLAYERSTATE_FASTFORWARD,
	PLAYERSTATE_FASTBACKWARD,
	PLAYERSTATE_BUFFERING,//��ǰ����ý���ǵõ�����������������
	PLAYERSTATE_WAITFORSESSION,//�ȴ��������ӣ�������������������waiting fro session to begin
	PLAYERSTATE_ITEMFINISH,//mediaended media item has completed playback
	PLAYERSTATE_PREPARENEW,//transitioning preparing new media item
	PLAYERSTATE_READY,//Ready ready to begin playing
	PLAYERSTATE_RECONECT//reconnecting to stream
}play_state;

typedef enum media_type_
{
	MEDIA_TYPE_VOD=0,
	MEDIA_TYPE_FILM=1     //ʱ��
}media_type;

#define PLAYER_Full_Win_X 0 
#define PLAYER_Full_Win_Y 0
#define PLAYER_Full_Win_WIDTH 10000
#define PLAYER_Full_Win_HEGHT 10000

class RealizeSNmediaplayer
{
	
public:
	/*���캯��*/
	RealizeSNmediaplayer();
	
	RealizeSNmediaplayer(int instanceid, int videoDisplayMode, int playmode,
	int height, int width, int left, int top, int muteFlag,  int muteuiflag, int subtitleFlag, 
	int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag,string json_str,
	int useNativeUIFlag,int audioVolumeUIFlag,int audioTrackUIFlag,int progressBarUIFlag,
	int channelNoUIFlag,int allowTrickmodeFlag,int speed,int volume,int pipmode,
	MediaplayerStateChangeCallback playstate_func);//��json������

//����Ϊ�麯��
	/*��������*/
	virtual ~RealizeSNmediaplayer();

	virtual play_state getPlayState()=0;

	virtual void player_setvolume(int volumeindex)=0;

	/*	Ҫ���ն˷���ָ����Ƶ�������������ء�
	���ɱ�������Ϊ������Ƶ����Ҳ����-1��
	Ƶ����ַΪͨ��SetConfig���õ�Ƶ���б��еĵ�ַ	
	Userchannelid����������ʾ�û�Ƶ���š�
	����ֵ��0����ʾ�ɹ���
	-1����ʾƵ������Ч��
	*/
	virtual int joinChannel(int userchannelid,string& channel_info)=0;

	/*Ҫ���ն��뿪ָ����Ƶ��������������
	����ֵ��0����ʾ�ɹ���
	-1����ʾƵ������Ч��
	*/
	virtual int leaveChannel()=0;
	
	/*	0: ����TrickMode����
		1: ������TrickMode���� (Ĭ��ֵ)
	*/
	virtual void setAllowTrickmodeFlag (int allowTrickmode)=0;

/*	��ý����ʼ�㿪ʼ���š�
	��TV channel����ʵʱTV�ķ�ʽ��ʼ���š�
	*/
	virtual int playFromStart() =0;

	/*	�ӵ�ǰý���ĳ��ʱ��㿪ʼ����ý��
	����playlist������ָ��ǰ�Ѿ�ѡ�е�ý�壩��
	��ʵʱ���ŵ�TV channel�õ�����Ч��
	���Դ���ʱ��״̬��TV channel��Ч��
	-type:
	1���μ�RFC2326�е�Normal Play Time (NPT)
	2���μ�RFC2326�е�Absolute Time (Clock Time)
	-timestamp:
	�μ�RFC2326�е�Normal Play Time (NPT)��Absolute Time (Clock Time)����ʱ�����͵ĸ�ʽ
	timestamp:��VoD�����Ǵ�ý����ʼ�㿪ʼ��������ʱ�䣻��TVoD����ʱ�����ý����Ծ��Ǿ���ʱ�䡣
	-speed��
	�����ٶȣ���ѡ������
	*/
	virtual int playByTime(int type, int timestamp,int speed) =0;

	/*��ͣ���ڲ��ŵ�ý��*/
	virtual int pause() =0;

	/*	-speed��2��32*/
	virtual int fastForward(int speed) =0;

	/*	-speed��-2��-32*/
	virtual int fastRewind(int speed) =0;

	/*�ӵ�ǰý�����ͣ/���/����״̬�ָ��������š�*/
	virtual int resume() =0;

	/*����ý��ĩ�˲���*/
	virtual int gotoEnd() =0;//��ѡ��

	/*����ý����ʼ�㲥��*/
	virtual int gotoStart()=0;//��ѡ��

	/*ֹͣ���ڲ��ŵ�ý�塣���ͷŻ����б��ز������������Դ��*/
	virtual int stop()=0;

	/*ֹͣ���ڲ��ŵ�ý�塣���ͷŻ����б��ز������������Դ��*/
	virtual int player_setwindowparms(int top, int left, int width, int height)=0;

	/*�л�������ѭ�����С�*/
	virtual int switchAudioChannel()=0;//��ѡ��

	/*�л����죬ѭ�����С�*/
	virtual int switchAudioTrack()=0;//��ѡ��

	/*�л���Ļ��ѭ�����С�*/
	virtual int switchSubtitle()=0;//��ѡ��

	/*��ȡ��ǰ���ŵ�ý�����ʱ��,����������Ϊ��λ*/
	virtual int getMediaDuration()=0;

	/*	��ȡý�岥�ŵ��ĵ�ǰʱ���
	��VoDΪ�Ӹ�ý����ʼ�㿪ʼ��������ʱ�䣬����Ϊ��λ��
	��TVoDΪ��ǰ���ŵ�ľ���ʱ�䣻
	��Channel����������
	�μ�RFC2326�е�Normal Play Time (NPT)��Absolute Time (Clock Time)����ʱ�����͵ĸ�ʽ
	*/
	virtual unsigned long getCurrentPlayTime()=0;

	/*�������ĵ�ǰ����ģʽ
	�������ٰ���������ģʽ���͡�ģʽ��ز����� ������Ϣ��
	����ģʽ�֣�Normal Play��Pause��Trickmode��
	��ģʽΪTrickmodeʱ�����2x/-2x, 4x/-4x, 8x/-8x, 16x/-16x, 32x/-32x��������ʾ���/���˵��ٶȲ���
	*/
	virtual char * getPlaybackMode() =0;

	/*	���ڻ�ȡ��һ�η����Ĵ������*/
	virtual int GetLastError()=0;//��ѡ

	/*���ػ����е�ǰ���ŵ�Ƶ���ţ�
	���ܻ����Ч��Ƶ����ʱ������-1��
	*/
	virtual int getChannelNum() =0;

	virtual int setSingleMedia(string mediaStr)=0;


//����Ϊ�麯��
	/*	����videoDisplayMode, vedioDisplayArea����, ������Ƶ����ʾ��*/
	int refreshVideoDisplay();

	/*�������̵�������չ�������ɳ����Զ���*/
	virtual int sendVendorSpecificCommand(char *xml){return 0;}

	/*������ƵPID��
	������ڲ��ŷ��ص�ǰ���ŵ�PID�����򷵻�ȱʡ/��һ��PID��
	*/
	int getAudioPID();//��ѡ��

	/*	����һ������������ƵPID��Ϣ�Ķ���
	�������������Ϣ�� 
	pids ��ǰý�����а�������ƵPID���� 
	pidarr[1..pids]�C��ƵPID���飺 
	AudioPID�CPIDֵ����ֵ�� 
	AudioLanguage�CPID�����Ա�־���ַ�����
	*/
	pid_info *getAudioPIDs();//��ѡ��

	/*ָ��ʹ�õ���Ƶpid*/
	void setAudioPID(int pid);

	/*������ĻPID��������ڲ��ŷ��ص�ǰ���ŵ�PID�����򷵻�ȱ ʡ/��һ��PID*/
	int getSubtitlePID();

	/*	����һ������������ĻPID��Ϣ�Ķ��󡣶������������Ϣ�� 
	pids�C��ǰý�����а�������ĻPID���� 
	pidarr[1..pids]�C��ĻPID���飺 
	SubtitlePID -PIDֵ����ֵ�� 
	SubtitleLanguage- PID�����Ա�־���ַ�����
	*/
	pid_info *getSubtitlePIDs();

	/*ָ��ʹ�õ���Ļpid*/
	void setSubtitlePID(int pid);

	/*Volume��0-100����ʾ������0Ϊ������100Ϊ�������*/
	void setVolume(int volume_value);
	
	
	/*���ò���������ģʽ
	   0: ��ý��Ĳ���ģʽ (Ĭ��ֵ)
	   1: �����б�Ĳ���ģʽ
	*/
	void setSingleOrPlaylistMode(int playmode){real_playmode=playmode;return;}

	/*��ȡ����������ģʽ*/
	int getSingleOrPlaylistMode(){return real_playmode;}


	/*0: ��setVideoDisplayArea()���趨��Height, Width, Left, Top������ָ����λ�úʹ�С����ʾ��Ƶ
      	   1: ȫ����ʾ����ȫ���߶ȺͿ����ʾ(Ĭ��ֵ)
      	   2: �������ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ�������ʾ
      	   3: ���߶���ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ���߶���ʾ
      	   255: ��Ƶ��ʾ���ڽ����رա������ڱ���ý�������ӵ�ǰ���£�������Ƶ���ڡ������ý�岥��û�б���ͣ��������������Ƶ��

	   MediaPlayer�����Ӧ����Ƶ���ڵ���ʾģʽ.
         ÿ�ε��øú�������Ƶ��ʾ���ڲ����ᱻ��������ˢ��
         �Է�ӳ���ĺ����ʾЧ��ֻ�еȵ���ʽ����refreshVideoDisplay()��
         �Ż�ˢ��	
      */
	void setVideoDisplayMode(int videoDisplayMode);
	int getVideoDisplayMode(){return real_videoDisplayMode;}

	/*	MediaPlayer�����Ӧ����Ƶ���ڵ���ʾģʽ.
	ÿ�ε��øú�������Ƶ��ʾ���ڲ����ᱻ��������ˢ��
	�Է�ӳ���ĺ����ʾЧ��ֻ�еȵ���ʽ����refreshVideoDisplay()��
	�Ż�ˢ��
	*/
	void setVideoDisplayArea (int left, int top, int width, int height){
		real_videoDisplayrect.x=left;
		real_videoDisplayrect.y=top;
		real_videoDisplayrect.width=width;
		real_videoDisplayrect.height=height;
		printf("setVideoDisplayArea,x(%d),y(%d),w(%d),h(%d)\n",left,top,width,height);
		return;
	}

	int getVideoDisplayLeft(){return real_videoDisplayrect.x;}

	int getVideoDisplayTop(){return real_videoDisplayrect.y;}

	int getVideoDisplayWidth(){return real_videoDisplayrect.width;}

	int getVideoDisplayHeight(){return real_videoDisplayrect.height;}

	/*	0:  ����Ϊ���� (Ĭ��ֵ)
		1:  ����Ϊ����
	*/
	void setMuteFlag(int mute_flag);//�˺�����Ҫά��muteflag

	int getMuteFlag(){return real_muteflag;}

	/*	0: ��ʹ��Player�ı���UI��ʾ����
		1: ʹ��Player�ı���UI��ʾ���� (Ĭ��ֵ)
	*/
	void setNativeUIFlag (int nativeUI_flag){real_nativeUIFlag = nativeUI_flag;}//��ѡ������,�˺�����Ҫά��nativeUIFlag

	int getNativeUIFlag(){return real_nativeUIFlag;}//��ѡ������

	/*	0: ��ʹ�ܾ�����ʾ�ı���UI��ʾ����
		1: ʹ�ܾ�����ʾ�ı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setMuteUIFlag (int muteUI_flag){real_muteUIFlag = muteUI_flag;}//��ѡ������,�˺�����Ҫά��muteUIFlag
	
	int getMuteUIFlag(){return real_muteUIFlag;}//��ѡ������

	/*	0: ��ʹ���������ڵı���UI��ʾ����
		1: ʹ���������ڵı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setAudioVolumeUIFlag (int audioVolumeUI_flag){real_audioVolumeUIFlag = audioVolumeUI_flag;}//��ѡ������,�˺�����Ҫά��audioVolumeUIFlag
	
	int getAudioVolumeUIFlag(){return real_audioVolumeUIFlag;}//��ѡ������

	/*	0: ��ʹ������ѡ��ı���UI��ʾ����
		1: ʹ������ѡ��ı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	virtual void setAudioTrackUIFlag (int audioTrackUI_flag){real_audioTrackUIFlag = audioTrackUI_flag;}//��ѡ������,�˺�����Ҫά��audioTrackUIFlag
	
	int getAudioTrackUIFlag(){return real_audioTrackUIFlag;}//��ѡ������

	/*    0����Ϊѭ�����Ž�Ŀ(Ĭ��ֵ)
		1��ѭ��,���β���
	*/
	void setCycleFlag (int cycleFlag){return;}

	/*    0����Ϊ������Ž�Ŀ(Ĭ��ֵ)
		1˳�򲥷�
	*/
	void setRandomFlag (int randomFlag){return;}

	/*	0: ��ʹ�ܽ������ı���UI��ʾ����
		1: ʹ�ܽ������ı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setProgressBarUIFlag (int progressBarUI_flag){real_progressBarUIFlag = progressBarUI_flag;}//��ѡ������,�˺�����Ҫά��progressBarUIFlag
	
	int getProgressBarUIFlag(){return real_progressBarUIFlag;}//��ѡ������

	/*	0: ��ʹ��Ƶ���ŵı���UI��ʾ����
		1: ʹ��Ƶ���ŵı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setChannelNoUIFlag (int channelNoUI){real_channelNoUIFlag = channelNoUI;}//��ѡ������
	
	int getChannelNoUIFlag(){return real_channelNoUIFlag;}//��ѡ������

	/*	0:  ����ʾ��Ļ (Ĭ��ֵ)
		1:  ��ʾ��Ļ
	*/
	void setSubtitileFlag (int subtitile){real_subtitileFlag = subtitile;}//��ѡ������
	
	int getSubtitileFlag(){return real_subtitileFlag;}//��ѡ������

	/*���õ�ǰ������Ϊ���л�ģʽ
	mode:1���л�������:��׼ģʽ*/
	void setPIPVideoMode(int pipmode){return;}
	/*	0��100֮�������ֵ��0��ʾ��͸����100��ʾ��ȫ͸����
		(Ĭ��ֵΪ0)
	*/
	void setVideoAlpha (int vAlpha){return;}//��ѡ������
	
	int getVideoAlpha(){return real_videoAlpha;}//��ѡ������

	int getAllowTrickmodeFlag(){return real_allowTrickmodeFlag;}

	/*	�������̵���չ�������úͻ�ȡ����ʽ�ɳ����Լ�ȷ��*/
	void setVendorSpecificAttr (char * vendorSpecificAttr){return;}
	
	string getVendorSpecificAttr(){return PLAYERVENDORINFO;}

	/*	��ȡ��ǰ���ŵ�ý��ı�ʶ�� 
	MediaCode��ý���Ψһ��ʶ����Ŀ��Ƶ���ȣ���
	ʹ��ϵͳ�����ContentID�����������ڲ����б�ģʽʱ��
	MediaCode�Ƕ�̬�ı�ġ�
	*/
	char *getMediaCode();


	/*	��ȡ��ǰ����������
	�ַ���Left��Right��Stereo��JointStereo
	*/
	char * getCurrentAudioChannel();

	/*��ȡ��ǰ�ĵ���������
	�ַ���Chinese��English��
	*/
	char * getAudioTrack();

	/*��ȡ��ǰѡ�����Ļ����
	�ַ���Chinese��English��
	*/
	char * getSubtitle();

	/*��ȡ�����б��е�ý������*/
	int getMediaCount();//��ѡ��

	/*��ȡ��ǰ����ý�壬�ڲ����б��е�����ֵ
	0����ʾý���б�Ķ���
	*/
	int getCurrentIndex();//��ѡ��

	/*��ȡ��ǰ���ŵ�ý���EntryID*/
	char * getEntryID();//��ѡ��

	/*�ַ�������JSON��ʽ���ַ�������װ�����б����ݡ�*/
	char *getPlaylist();//��ѡ��

	/*	nativePlayerInstanceID:һ���ֽڳ��ȵ��޷���������0��255
		���أ�
		0�� ��ʾ�󶨳ɹ���
		��1�� ��ʾ��ʧ�ܣ�
		����ֵδ����
	*/
	int bindNativePlayerInstance(unsigned char nativePlayerInstanceID);

	/*��ʼ��MediaPlayer�����ԡ�
	���´�����Media Player��Ϊ�˺����е�STB����ĳ��ý�岥��ʵ��
	������ǰ����ҳ���д���Media Player����ʱ�����ģ����а󶨣�
	������øú�����
	���أ�
	0�� ��ʾ�󶨳ɹ���
	��1�� ��ʾ��ʧ�ܣ�
	����ֵδ����
	*/
	int initMediaPlayer(unsigned char nativePlayerinstanceID, int playlistFlag, int videoDisplayMode, 
		int height, int width, int left, int top, int muteFlag,  int useNativeUIFlag, int subtitleFlag, 
		int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag);


	/*��ȡstb���ز��Ÿ���ʵ����instanceID
	type:0ȱʡʵ����1����������2���л�������������ֵԤ��*/
	unsigned char getNativePlayerInstanceIDByType(int type);
	/*���õ������ŵ�ý��*/

	/*�ڲ����б�ĩ�ˣ���ӵ���ý�塣
	index�����Ϊ��������-1������ʾĬ��������б�β����Append����
	��ָ����indexֵ���ڻ����playlist������ý����Ŀ���������playlistβ����
	�������Ϊ�����б�ĳ��λ�ã�Insert����
	index��������Ŀ���ƶ�/����/ɾ������̬�仯��
	��STB��֤��ǰplaylist��������Ŀ��index�Ǵ�0�����һ�����ġ�
	����entry_id��Ψһ��ʶplaylist��ÿ��ý����Ŀ��
	һ���ɹ�����ĳ��ý��󣬸�ֵ����仯��
	*/
	int addSingleMedia(int index, char *mediaStr);//��ѡ��

	//�ڲ����б�ĩ�ˣ����һ��ý�塣
	int addBatchMedia(char *batchMediaStr);//	��ѡ��


	/*�Ӳ����б���ɾ��ָ��ý��
	�����б���ĳ��ý����Ŀ��Ψһ��ʶ�������ý��ʱ���ò����ڸò����б��б��ֲ��䣩
	*/
	int removeMediaByEntryID(char *entryID);//��ѡ��

	/*�Ӳ����б���ɾ��ָ��ý��
	index��������ý����ý���б��е�����ֵ
	*/
	int removeMediaByIndex(int index);//	��ѡ��

	/*��ղ����б�
	����ֵ��0����ʾ�ɹ���
	-1����ʾʧ�ܡ�
	*/
	int clearAllMedia();//	��ѡ��

	/*	����index��ָ����ý���ڲ����б����ƶ�
	entryID: �����б���ĳ��ý����Ŀ��Ψһ��ʶ��
	toIndex����������Ҫ�ƶ���������ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaByIndex(char *entryID, int toIndex);//	��ѡ��

	/*����ƫ������ָ����ý���ڲ����б����ƶ�
	entryID: �����б���ĳ��ý����Ŀ��Ψһ��ʶ��
	offset��ƫ��������������ʾ��ָ��ý�����б�ĩ���ƶ�����������ʾ��ָ��ý�����б���ʼ���ƶ�
	*/
	int moveMediaByOffset(char *entryID, int offset);//��ѡ��

	/*����index��ָ����ý���ڲ����б����ƶ�
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	toIndex����������Ҫ�ƶ���������ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaByIndex1(int index, int toIndex);//��ѡ��

	/*����ƫ������ָ����ý���ڲ����б����ƶ�
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	offset��ƫ��������������ʾ��ָ��ý�����б�ĩ���ƶ�����������ʾ��ָ��ý�����б���ʼ���ƶ�
	*/
	int moveMediaByOffset1(int index, int offset);//��ѡ��

	/*��ָ����ý������*/
	int moveMediaToNext(char *entryID);//	��ѡ��

	/*��ָ����ý������*/
	int moveMediaToPrevious(char *entryID);//	��ѡ��

	/*��ָ����ý���Ƶ��б���*/
	int moveMediaToFirst(char *entryID);//	��ѡ��

	/*��ָ����ý���Ƶ��б�ĩ��*/
	int moveMediaToLast(char *entryID);//	��ѡ��

	/*��ָ����ý������
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToNext1(int index);//	��ѡ��

	/*��ָ����ý������
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToPrevious1(int index);//	��ѡ��

	/*��ָ����ý���Ƶ��б���
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToFirst1(int index);//	��ѡ��

	/*��ָ����ý���Ƶ��б�ĩ��
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToLast1(int index);//	��ѡ��

	/*��ý���ڲ����б��е�����ѡ��Ϊ��ǰ��ѡ���Ž�Ŀ
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int selectMediaByIndex(int index);//	��ѡ��

	/*���뵱ǰý��������ƫ����ѡ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ
	offset��ƫ��������������ʾ�ӵ�ǰý�����б�ĩ����ת����������ʾ�ӵ�ǰý�����б���ʼ����ת
	*/
	int selectMediaByOffset(int offset);//	��ѡ��

	/*ѡȡ�����б��е���һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectNext();//	��ѡ��

	/*ѡȡ�����б��е���һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectPrevious();//	��ѡ��

	/*ѡȡ�����б��еĵ�һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectFirst();//	��ѡ��

	/*ѡȡ�����б��е����һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectLast();//	��ѡ��

	/*��ĳ��ý����Ŀ��Ψһ��ʶѡ��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int SelectMediaByEntryID(char *entryID);//	��ѡ��


	/*����ֵ����ǰϵͳ������0-100*/
	int getVolume(){return real_volume;}

	/*	ioStr:Ҫ���õĲ��������ơ������������ɻ����ж��壩
		wrStr��Ҫ���õĲ�����ֵ��
	*/
	int set(string ioStr, string wrStr);//��չʹ��

	/*ioStr:Ҫ��ȡ�Ĳ��������ơ������������ɻ����ж��壩*/
	int get(string ioStr);//��չʹ��

	static int getevent();


protected:
	static MediaPlayErrorType eventtype;

	int real_instanceid;
	string real_json_str;
	int real_playmode;//	0: ��ý��Ĳ���ģʽ (Ĭ��ֵ);1: �����б�Ĳ���ģʽ
	/*0: ��setVideoDisplayArea()���趨��Height, Width, Left, Top������ָ����λ�úʹ�С����ʾ��Ƶ
      	   1: ȫ����ʾ����ȫ���߶ȺͿ����ʾ(Ĭ��ֵ)
      	   2: �������ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ�������ʾ
      	   3: ���߶���ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ���߶���ʾ
      	   255: ��Ƶ��ʾ���ڽ����رա������ڱ���ý�������ӵ�ǰ���£�������Ƶ���ڡ������ý�岥��û�б���ͣ��������������Ƶ��
      	*/
	int real_videoDisplayMode;
	int iftvfullscreen;
	PlayerRect real_videoDisplayrect;
	int real_muteflag;//0:  ����Ϊ���� (Ĭ��ֵ);1:  ����Ϊ����
	int real_nativeUIFlag;//0: ��ʹ��Player�ı���UI��ʾ����;1: ʹ��Player�ı���UI��ʾ���� (Ĭ��ֵ)
	int real_muteUIFlag;//: ��ʹ�ܾ�����ʾ�ı���UI��ʾ����;1: ʹ�ܾ�����ʾ�ı���UI��ʾ���ܣ�Ĭ��ֵ��
	int real_audioVolumeUIFlag;//0: ��ʹ���������ڵı���UI��ʾ����;1: ʹ���������ڵı���UI��ʾ���ܣ�Ĭ��ֵ��
	int real_audioTrackUIFlag;//0: ��ʹ������ѡ��ı���UI��ʾ����;1: ʹ������ѡ��ı���UI��ʾ���ܣ�Ĭ��ֵ��
	int real_progressBarUIFlag;//0: ��ʹ�ܽ������ı���UI��ʾ����;1: ʹ�ܽ������ı���UI��ʾ���ܣ�Ĭ��ֵ��
	int real_channelNoUIFlag;//0: ��ʹ��Ƶ���ŵı���UI��ʾ����;1: ʹ��Ƶ���ŵı���UI��ʾ���ܣ�Ĭ��ֵ��
	int real_subtitileFlag;//0:  ����ʾ��Ļ (Ĭ��ֵ);	1:  ��ʾ��Ļ
	int real_videoAlpha;//0��100֮�������ֵ��0��ʾ��͸����100��ʾ��ȫ͸��(Ĭ��ֵΪ0)
	int real_allowTrickmodeFlag;//	0: ����TrickMode����;1: ������TrickMode���� (Ĭ��ֵ)
	int real_playlistFlag;
	int real_cycleFlag;//0����Ϊѭ�����Ž�Ŀ(Ĭ��ֵ);1��ѭ��,���β���
	int real_randomFlag;
	int real_autoDelFlag;
	string real_mediacode;
	int real_speed;//-32,-16,-8,-4,-2,0,...32
	int real_volume;//0-100����ʾ������0Ϊ������100Ϊ�������
	int real_pipmode;
	void * real_mpobject;
	MediaplayerStateChangeCallback real_playstate_func;
	media_type playing_media_type;
	

};
#endif
