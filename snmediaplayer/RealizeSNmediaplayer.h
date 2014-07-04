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



typedef enum
{
	MEDIA_TVPLAY,
	MEDIA_VODPLAY
}StreamType;



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

//消息队列 key值
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
	PLAYERSTATE_BUFFERING,//当前缓冲媒体是得到服务器的其他数据
	PLAYERSTATE_WAITFORSESSION,//等待建立连接，但服务器不发送数据waiting fro session to begin
	PLAYERSTATE_ITEMFINISH,//mediaended media item has completed playback
	PLAYERSTATE_PREPARENEW,//transitioning preparing new media item
	PLAYERSTATE_READY,//Ready ready to begin playing
	PLAYERSTATE_RECONECT//reconnecting to stream
}play_state;

typedef enum media_type_
{
	MEDIA_TYPE_VOD=0,
	MEDIA_TYPE_FILM=1     //时移
}media_type;

#define PLAYER_Full_Win_X 0 
#define PLAYER_Full_Win_Y 0
#define PLAYER_Full_Win_WIDTH 10000
#define PLAYER_Full_Win_HEGHT 10000

class RealizeSNmediaplayer
{
	
public:
	/*构造函数*/
	RealizeSNmediaplayer();
	
	RealizeSNmediaplayer(int instanceid, int videoDisplayMode, int playmode,
	int height, int width, int left, int top, int muteFlag,  int muteuiflag, int subtitleFlag, 
	int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag,string json_str,
	int useNativeUIFlag,int audioVolumeUIFlag,int audioTrackUIFlag,int progressBarUIFlag,
	int channelNoUIFlag,int allowTrickmodeFlag,int speed,int volume,int pipmode,
	MediaplayerStateChangeCallback playstate_func);//把json存下来

//以下为虚函数
	/*析构函数*/
	virtual ~RealizeSNmediaplayer();

	virtual play_state getPlayState()=0;

	virtual void player_setvolume(int volumeindex)=0;

	/*	要求终端访问指定的频道，并立即返回。
	对由本地设置为跳过的频道，也返回-1。
	频道地址为通过SetConfig设置的频道列表中的地址	
	Userchannelid，整数，表示用户频道号。
	返回值：0，表示成功；
	-1：表示频道号无效。
	*/
	virtual int joinChannel(int userchannelid,string& channel_info)=0;

	/*要求终端离开指定的频道，并立即返回
	返回值：0，表示成功；
	-1：表示频道号无效。
	*/
	virtual int leaveChannel()=0;
	
	/*	0: 允许TrickMode操做
		1: 不允许TrickMode操作 (默认值)
	*/
	virtual void setAllowTrickmodeFlag (int allowTrickmode)=0;

/*	从媒体起始点开始播放。
	对TV channel，以实时TV的方式开始播放。
	*/
	virtual int playFromStart() =0;

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
	virtual int playByTime(int type, int timestamp,int speed) =0;

	/*暂停正在播放的媒体*/
	virtual int pause() =0;

	/*	-speed：2至32*/
	virtual int fastForward(int speed) =0;

	/*	-speed：-2至-32*/
	virtual int fastRewind(int speed) =0;

	/*从当前媒体的暂停/快进/快退状态恢复正常播放。*/
	virtual int resume() =0;

	/*跳到媒体末端播放*/
	virtual int gotoEnd() =0;//可选的

	/*跳到媒体起始点播放*/
	virtual int gotoStart()=0;//可选的

	/*停止正在播放的媒体。并释放机顶盒本地播放器的相关资源。*/
	virtual int stop()=0;

	/*停止正在播放的媒体。并释放机顶盒本地播放器的相关资源。*/
	virtual int player_setwindowparms(int top, int left, int width, int height)=0;

	/*切换声道，循环进行。*/
	virtual int switchAudioChannel()=0;//可选的

	/*切换音轨，循环进行。*/
	virtual int switchAudioTrack()=0;//可选的

	/*切换字幕，循环进行。*/
	virtual int switchSubtitle()=0;//可选的

	/*获取当前播放的媒体的总时长,整数，以秒为单位*/
	virtual int getMediaDuration()=0;

