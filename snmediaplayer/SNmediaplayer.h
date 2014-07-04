#ifndef _SNMEDIAPLAYER_H
#define _SNMEDIAPLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "RealizeSNmediaplayer.h"
using namespace std;

class SNmediaplayer
{
	
public:
	/*构造函数*/
	SNmediaplayer(void *mpobject, MediaplayerStateChangeCallback playstate_func);//判断后instanceid赋新值，默认为0

	/*析构函数*/
	virtual ~SNmediaplayer();

	/*获得STB本地播放器实例的instanceID*/
	unsigned char getNativePlayerInstanceID(){return sn_instanceid;}

	/*获取播放器状态*/
	play_state getPlayState();

	/*	nativePlayerInstanceID:一个字节长度的无符号整数，0－255
		返回：
		0： 表示绑定成功；
		－1： 表示绑定失败；
		其他值未定义
	*/
	int bindNativePlayerInstance(unsigned char nativePlayerInstanceID);//修改instanceid值

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

	/*释放机顶盒MediaPlayer的对象，结束对应MediaPlayer的生命周期。
	nativePlayerInstanceID:	一个字节长度的无符号整数，0－255
	返回：
	0： 表示释放成功；
	－1： 表示释放失败；
	其他值未定义
	*/
	int releaseMediaPlayer (unsigned char nativePlayerInstanceID);//释放real_mediaplayer[nativePlayerinstanceID]，并置零

	/*彻底关闭本地播放器*/
	int releaseNativePlayer(unsigned char nativePlayerInstanceID);//同上

	/*获取stb本地播放各种实例的instanceID
	type:0缺省实例；1主播放器；2画中画播放器；其他值预留*/
	unsigned char getNativePlayerInstanceIDByType(int type);
	/*设置单个播放的媒体*/
	int setSingleMedia(string mediaStr);

	/*在播放列表末端，添加单个媒体。
	index：如果为负数（如-1），表示默认添加在列表尾部（Append）；
	如指定的index值大于或等于playlist中已有媒体数目，则添加至playlist尾部；
	其它情况为插入列表某个位置（Insert）；
	index会随着条目的移动/增加/删除而动态变化，
	由STB保证当前playlist中所有条目的index是从0按序加一递增的。
	对于entry_id：唯一标识playlist中每个媒体条目，
	一旦成功加入某个媒体后，该值不会变化。
	*/
	int addSingleMedia(int index, char *mediaStr){return 0;}//可选的

	//在播放列表末端，添加一批媒体。
	int addBatchMedia(char *batchMediaStr){return 0;}//	可选的

	/*	要求终端访问指定的频道，并立即返回。
	对由本地设置为跳过的频道，也返回-1。
	频道地址为通过SetConfig设置的频道列表中的地址	
	Userchannelid，整数，表示用户频道号。
	返回值：0，表示成功；
	-1：表示频道号无效。
	*/
	int joinChannel(int userchannelid);

	/*要求终端离开指定的频道，并立即返回
	返回值：0，表示成功；
	-1：表示频道号无效。
	*/
	int leaveChannel();

	/*从播放列表中删除指定媒体
	播放列表中某个媒体条目的唯一标识（在添加媒体时设置并且在该播放列表中保持不变）
	*/
	int removeMediaByEntryID(char *entryID){return 0;};//可选的

	/*从播放列表中删除指定媒体
	index，整数，媒体在媒体列表中的索引值
	*/
	int removeMediaByIndex(int index){return 0;}//	可选的

	/*清空播放列表
	返回值：0，表示成功；
	-1：表示失败。
	*/
	int clearAllMedia(){return 0;}//	可选的

	/*	根据index将指定的媒体在播放列表中移动
	entryID: 播放列表中某个媒体条目的唯一标识。
	toIndex，整数，需要移动到的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaByIndex(char *entryID, int toIndex){return 0;}//	可选的

	/*根据偏移量将指定的媒体在播放列表中移动
	entryID: 播放列表中某个媒体条目的唯一标识。
	offset，偏移量，正整数表示从指定媒体向列表末端移动，负整数表示从指定媒体向列表起始端移动
	*/
	int moveMediaByOffset(char *entryID, int offset){return 0;}//可选的

