#ifndef _EZBROWSER_H_
#define _EZBROWSER_H_

#undef EZBROWSER_EXPORT
#if defined(__GNUC__)
    #define EZBROWSER_EXPORT __attribute__((visibility("default")))
#elif defined(WIN32) || defined(_WIN32)
    #define EZBROWSER_EXPORT __declspec(dllexport)
#else
    #define EZBROWSER_EXPORT
#endif


#if defined(__cplusplus)
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

typedef struct TEzbrowser_
{
    void* manager;
}TEzbrowser;

typedef enum MOUSE_EVENT_TYPE
{
    MOUSE_UP_EVENT = 0,
    MOUSE_DOWN_EVENT,
    MOUSE_MOVE_EVENT
}MOUSE_EVENT_TYPE;

typedef enum KEY_EVENT_TYPE
{
    KEY_UP_EVENT = 0,
    KEY_DOWN_EVENT
}KEY_EVENT_TYPE;

enum
{
    EZBROWSER_ERROR = -1,
    EZBROWSER_OK = 0,
    EZBROWSER_EXIT = 1
};

typedef enum
{
    EZBROWSER_FOREGROUND,
    EZBROWSER_BACKGROUND
}RUN_STATE;

typedef enum
{
    EZBROWSER_STATUS_STOPPING =1<<0,
    EZBROWSER_STATUS_RUNNING =1<<1,
}RUN_STATUS;


typedef enum
{
    EZBROWSER_VMODE_720X576,
    EZBROWSER_VMODE_1280X720,
    EZBROWSER_VMODE_1920X1080
}VIEW_MODE_TYPE;


typedef struct
{
    int x0;
    int y0;
    unsigned int width;
    unsigned int height;
}IFrameRect;


 typedef int (*ErrorCallBack)(int, const char*);
 typedef void (*PageLoadingCallBack)(void);
 typedef void (*PageFinishLoadCallBack)(void);

/*浏览器按指定分辨率初始化*/
EZBROWSER_EXPORT TEzbrowser* ezBrowser_Init(VIEW_MODE_TYPE vtpye);

/*释放浏览器占用资源*/
EZBROWSER_EXPORT int ezBrowser_Fini(TEzbrowser* browser);

/*浏览器主循环*/
EZBROWSER_EXPORT int ezBrowser_MainLoop(TEzbrowser* browser);

/*设置浏览器访问的主页面*/
EZBROWSER_EXPORT int ezBrowser_SetHomePage(TEzbrowser* browser,const char* home_url);

/*浏览器在当前窗口访问指定URL的网页*/
EZBROWSER_EXPORT int ezBrowser_LoadURL(TEzbrowser* browser,const char* in_url);

/*浏览器在根窗口访问指定URL的网页，如有子窗口，全部关闭*/
EZBROWSER_EXPORT int ezBrowser_LoadURLInRootWindow(TEzbrowser* browser,const char* in_url);

/*浏览器在新开窗口访问指定URL的网页*/
EZBROWSER_EXPORT int ezBrowser_LoadURLByNewWindow(TEzbrowser* browser,const char* in_url);

/*浏览器关闭当前窗口*/
EZBROWSER_EXPORT int ezBrowser_CloseCurrentWindow(TEzbrowser* browser);

/*指定iframe网页重定向到新的URL*/
EZBROWSER_EXPORT int ezBrowser_LoadIFrameURLByName(TEzbrowser* browser, const char* frame_name, const char* in_url);

/*得到iframe网页的坐标位置*/
EZBROWSER_EXPORT int ezBrowser_GetIFrameRectByName(TEzbrowser* browser, const char* frame_name, IFrameRect *framerect);

/*得到当前浏览器访问页面的URL*/
EZBROWSER_EXPORT int ezBrowser_CurrentURL(TEzbrowser* browser, char* buf);

/*判断当前网页是否已经加载并且绘制到屏幕完毕*/
EZBROWSER_EXPORT int ezBrowser_IsURLLoadingFinished(TEzbrowser* browser,const char* in_url);

/*设置浏览器的运行方式（前台或后台运行）*/
EZBROWSER_EXPORT int ezBrowser_SetRunningState(TEzbrowser*browser, RUN_STATE state);

/*开始运行浏览器*/
EZBROWSER_EXPORT int ezBrowser_StartRunning(TEzbrowser*browser);

/*停止运行浏览器，不处理任何事件*/
EZBROWSER_EXPORT int ezBrowser_StopRunning(TEzbrowser*browser);

/*浏览器遥控器按键事件处理*/
EZBROWSER_EXPORT int ezBrowser_OnKeyEvent(TEzbrowser* browser, unsigned int key, KEY_EVENT_TYPE type);

/*浏览器鼠标按键事件处理*/
EZBROWSER_EXPORT int ezBrowser_OnMouseEvent(TEzbrowser* browser, int x, int y, MOUSE_EVENT_TYPE type);

/*退出浏览器*/
EZBROWSER_EXPORT int ezBrowser_OnExitEvent(TEzbrowser* browser);

/*浏览器回退到历史访问记录中的上一个页面*/
EZBROWSER_EXPORT int ezBrowser_OnGoBackward(TEzbrowser* browser);

/*浏览器前进到历史访问记录中的下一个页面*/
EZBROWSER_EXPORT int ezBrowser_OnGoForward(TEzbrowser* browser);

/*浏览器访问主页面*/
EZBROWSER_EXPORT int ezBrowser_OnGoHome(TEzbrowser* browser);

/*刷新当前页面*/
EZBROWSER_EXPORT int ezBrowser_OnRefresh(TEzbrowser* browser);

/*加载widget*/
EZBROWSER_EXPORT int ezBrowser_LoadWidget(TEzbrowser* browser, const char* name, int x, int y);

/*卸载widget*/
EZBROWSER_EXPORT int ezBrowser_UnloadWidget(TEzbrowser* browser, const char* name, int x, int y);

/*清除浏览器访问页面的历史记录*/
EZBROWSER_EXPORT int ezBrowser_ClearHistoryRecord(TEzbrowser* browser);

/*设置浏览器出错的回调接口*/
EZBROWSER_EXPORT int ezBrowser_SetErrorCallback(TEzbrowser *browser, ErrorCallBack errCb);

/*设置浏览器页面加载过程中的事件处理接口（如调用图
形接口给出加载进度提示）*/
EZBROWSER_EXPORT int ezBrowser_PageLoadingCallback(TEzbrowser *browser,PageLoadingCallBack LoadingCb);

/*设置浏览器页面加载完毕的事件处理接口（如调用图
形接口擦除加载进度提示）*/
EZBROWSER_EXPORT int ezBrowser_PagefinishLoadCallback(TEzbrowser *browser,PageLoadingCallBack finishLoadCb);

/*加载业务相关的JS动态库*/
EZBROWSER_EXPORT int ezBrowser_LoadService(TEzbrowser *browser, const char* servicepath);

/*卸载业务相关的JS动态库*/
EZBROWSER_EXPORT int ezBrowser_UnloadService(TEzbrowser *browser, const char* servicepath);

/*获取浏览器当前运行方式(前台或后台运行)*/
EZBROWSER_EXPORT int ezBrowser_getRunningState(TEzbrowser *browser);

/*获取浏览器当前版本号*/
EZBROWSER_EXPORT char *ezBrowser_Version();

/*获取浏览器当前状态(stopping/running)*/
EZBROWSER_EXPORT int ezBrowser_getRunningStatus(TEzbrowser *browser);

#if defined(__cplusplus)
}
#endif

#endif//_EZBROWSER_H_