	/*	获取媒体播放到的当前时间点
	对VoD为从该媒体起始点开始计算的相对时间，以秒为单位；
	对TVoD为当前播放点的绝对时间；
	对Channel而言无意义
	参见RFC2326中的Normal Play Time (NPT)和Absolute Time (Clock Time)两种时间类型的格式
	*/
	virtual unsigned long getCurrentPlayTime()=0;

	/*播放器的当前播放模式
	其中至少包括“播放模式”和“模式相关参数” 两类信息，
	播放模式分：Normal Play，Pause，Trickmode；
	当模式为Trickmode时必须带2x/-2x, 4x/-4x, 8x/-8x, 16x/-16x, 32x/-32x参数来表示快进/快退的速度参数
	*/
	virtual char * getPlaybackMode() =0;

	/*	用于获取上一次发生的错误代码*/
	virtual int GetLastError()=0;//可选

	/*返回机顶盒当前播放的频道号，
	不能获得有效的频道号时，返回-1。
	*/
	virtual int getChannelNum() =0;

	virtual int setSingleMedia(string mediaStr)=0;


//以上为虚函数
	/*	根据videoDisplayMode, vedioDisplayArea属性, 调整视频的显示。*/
	int refreshVideoDisplay();

	/*留作厂商的命令扩展，参数由厂商自定义*/
	virtual int sendVendorSpecificCommand(char *xml){return 0;}

	/*返回音频PID。
	如果正在播放返回当前播放的PID，否则返回缺省/第一个PID。
	*/
	int getAudioPID();//可选的

	/*	返回一个包含所有音频PID信息的对象。
	对象包含以下信息： 
	pids 当前媒体流中包含的音频PID个数 
	pidarr[1..pids]–音频PID数组： 
	AudioPID–PID值（数值） 
	AudioLanguage–PID的语言标志（字符串）
	*/
	pid_info *getAudioPIDs();//可选的

	/*指定使用的音频pid*/
	void setAudioPID(int pid);

	/*返回字幕PID。如果正在播放返回当前播放的PID，否则返回缺 省/第一个PID*/
	int getSubtitlePID();

	/*	返回一个包含所有字幕PID信息的对象。对象包含以下信息： 
	pids–当前媒体流中包含的字幕PID个数 
	pidarr[1..pids]–字幕PID数组： 
	SubtitlePID -PID值（数值） 
	SubtitleLanguage- PID的语言标志（字符串）
	*/
	pid_info *getSubtitlePIDs();

	/*指定使用的字幕pid*/
	void setSubtitlePID(int pid);

	/*Volume：0-100，表示音量，0为静音，100为最大音量*/
	void setVolume(int volume_value);
	
	
	/*设置播放器播放模式
	   0: 单媒体的播放模式 (默认值)
	   1: 播放列表的播放模式
	*/
	void setSingleOrPlaylistMode(int playmode){real_playmode=playmode;return;}

	/*获取播放器播放模式*/
	int getSingleOrPlaylistMode(){return real_playmode;}


	/*0: 按setVideoDisplayArea()中设定的Height, Width, Left, Top属性所指定的位置和大小来显示视频
      	   1: 全屏显示，按全屏高度和宽度显示(默认值)
      	   2: 按宽度显示，指在不改变原有图像纵横比的情况下按全屏宽度显示
      	   3: 按高度显示，指在不改变原有图像纵横比的情况下按全屏高度显示
      	   255: 视频显示窗口将被关闭。它将在保持媒体流连接的前提下，隐藏视频窗口。如果流媒体播放没有被暂停，将继续播放音频。

	   MediaPlayer对象对应的视频窗口的显示模式.
         每次调用该函数后，视频显示窗口并不会被立即重新刷新
         以反映更改后的显示效果只有等到显式调用refreshVideoDisplay()后
         才会刷新	
      */
	void setVideoDisplayMode(int videoDisplayMode);
	int getVideoDisplayMode(){return real_videoDisplayMode;}

	/*	MediaPlayer对象对应的视频窗口的显示模式.
	每次调用该函数后，视频显示窗口并不会被立即重新刷新
	以反映更改后的显示效果只有等到显式调用refreshVideoDisplay()后
	才会刷新
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

	/*	0:  设置为有声 (默认值)
		1:  设置为静音
	*/
	void setMuteFlag(int mute_flag);//此函数需要维护muteflag

	int getMuteFlag(){return real_muteflag;}