	/*根据index将指定的媒体在播放列表中移动
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	toIndex，整数，需要移动到的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaByIndex1(int index, int toIndex){return 0;}//可选的

	/*根据偏移量将指定的媒体在播放列表中移动
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	offset，偏移量，正整数表示从指定媒体向列表末端移动，负整数表示从指定媒体向列表起始端移动
	*/
	int moveMediaByOffset1(int index, int offset){return 0;}//可选的

	/*将指定的媒体下移*/
	int moveMediaToNext(char *entryID){return 0;}//	可选的

	/*将指定的媒体上移*/
	int moveMediaToPrevious(char *entryID){return 0;}//	可选的

	/*将指定的媒体移到列表顶端*/
	int moveMediaToFirst(char *entryID){return 0;}//	可选的

	/*将指定的媒体移到列表末端*/
	int moveMediaToLast(char *entryID){return 0;}//	可选的

	/*将指定的媒体下移
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToNext1(int index){return 0;}//	可选的

	/*将指定的媒体上移
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToPrevious1(int index){return 0;}//	可选的

	/*将指定的媒体移到列表顶端
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToFirst1(int index){return 0;}//	可选的

	/*将指定的媒体移到列表末端
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int moveMediaToLast1(int index){return 0;}//	可选的

	/*按媒体在播放列表中的索引选中为当前候选播放节目
	index，整数，指定的媒体在播放列表中的索引值。0：表示播放列表的顶端。
	*/
	int selectMediaByIndex(int index){return 0;}//	可选的

	/*按与当前媒体索引的偏移量选中媒体，作为当前候选播放节目
	offset，偏移量，正整数表示从当前媒体向列表末端跳转，负整数表示从当前媒体向列表起始端跳转
	*/
	int selectMediaByOffset(int offset){return 0;}//	可选的

	/*选取播放列表中的下一个媒体，作为当前候选播放节目*/
	int selectNext(){return 0;}//	可选的

	/*选取播放列表中的上一个媒体，作为当前候选播放节目*/
	int selectPrevious(){return 0;}//	可选的

	/*选取播放列表中的第一个媒体，作为当前候选播放节目*/
	int selectFirst(){return 0;}//	可选的

	/*选取播放列表中的最后一个媒体，作为当前候选播放节目*/
	int selectLast(){return 0;}//	可选的

	/*按某个媒体条目的唯一标识选中为当前候选播放节目*/
	int SelectMediaByEntryID(char *entryID){return 0;}//	可选的

	/*	从媒体起始点开始播放。
	对TV channel，以实时TV的方式开始播放。
	*/
	int playFromStart();

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
	int playByTime(int type, int timestamp,int speed);

