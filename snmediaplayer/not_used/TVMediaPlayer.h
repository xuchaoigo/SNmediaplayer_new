#ifndef _TVMEDIAPLAYER
#define _TVMEDIAPLAYER

#include "SNmediaplayer.h"
#include "RealizeSNmediaplayer.h"

class TVMediaPlayer:class RealizeSNmediaplayer
{
	
public:
	/*构造函数*/
	TVMediaPlayer();

	/*析构函数*/
	~ TVMediaPlayer();

	play_state getPlayState();

	player_setvolume(int volumeindex);

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

	/*返回机顶盒当前播放的频道号，
	不能获得有效的频道号时，返回-1。
	*/
	 int getChannelNum() ;

};

#endif
