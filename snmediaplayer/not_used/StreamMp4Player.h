#ifndef _STREAMMP4PLAYER_H
#define _STREAMMP4PLAYER_H

#include <sys/msg.h>

#include "RealizeSNmediaplayer.h"
#include "commonplayer.h"
#include "mp4player.h"
#include "mp3_parse.h"


#include "VodClient.h"


struct audiopacket{
	unsigned long AudioFrameSize[80];
	unsigned long NumPackets;
	unsigned long index;
	unsigned long timestamp;
	unsigned char buf[1500];
	unsigned char *bufstart;
};


class StreamMp4Player:public RealizeSNmediaplayer
{
	
public:
	/*构造函数*/
	StreamMp4Player();
	StreamMp4Player(int instanceid,int videoDisplayMode, int playmode,
		int height, int width, int left, int top, int muteFlag,  int muteuiflag, int subtitleFlag, 	
		int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag,string json_str,	
		int useNativeUIFlag,int audioVolumeUIFlag,int audioTrackUIFlag,int progressBarUIFlag,	
		int channelNoUIFlag,int allowTrickmodeFlag,int speed,int volume,int pipmode,void *mpobject,	
		MediaplayerStateChangeCallback playstate_func);

	/*析构函数*/
	~ StreamMp4Player();

	int getchannelurlfromjson();
	
	int getmediaurlfromjson();

	play_state getPlayState(){return playerstate;}

	void player_setvolume(int volumeindex);

	/*	要求终端访问指定的频道，并立即返回。
	对由本地设置为跳过的频道，也返回-1。
	频道地址为通过SetConfig设置的频道列表中的地址	
	Userchannelid，整数，表示用户频道号。
	返回值：0，表示成功；
	-1：表示频道号无效。
	*/
	int joinChannel(int userchannelid, string& json_str);

	/*要求终端离开指定的频道，并立即返回
	返回值：0，表示成功；
	-1：表示频道号无效。
	*/
	int leaveChannel();
	
	/*	0: 允许TrickMode操做
		1: 不允许TrickMode操作 (默认值)
	*/
	void setAllowTrickmodeFlag (int allowTrickmode);

/*	从媒体起始点开始播放。
	对TV channel，以实时TV的方式开始播放。
	*/
	 int playFromStart() ;

	/*	从当前媒体的某个时间点开始播放媒体
	（对playlist而言是指当前已经选中的媒体）。
	对实时播放的TV channel该调用无效，
	但对处于时移状态的TV channel有效。
	-type:
	1：参见RFC2326中的Normal Play Time (NPT)
	2：参见RFC2326中的Absolute Time (Clock Time)
	-timestamp:
	参见RFC2326中的Normal Play Time (NPT)和Absolute Time (Clock Time)两种时间类型的格式
	timestamp:对VoD而言是从媒体起始点开始计算的相对时间；对TVoD等有时间基的媒体而言就是绝对时间。
	-speed：
	播放速度，可选参数。
	*/
	 int playByTime(int type, int timestamp,int speed) ;

	/*暂停正在播放的媒体*/
	 int pause() ;

	/*	-speed：2至32*/
	 int fastForward(int speed) ;

	/*	-speed：-2至-32*/
	 int fastRewind(int speed) ;

	/*从当前媒体的暂停/快进/快退状态恢复正常播放。*/
	 int resume() ;

	/*跳到媒体末端播放*/
	 int gotoEnd() ;//可选的

	/*跳到媒体起始点播放*/
	 int gotoStart();//可选的

	/*停止正在播放的媒体。并释放机顶盒本地播放器的相关资源。*/
	 int stop()  ;

	/*停止正在播放的媒体。并释放机顶盒本地播放器的相关资源。*/
	 int player_setwindowparms(int top, int left, int width, int height)  ;

	/*切换声道，循环进行。*/
	 int switchAudioChannel() ;//可选的

	/*切换音轨，循环进行。*/
	 int switchAudioTrack();//可选的

	/*切换字幕，循环进行。*/
	 int switchSubtitle();//可选的

	/*获取当前播放的媒体的总时长,整数，以秒为单位*/
	 int getMediaDuration();

	/*	获取媒体播放到的当前时间点
	对VoD为从该媒体起始点开始计算的相对时间，以秒为单位；
	对TVoD为当前播放点的绝对时间；
	对Channel而言无意义
	参见RFC2326中的Normal Play Time (NPT)和Absolute Time (Clock Time)两种时间类型的格式
	*/
	 unsigned long getCurrentPlayTime();