	/*暂停正在播放的媒体*/
	int pause()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->pause();
	}

	/*	-speed：2至32*/
	int fastForward(int speed)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->fastForward(speed);
	}

	/*	-speed：-2至-32*/
	int fastRewind(int speed)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->fastRewind(speed);
	}

	/*从当前媒体的暂停/快进/快退状态恢复正常播放。*/
	int resume()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->resume();
	}

	/*跳到媒体末端播放*/
	int gotoEnd()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->gotoEnd();
	}//可选的

	/*跳到媒体起始点播放*/
	int gotoStart()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->gotoStart();
	}//可选的

	/*停止正在播放的媒体。并释放机顶盒本地播放器的相关资源。*/
	int stop();

	/*	根据videoDisplayMode, vedioDisplayArea属性, 调整视频的显示。*/
	int refreshVideoDisplay();

	/*切换声道，循环进行。*/
	int switchAudioChannel()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->switchAudioChannel();

	}//可选的

	/*切换音轨，循环进行。*/
	int switchAudioTrack()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->switchAudioTrack();
	}//可选的

	/*切换字幕，循环进行。*/
	int switchSubtitle()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->switchSubtitle();
	}//可选的

	/*留作厂商的命令扩展，参数由厂商自定义*/
	int sendVendorSpecificCommand(char *xml){return 0;}

	/*返回音频PID。
	如果正在播放返回当前播放的PID，否则返回缺省/第一个PID。
	*/
	int getAudioPID()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getAudioPID();
	}//可选的

	/*	返回一个包含所有音频PID信息的对象。
	对象包含以下信息： 
	pids 当前媒体流中包含的音频PID个数 
	pidarr[1..pids]–音频PID数组： 
	AudioPID–PID值（数值） 
	AudioLanguage–PID的语言标志（字符串）
	*/
	pid_info *getAudioPIDs()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getAudioPIDs();
	}//可选的

	/*指定使用的音频pid*/
	void setAudioPID(int pid)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return ;
		return sn_real_mediaplayer[sn_instanceid]->setAudioPID(pid);
	}

	/*返回字幕PID。如果正在播放返回当前播放的PID，否则返回缺 省/第一个PID*/
	int getSubtitlePID()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getSubtitlePID();
	}

	/*	返回一个包含所有字幕PID信息的对象。对象包含以下信息： 
	pids–当前媒体流中包含的字幕PID个数 
	pidarr[1..pids]–字幕PID数组： 
	SubtitlePID -PID值（数值） 
	SubtitleLanguage- PID的语言标志（字符串）
	*/
	pid_info *getSubtitlePIDs()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getSubtitlePIDs();
	}

	/*指定使用的字幕pid*/
	void setSubtitlePID(int pid)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return;
		return sn_real_mediaplayer[sn_instanceid]->setSubtitlePID(pid);
	}

	/*Volume：0-100，表示音量，0为静音，100为最大音量*/
	void setVolume(int volume_value);

	/*返回值：当前系统音量，0-100*/
	int getVolume();

	/*	ioStr:要设置的参数的名称。（具体意义由机顶盒定义）
		wrStr：要设置的参数的值。
	*/
	int set(string ioStr, string wrStr){return 0;};//扩展使用

	/*ioStr:要读取的参数的名称。（具体意义由机顶盒定义）*/
	string get(string ioStr){return NULL;};//扩展使用

	/*设置播放器播放模式
	   0: 单媒体的播放模式 (默认值)
	   1: 播放列表的播放模式
	*/
	void setSingleOrPlaylistMode(int plmode){return;}//{return sn_real_mediaplayer[sn_instanceid]->setSingleOrPlaylistMode(plmode);}

	/*获取播放器播放模式*/
	int getSingleOrPlaylistMode(){return 0;}//{return sn_real_mediaplayer[sn_instanceid]->getSingleOrPlaylistMode();}


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
	void setVideoDisplayMode(int vDisplayMode);

	int getVideoDisplayMode(){return sn_videoDisplayMode;}

	/*	MediaPlayer对象对应的视频窗口的显示模式.
	每次调用该函数后，视频显示窗口并不会被立即重新刷新
	以反映更改后的显示效果只有等到显式调用refreshVideoDisplay()后
	才会刷新
	*/
	void setVideoDisplayArea (int left, int top, int width, int height);
	
	int getVideoDisplayLeft(){return sn_videoDisplayrect.x;}

	int getVideoDisplayTop(){return sn_videoDisplayrect.y;}

	int getVideoDisplayWidth(){return sn_videoDisplayrect.width;}

	int getVideoDisplayHeight(){return sn_videoDisplayrect.height;}

	/*	0:	设置为有声 (默认值)
		1:	设置为静音
	*/
	void setMuteFlag(int mute_flag);

	int getMuteFlag();//jiangy 

	/*	0: 不使能Player的本地UI显示功能
		1: 使能Player的本地UI显示功能 (默认值)
	*/
	void setNativeUIFlag (int nativeUI_flag);
	int getNativeUIFlag(){return sn_nativeUIFlag;}//可选的属性

	/*	0: 不使能静音提示的本地UI显示功能
		1: 使能静音提示的本地UI显示功能（默认值）
	*/
	void setMuteUIFlag (int muteUI_flag);

	int getMuteUIFlag(){return sn_muteUIFlag;}//可选的属性

	/*	0: 不使能音量调节的本地UI显示功能
		1: 使能音量调节的本地UI显示功能（默认值）
	*/
	void setAudioVolumeUIFlag (int audioVolumeUI_flag);

	int getAudioVolumeUIFlag(){return sn_audioVolumeUIFlag;}//可选的属性

	/*	0: 不使能音轨选择的本地UI显示功能
		1: 使能音轨选择的本地UI显示功能（默认值）
	*/
	void setAudioTrackUIFlag (int audioTrackUI_flag);

	int getAudioTrackUIFlag(){return sn_audioTrackUIFlag;}//可选的属性

	/*	0: 不使能进度条的本地UI显示功能
		1: 使能进度条的本地UI显示功能（默认值）
	*/
	void setProgressBarUIFlag (int progressBarUI_flag);

	int getProgressBarUIFlag(){return sn_progressBarUIFlag;}//可选的属性

	/*	0: 不使能频道号的本地UI显示功能
		1: 使能频道号的本地UI显示功能（默认值）
	*/
	void setChannelNoUIFlag (int channelNoUI_flag);

	int getChannelNoUIFlag(){return sn_channelNoUIFlag;}//可选的属性

	/*	0:	不显示字幕 (默认值)
		1:	显示字幕
	*/
	void setSubtitileFlag (int subtitile_flag);

	int getSubtitileFlag(){return sn_subtitileFlag;}//可选的属性

	/*	0－100之间的整数值，0表示不透明，100表示完全透明。
		(默认值为0)
	*/
	void setVideoAlpha (int vAlpha);

	int getVideoAlpha(){return sn_videoAlpha;}//可选的属性

	/*	0: 允许TrickMode操做
		1: 不允许TrickMode操作 (默认值)
	*/
	void setAllowTrickmodeFlag (int allowTrickmode_flag);

	int getAllowTrickmodeFlag(){return sn_allowTrickmodeFlag;}

	/*    0设置为循环播放节目(默认值)
		1不循环,单次播放
	*/
	void setCycleFlag (int cycleFlag);

	int getCycleFlag(){return sn_cycleFlag;}

	/*    0设置为随机播放节目(默认值)
		1顺序播放
	*/
	void setRandomFlag (int randomFlag);

	int getRandomFlag(){return sn_randomFlag;}

	/*	留作厂商的扩展属性设置和获取，格式由厂商自己确定*/
	void setVendorSpecificAttr (char * vendorSpecificAttr){return;}

	char * getVendorSpecificAttr(){return NULL;}

	/*设置当前播放器为画中画模式
	mode:1画中画；其他:标准模式*/
	void setPIPVideoMode(int pipmode);;

	/*获取当前播放器的画中画设置*/
	int getPIPVideoMode(){return sn_pipmode;}
	/*	获取当前播放的媒体的标识， 
	MediaCode是媒体的唯一标识（节目，频道等），
	使用系统定义的ContentID。当播放器在播放列表模式时，
	MediaCode是动态改变的。
	*/
	string getMediaCode()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getMediaCode();
	}

	/*获取当前播放的媒体的总时长,整数，以秒为单位*/
	int getMediaDuration()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getMediaDuration();
	}

	/*	获取媒体播放到的当前时间点
	对VoD为从该媒体起始点开始计算的相对时间，以秒为单位；
	对TVoD为当前播放点的绝对时间；
	对Channel而言无意义
	参见RFC2326中的Normal Play Time (NPT)和Absolute Time (Clock Time)两种时间类型的格式
	*/
	unsigned long getCurrentPlayTime()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getCurrentPlayTime();
	}

	/*播放器的当前播放模式
	其中至少包括“播放模式”和“模式相关参数” 两类信息，
	播放模式分：Normal Play，Pause，Trickmode；
	当模式为Trickmode时必须带2x/-2x, 4x/-4x, 8x/-8x, 16x/-16x, 32x/-32x参数来表示快进/快退的速度参数
	*/
	char * getPlaybackMode()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getPlaybackMode();
	}

	/*	用于获取上一次发生的错误代码*/
	int GetLastError()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->GetLastError();
	}//可选

	/*返回机顶盒当前播放的频道号，
	不能获得有效的频道号时，返回-1。
	*/
	int getChannelNum()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getChannelNum();
	}

	/*	获取当前的声道类型
	字符串Left、Right、Stereo、JointStereo
	*/
	char * getCurrentAudioChannel()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getCurrentAudioChannel();
	}

	/*获取当前的的音轨类型
	字符串Chinese、English等
	*/
	char * getAudioTrack()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getAudioTrack();
	}

	/*获取当前选择的字幕类型
	字符串Chinese、English等
	*/
	char * getSubtitle()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getSubtitle();
	}

	/*获取播放列表中的媒体数量*/
	int getMediaCount(){return sn_playlist_media_num;}//可选的

	/*获取当前播的媒体，在播放列表中的索引值
	0：表示媒体列表的顶端
	*/
	int getCurrentIndex(){return 0;}//可选的

	/*获取当前播放的媒体的EntryID*/
	string getEntryID();//可选的

	/*字符串，用JSON格式的字符串，封装播放列表数据。*/
	string getPlaylist();//可选的

	
	static int registerChannel(string channel_info);

	static int setevent(MediaPlayErrorType ineventtype);

	static int registerChannelInfo(string name, string value);