	/*	0: 不使能Player的本地UI显示功能
		1: 使能Player的本地UI显示功能 (默认值)
	*/
	void setNativeUIFlag (int nativeUI_flag){real_nativeUIFlag = nativeUI_flag;}//可选的属性,此函数需要维护nativeUIFlag

	int getNativeUIFlag(){return real_nativeUIFlag;}//可选的属性

	/*	0: 不使能静音提示的本地UI显示功能
		1: 使能静音提示的本地UI显示功能（默认值）
	*/
	void setMuteUIFlag (int muteUI_flag){real_muteUIFlag = muteUI_flag;}//可选的属性,此函数需要维护muteUIFlag
	
	int getMuteUIFlag(){return real_muteUIFlag;}//可选的属性

	/*	0: 不使能音量调节的本地UI显示功能
		1: 使能音量调节的本地UI显示功能（默认值）
	*/
	void setAudioVolumeUIFlag (int audioVolumeUI_flag){real_audioVolumeUIFlag = audioVolumeUI_flag;}//可选的属性,此函数需要维护audioVolumeUIFlag
	
	int getAudioVolumeUIFlag(){return real_audioVolumeUIFlag;}//可选的属性

	/*	0: 不使能音轨选择的本地UI显示功能
		1: 使能音轨选择的本地UI显示功能（默认值）
	*/
	virtual void setAudioTrackUIFlag (int audioTrackUI_flag){real_audioTrackUIFlag = audioTrackUI_flag;}//可选的属性,此函数需要维护audioTrackUIFlag
	
	int getAudioTrackUIFlag(){return real_audioTrackUIFlag;}//可选的属性

	/*    0设置为循环播放节目(默认值)
		1不循环,单次播放
	*/
	void setCycleFlag (int cycleFlag){return;}

	/*    0设置为随机播放节目(默认值)
		1顺序播放
	*/
	void setRandomFlag (int randomFlag){return;}

	/*	0: 不使能进度条的本地UI显示功能
		1: 使能进度条的本地UI显示功能（默认值）
	*/
	void setProgressBarUIFlag (int progressBarUI_flag){real_progressBarUIFlag = progressBarUI_flag;}//可选的属性,此函数需要维护progressBarUIFlag
	
	int getProgressBarUIFlag(){return real_progressBarUIFlag;}//可选的属性

	/*	0: 不使能频道号的本地UI显示功能
		1: 使能频道号的本地UI显示功能（默认值）
	*/
	void setChannelNoUIFlag (int channelNoUI){real_channelNoUIFlag = channelNoUI;}//可选的属性
	
	int getChannelNoUIFlag(){return real_channelNoUIFlag;}//可选的属性

	/*	0:  不显示字幕 (默认值)
		1:  显示字幕
	*/
	void setSubtitileFlag (int subtitile){real_subtitileFlag = subtitile;}//可选的属性
	
	int getSubtitileFlag(){return real_subtitileFlag;}//可选的属性

	/*设置当前播放器为画中画模式
	mode:1画中画；其他:标准模式*/
	void setPIPVideoMode(int pipmode){return;}
	/*	0－100之间的整数值，0表示不透明，100表示完全透明。
		(默认值为0)
	*/
	void setVideoAlpha (int vAlpha){return;}//可选的属性
	
	int getVideoAlpha(){return real_videoAlpha;}//可选的属性

	int getAllowTrickmodeFlag(){return real_allowTrickmodeFlag;}

	/*	留作厂商的扩展属性设置和获取，格式由厂商自己确定*/
	void setVendorSpecificAttr (char * vendorSpecificAttr){return;}
	
	string getVendorSpecificAttr(){return PLAYERVENDORINFO;}

	/*	获取当前播放的媒体的标识， 
	MediaCode是媒体的唯一标识（节目，频道等），
	使用系统定义的ContentID。当播放器在播放列表模式时，
	MediaCode是动态改变的。
	*/
	char *getMediaCode();


	/*	获取当前的声道类型
	字符串Left、Right、Stereo、JointStereo
	*/
	char * getCurrentAudioChannel();

	/*获取当前的的音轨类型
	字符串Chinese、English等
	*/
	char * getAudioTrack();

	/*获取当前选择的字幕类型
	字符串Chinese、English等
	*/
	char * getSubtitle();

