#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include "DSP_graphics_api.h"

struct savegraphics
{
    DSPRect in_rect;
    char *graphics;
};

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define DEFAULTFONT "/tango/sunglobe.ttf"

#define BUTTON_X (SCREEN_WIDTH*0.75-10)
#define BUTTON_Y (90)
#define BUTTON_WIDTH_UI 250
#define BUTTON_HEIGHT_UI 55
#define UIPICTURE_PATH "/tango/UI-pictures/"
#define UIMUSICBACKGROUND UIPICTURE_PATH"musicbg.png"


#define BLUE 0x0000FFFF
#define YELLOW 0xFFFF00FF
#define WHITE 0xFFFFFFFF
#define GRAY 0x999999FF
#define BLACK 0x000000FF

#define UISHOW_LAST_SECOND 5

#define TVCHANNEL_X 120
#define TVCHANNEL_Y 90
#define TVCHANNEL_FONT_SIZE 45

#define SHOWTIME_X 120
#define SHOWTIME_Y 90
#define SHOWTIME_FONT_SIZE 45
#define SHOWTIME_WIDTH 500
#define SHOWTIME_HEIGHT 45
#define SINGLE_WIDTH 100


#define TVODLOG_X 70
#define TVODLOG_Y 50

#define TVODLOG_WIDTH 40
#define TVODLOG_HEIGHT 40

typedef enum
{
	STATE_CLOSE = 548,
	STATE_PLAY,
	STATE_STOP,
	STATE_PAUSE,
	STATE_STEP,
	STATE_FAST_FORWARD_2,
	STATE_FAST_FORWARD_4,
	STATE_FAST_FORWARD_8,
	STATE_FAST_FORWARD_16,
	STATE_FAST_FORWARD_32,
	STATE_FAST_REWIND_2,
	STATE_FAST_REWIND_4,
	STATE_FAST_REWIND_8,
	STATE_FAST_REWIND_16,
	STATE_FAST_REWIND_32,
	STATE_SLOW_FORWARD,
	STATE_STEP_BACK,
	STATE_BACKPLAY,//后退之后，按下播放键时，设置为此状态
	STATE_BACK_STOP,//后退之后，再次播放之前进入case001/005时，停止播放器时，设置为此状态
	STATE_AD,//正在播放广告
	STATE_SEARCH,//播放时间跳转，输入时间后的状态
	STATE_NONE,
	STATE_MUTE,//静音
	STATE_WAIT,//播放前缓冲
	STATE_REPEAT,//循环播放
	STATE_NOT_REPEAT,//取消循环播放
	STATE_AUDIOMODE_LEFT,  //左声道
	STATE_AUDIOMODE_RIGHT,  //右声道
	STATE_AUDIOMODE_STEREO,  //立体声
	STATE_SUB_DELAY,
	STATE_JUMP,
	STATE_VIDEORATIO,
	STATE_AUDIOINFO,  //音频信息
	STATE_TV_CHANNEL,
	STATE_NO_SIGNAL,
	SHOW_NOTICE_INFO,
	SHOW_PROG_TIME, //显示节目时间
	SHOW_TVOD_LOG

} typeShowMediainfo;




#ifdef __cplusplus
extern "C"{
#endif

void UI_JD_disappear(void *context);

int UI_JD_display(typeShowMediainfo cmd, int value, char *in_show_text);

void UI_TVODLOG_disappear();

void UI_JD_displaytime_disappear();

int UI_JD_displaytime(char *in_show_text);

void UI_JD_displayhour_disappear();

int UI_JD_displaytime_hour(int hour);

void UI_JD_displayminute_disappear();

int UI_JD_displaytime_minute(int minute);

void UI_JD_displaysecond_disappear();

int UI_JD_displaytime_second(int second);

#ifdef __cplusplus
}
#endif
#endif