protected:
		//注册频道
	static vector<string> sn_channel_list;
	static vector<int> userchannelid_list;
	static RealizeSNmediaplayer *sn_real_mediaplayer[MAX_PLAYER_NUM];
	unsigned char sn_instanceid;//0或1
	int sn_volume;//0-100，表示音量，0为静音，100为最大音量
	int sn_playlist_media_num;//记录播放列表中媒体数量
	int sn_playmode;//	0: 单媒体的播放模式 (默认值);1: 播放列表的播放模式
	/*0: 按setVideoDisplayArea()中设定的Height, Width, Left, Top属性所指定的位置和大小来显示视频
      	   1: 全屏显示，按全屏高度和宽度显示(默认值)
      	   2: 按宽度显示，指在不改变原有图像纵横比的情况下按全屏宽度显示
      	   3: 按高度显示，指在不改变原有图像纵横比的情况下按全屏高度显示
      	   255: 视频显示窗口将被关闭。它将在保持媒体流连接的前提下，隐藏视频窗口。如果流媒体播放没有被暂停，将继续播放音频。
      	*/
	int sn_videoDisplayMode;
	PlayerRect sn_videoDisplayrect;
	int sn_muteflag;//0:  设置为有声 (默认值);1:  设置为静音
	int sn_nativeUIFlag;//0: 不使能Player的本地UI显示功能;1: 使能Player的本地UI显示功能 (默认值)
	int sn_muteUIFlag;//: 不使能静音提示的本地UI显示功能;1: 使能静音提示的本地UI显示功能（默认值）
	int sn_audioVolumeUIFlag;//0: 不使能音量调节的本地UI显示功能;1: 使能音量调节的本地UI显示功能（默认值）
	int sn_audioTrackUIFlag;//0: 不使能音轨选择的本地UI显示功能;1: 使能音轨选择的本地UI显示功能（默认值）
	int sn_progressBarUIFlag;//0: 不使能进度条的本地UI显示功能;1: 使能进度条的本地UI显示功能（默认值）
	int sn_channelNoUIFlag;//0: 不使能频道号的本地UI显示功能;1: 使能频道号的本地UI显示功能（默认值）
	int sn_subtitileFlag;//0:  不显示字幕 (默认值);	1:  显示字幕
	int sn_videoAlpha;//0－100之间的整数值，0表示不透明，100表示完全透明(默认值为0)
	int sn_allowTrickmodeFlag;//0: 允许TrickMode操做;1: 不允许TrickMode操作 (默认值)
	int sn_playlistFlag;
	int sn_cycleFlag;
	int sn_randomFlag;
	int sn_autoDelFlag;
	int sn_pipmode;// 1画中画；其他:标准模式
	string sn_entryID;// 播放列表下有意义
	//string mediacode;//媒体的唯一标识
	int sn_speed;//-32,-16,-8,-4,-2,0,...32
	string sn_json_str;
	void *sn_mpobject;
	
	MediaplayerStateChangeCallback sn_playstate_func;
};

#endif