	/*获取播放列表中的媒体数量*/
	int getMediaCount();//可选的

	/*获取当前播的媒体，在播放列表中的索引值
	0：表示媒体列表的顶端
	*/
	int getCurrentIndex();//可选的

	/*获取当前播放的媒体的EntryID*/
	char * getEntryID();//可选的

	/*字符串，用JSON格式的字符串，封装播放列表数据。*/
	char *getPlaylist();//可选的

	/*	nativePlayerInstanceID:一个字节长度的无符号整数，0－255
		返回：
		0： 表示绑定成功；
		－1： 表示绑定失败；
		其他值未定义
	*/
	int bindNativePlayerInstance(unsigned char nativePlayerInstanceID);

	/*初始化MediaPlayer的属性。
	当新创建的Media Player是为了和已有的STB本地某个媒体播放实例
	（由先前其它页面中创建Media Player对象时创建的）进行绑定，
	无需调用该函数。
	返回：
	0： 表示绑定成功；
	－1： 表示绑定失败；
	其他值未定义
	*/
	int initMediaPlayer(unsigned char nativePlayerinstanceID, int playlistFlag, int videoDisplayMode, 
		int height, int width, int left, int top, int muteFlag,  int useNativeUIFlag, int subtitleFlag, 
		int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag);


	/*获取stb本地播放各种实例的instanceID
	type:0缺省实例；1主播放器；2画中画播放器；其他值预留*/
	unsigned char getNativePlayerInstanceIDByType(int type);
	/*设置单个播放的媒体*/

	/*在播放列表末端，添加单个媒体。
	index：如果为负数（如-1），表示默认添加在列表尾部（Append）；
	如指定的index值大于或等于playlist中已有媒体数目，则添加至playlist尾部；
	其它情况为插入列表某个位置（Insert）；
	index会随着条目的移动/增加/删除而动态变化，
	由STB保证当前playlist中所有条目的index是从0按序加一递增的。
	对于entry_id：唯一标识playlist中每个媒体条目，
	一旦成功加入某个媒体后，该值不会变化。
	*/
	int addSingleMedia(int index, char *mediaStr);//可选的

	//在播放列表末端，添加一批媒体。
	int addBatchMedia(char *batchMediaStr);//	可选的


	/*从播放列表中删除指定媒体
	播放列表中某个媒体条目的唯一标识（在添加媒体时设置并且在该播放列表中保持不变）
	*/
	int removeMediaByEntryID(char *entryID);//可选的

	/*从播放列表中删除指定媒体
	index，整数，媒体在媒体列表中的索引值
	*/
	int removeMediaByIndex(int index);//	可选的

	/*清空播放列表
	返回值：0，表示成功；
	-1：表示失败。
	*/
	int clearAllMedia();//	可选的

	/*	根据index将指定的媒体在播放列表中移动
	entryID: 播放列表中某个媒体条目的唯一标识。
	toIndex，整数，需要移动到的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaByIndex(char *entryID, int toIndex);//	可选的

	/*根据偏移量将指定的媒体在播放列表中移动
	entryID: 播放列表中某个媒体条目的唯一标识。
	offset，偏移量，正整数表示从指定媒体向列表末端移动，负整数表示从指定媒体向列表起始端移动
	*/
	int moveMediaByOffset(char *entryID, int offset);//可选的

	/*根据index将指定的媒体在播放列表中移动
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	toIndex，整数，需要移动到的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaByIndex1(int index, int toIndex);//可选的

	/*根据偏移量将指定的媒体在播放列表中移动
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	offset，偏移量，正整数表示从指定媒体向列表末端移动，负整数表示从指定媒体向列表起始端移动
	*/
	int moveMediaByOffset1(int index, int offset);//可选的

	/*将指定的媒体下移*/
	int moveMediaToNext(char *entryID);//	可选的

	/*将指定的媒体上移*/
	int moveMediaToPrevious(char *entryID);//	可选的

	/*将指定的媒体移到列表顶端*/
	int moveMediaToFirst(char *entryID);//	可选的

	/*将指定的媒体移到列表末端*/
	int moveMediaToLast(char *entryID);//	可选的