	/*播放器的当前播放模式
	其中至少包括“播放模式”和“模式相关参数” 两类信息，
	播放模式分：Normal Play，Pause，Trickmode；
	当模式为Trickmode时必须带2x/-2x, 4x/-4x, 8x/-8x, 16x/-16x, 32x/-32x参数来表示快进/快退的速度参数
	*/
	 char * getPlaybackMode() ;

	/*	用于获取上一次发生的错误代码*/
	 int GetLastError();//可选

	 int SetLastEvent(	MediaPlayErrorType inerrortype);//可选

	/*返回机顶盒当前播放的频道号，
	不能获得有效的频道号时，返回-1。
	*/

	int setSingleMedia(string &mediaStr);

	 int getChannelNum(); 

 	int setSingleMedia(string mediaStr);

	void main_vod_player();

	static void mp4_player_thread(void *parm)
	{
		StreamMp4Player *client = (StreamMp4Player *)parm;

		client->main_mp4_player();
	}
	
	void main_mp4_player();

	
private:
	int Init_StreamMp4Player();
	
	int Release_Mp4MediaPlayer();
	

	
	int Read (unsigned char * buf, unsigned long len );

	void input_tvodtime(char *a);

	PLAYERProcess_cmd_goto process_tvod_msg();
	
	void process_unicast_msg(int cmd, PLAYERProcess_cmd_goto *gotocmd);

	int sendto_mediaplayermsg(int cmd,char *data=NULL, int datalen=0);

	int rcvfrom_mediaplayermsg(int *cmd, char *data=NULL);

	int PlayerRecvStopKey();

	void Set_Player_Window();

	void check_prebuffering_state(unsigned int buffersize);

	void process_common_msg(int *cmdtype, int *processed);	

	
private: //vod相关函数

	int clean_socketdata ();

	unsigned long long Get_LastTimeStamp(){ return u_lasttimestamp;}
	
	void input_time(char a);

	double Get_LastPlaytime(){return playing_time;}
	

	void show_time(int hour, int minute, int second);
	
	void show_step();

		
	void init_play();
	
	unsigned int check_interleave(rtp_packet * prtppacket);
	
	int Init_Unicast();
	
	int Quit_Unicast();

	int send_mp4_audio(rtp_packet *prtp_packet, unsigned char * databuf, int count);

	int send_mp4_video(rtp_packet *prtp_packet, unsigned char * databuf, int count);

	int rt_value;
	string playbackmode;
	int playspeed;
	int sallowTrickmode;
	Message recvmsg;
	Message sendmsg;
	pthread_t mediaplayer_thread;
	int msgid;
	int status_network;
	int status_decoder;
	int prebuf_max;
	string mediaurl;//with starttime
	string vodurl;//without starttime
	unsigned int AAC_SampleRateFromDSI;
	int 	Mp4PlayerHandle;
	RTSPClient RC;
	struct rtp * session;
	int porttimes;
	mp4parameter_t mp4parameter;
	double playing_time;	
	unsigned long media_start_time;
	unsigned long long time_out;
	play_state playerstate;
	int volumesize;
	int trickmode_id;
	int prebuf_level;
	string media_info;
	string audio_info;
	struct MP4PlayerConfig configinfo;
	int firsttime_a;// 1
	int firsttime_v;// 1
	unsigned long video_ptsoff;// = 0;
	unsigned long audio_ptsoff;// = 0;
	bool FirstSystemTimeStamp;// = TRUE;
	unsigned long firstVideoPTS;
	unsigned long firstAudioPTS;

	int NumPackets;
	
	unsigned char resumepauseplay;
	unsigned char resumetrickplay;
	unsigned char dropdummyvideo;
	unsigned char dropdummyaudio;
	unsigned char dummyaudio[32];
	unsigned char dummyvideo[32];

	unsigned long long u_lasttimestamp;
	
	bool videoEOS;// = FALSE;
	bool audioEOS;// = FALSE;

	double duration;
	
	bool prebufferingDone;// = FALSE;

	rtp_event event;

	struct audiopacket a_packet;
	DSPmp4Sample audiosample;
	DSPmp4Sample videosample;


};

#endif