	/*将指定的媒体下移
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToNext1(int index);//	可选的

	/*将指定的媒体上移
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToPrevious1(int index);//	可选的

	/*将指定的媒体移到列表顶端
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToFirst1(int index);//	可选的

	/*将指定的媒体移到列表末端
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToLast1(int index);//	可选的

	/*按媒体在播放列表中的索引选中为当前候选播放节目
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int selectMediaByIndex(int index);//	可选的

	/*按与当前媒体索引的偏移量选中媒体，作为当前候选播放节目
	offset，偏移量，正整数表示从当前媒体向列表末端跳转，负整数表示从当前媒体向列表起始端跳转
	*/
	int selectMediaByOffset(int offset);//	可选的

	/*选取播放列表中的下一个媒体，作为当前候选播放节目*/
	int selectNext();//	可选的

	/*选取播放列表中的上一个媒体，作为当前候选播放节目*/
	int selectPrevious();//	可选的

	/*选取播放列表中的第一个媒体，作为当前候选播放节目*/
	int selectFirst();//	可选的

	/*选取播放列表中的最后一个媒体，作为当前候选播放节目*/
	int selectLast();//	可选的

	/*按某个媒体条目的唯一标识选中为当前候选播放节目*/
	int SelectMediaByEntryID(char *entryID);//	可选的


	/*返回值：当前系统音量，0-100*/
	int getVolume(){return real_volume;}

	/*	ioStr:要设置的参数的名称。（具体意义由机顶盒定义）
		wrStr：要设置的参数的值。
	*/
	int set(string ioStr, string wrStr);//扩展使用

	/*ioStr:要读取的参数的名称。（具体意义由机顶盒定义）*/
	int get(string ioStr);//扩展使用

	static int getevent();


protected:
	static MediaPlayErrorType eventtype;

	int real_instanceid;
	string real_json_str;
	int real_playmode;//	0: 单媒体的播放模式 (默认值);1: 播放列表的播放模式
	/*0: 按setVideoDisplayArea()中设定的Height, Width, Left, Top属性所指定的位置和大小来显示视频
      	   1: 全屏显示，按全屏高度和宽度显示(默认值)
      	   2: 按宽度显示，指在不改变原有图像纵横比的情况下按全屏宽度显示
      	   3: 按高度显示，指在不改变原有图像纵横比的情况下按全屏高度显示
      	   255: 视频显示窗口将被关闭。它将在保持媒体流连接的前提下，隐藏视频窗口。如果流媒体播放没有被暂停，将继续播放音频。
      	*/
	int real_videoDisplayMode;
	int iftvfullscreen;
	PlayerRect real_videoDisplayrect;
	int real_muteflag;//0:  设置为有声 (默认值);1:  设置为静音
	int real_nativeUIFlag;//0: 不使能Player的本地UI显示功能;1: 使能Player的本地UI显示功能 (默认值)
	int real_muteUIFlag;//: 不使能静音提示的本地UI显示功能;1: 使能静音提示的本地UI显示功能（默认值）
	int real_audioVolumeUIFlag;//0: 不使能音量调节的本地UI显示功能;1: 使能音量调节的本地UI显示功能（默认值）
	int real_audioTrackUIFlag;//0: 不使能音轨选择的本地UI显示功能;1: 使能音轨选择的本地UI显示功能（默认值）
	int real_progressBarUIFlag;//0: 不使能进度条的本地UI显示功能;1: 使能进度条的本地UI显示功能（默认值）
	int real_channelNoUIFlag;//0: 不使能频道号的本地UI显示功能;1: 使能频道号的本地UI显示功能（默认值）
	int real_subtitileFlag;//0:  不显示字幕 (默认值);	1:  显示字幕
	int real_videoAlpha;//0－100之间的整数值，0表示不透明，100表示完全透明(默认值为0)
	int real_allowTrickmodeFlag;//	0: 允许TrickMode操做;1: 不允许TrickMode操作 (默认值)
	int real_playlistFlag;
	int real_cycleFlag;//0设置为循环播放节目(默认值);1不循环,单次播放
	int real_randomFlag;
	int real_autoDelFlag;
	string real_mediacode;
	int real_speed;//-32,-16,-8,-4,-2,0,...32
	int real_volume;//0-100，表示音量，0为静音，100为最大音量
	int real_pipmode;
	void * real_mpobject;
	MediaplayerStateChangeCallback real_playstate_func;
	media_type playing_media_type;
	

};
#endif
