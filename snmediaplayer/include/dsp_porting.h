#ifndef DSP_PORTING_H
#define DSP_PORTING_H

#ifdef __cplusplus
extern "C" {
#endif


#define DSP_PORTING_ERROR -1
#define DSP_PORTING_OK 0


#define MAX_TEXT_LENGTH         64
#define MAX_NUM_PID            	8
#define MAX_NUM_DSMCC           8
#define MAX_NUM_APP             8

#define OSD_WIDTH 		720
#define OSD_HEIGHT 		576
#define OSD_X_START 		30
#define OSD_Y_START 		50
#define BPP 			4

#if 1
#define DSP_DEBUG(args...) \
		do{ \
			printf("[DEBUG] %s:%s(). ", __FILE__, __FUNCTION__); printf("" args); \
			printf("\n");} \
		while(0)

#define DSP_WARN(args...) \
		do{ \
			printf("[WARN] %s:%s(). ", __FILE__, __FUNCTION__); printf("" args); \
			printf("\n");} \
		while(0)

#define DSP_ERROR(args...) \
		do{ \
			printf("[ERROR] %s:%s(). ", __FILE__, __FUNCTION__); printf("" args); \
			printf("\n");} \
		while(0)

#define DSP_ECHO() \
			printf("Enter %s().\n", __FUNCTION__);  
#endif

#define UNIVERSAL

#ifdef UNIVERSAL
#if 0
//#ifndef _WIN32
#define DSP_DEBUG(args...) 

#define DSP_WARN(args...) 

#define DSP_ERROR(args...) 
#endif

#define DSP_ECHO() 


typedef int		int32;
typedef unsigned int	uint32;
typedef short		int16;
typedef unsigned short	uint16;
typedef char		int8;
typedef unsigned char 	uint8;
#endif

typedef struct
{
	int32 x0;
	int32 y0;
	uint32 width;
	uint32 height;
} ScreenRect;

enum thread_priority
{
	THREAD_PRIORITY_MIN = 1,
	THREAD_PRIORITY_LOW = 25,
	THREAD_PRIORITY_MEDIUM = 50,
	THREAD_PRIORITY_HIGH = 75,
	THREAD_PRIORITY_MAX = 99
};

typedef struct KeyData
{
	uint32 VirCode;
	int32 key;
}KeyData;

typedef struct
{
	int32 fd;
	int8 d_path[256];
} DSPDIR;

typedef struct
{
	int32 isdir;
	int8 d_name[256];
} DSPDIRENT;


#define IP_VERSION_4 ((uint8)4)
#define IP_VERSION_6 ((uint8)6)

typedef unsigned long IPV4;
typedef struct
{
	uint16 ip[8];
}IPV6;

typedef union
{
	IPV4 ipv4;
	IPV6 ipv6;
} UniIP;

typedef struct
{
	uint8 version;
	uint8 padding1;
	uint8 padding2;
	uint8 padding3;
	UniIP addr;
#define dsp_addr4 addr.ipv4
#define dsp_addr6 addr.ipv6.ip
} IPADDR, *IPAddr;

enum ADDRESS_FAMILY
{
	ADDRESS_FAMILY_INET_V4,
	ADDRESS_FAMILY_INET_V6
};

#ifndef _WIN32
/* be same as struct hostent */
typedef struct
{
	int8 *h_name;        	/* official name of host */
	int8 **h_aliases;    	/* alias list */
	int32 h_addrtype;    	/* host address type */
	int32 h_length;      	/* length of address */
	int8 **h_addr_list;  	/* list of addresses */
} HOSTENT;
#endif
/* be same as struct iovec */
typedef struct 
{
	void *iov_base;    	/* Starting address */
	int32 iov_len;    	/* Length in bytes  */
} IOVEC;


/* be same as struct msghdr */
typedef struct
{
	void *msg_name;     	/* optional address */
	int32 msg_namelen;    	/* size of address */
	IOVEC *msg_iov;      	/* scatter/gather array */
	int32 msg_iovlen;     	/* elements in msg_iov */
	void  *msg_control;  	/* ancillary data, see below */
	int32 msg_controllen; 	/* ancillary data buffer len */
	int32 msg_flags;      	/* flags on received message */
} MSGHDR;

typedef struct DSP_FD_SET DSP_FD_SET;
struct DSP_FD_SET
{
	int32 fd;
	DSP_FD_SET *next;
};

#ifndef _WIN32
/* be same as struct timeval */
typedef struct 
{
	int32 tv_sec;		/* seconds */
	int32 tv_usec;		/* microseconds */
} TIMEVAL ;
#endif

/* be same as struct addrinfo */
typedef struct ADDRINFO ADDRINFO;
struct ADDRINFO 
{
	int32 ai_flags;
	int32 ai_family;
	int32 ai_socktype;
	int32 ai_protocol;
	int32 ai_addrlen;
	IPADDR *ai_addr;
	int8 *ai_canonname;
	ADDRINFO *ai_next;
};


enum DSP_VOD_STATUS
{
	VOD_STATUS_OK,
	VOD_STATUS_FAIL
};



#define DSP_QUEUE_SEM_WAIT_FOREVER			0

typedef struct 
{
	uint32 q1stWordOfMsg;                /* First word of a queue message.  */
	uint32 q2ndWordOfMsg;                /* Second word of a queue message. */
	uint32 q3rdWordOfMsg;                /* Third word of a queue message.  */
	uint32 q4thWordOfMsg;                /* Fourth word of a queue message. */
} DSP_QUEUE_MESSAGE;



enum
{
	DSP_OS_QUEUE_SEM_STATUS_NONE,
	DSP_OS_QUEUE_SEM_STATUS_AVAILABLE,  /* queue_wait/semaphore_wait is available(value>0, not locked, etc)*/
	DSP_OS_QUEUE_SEM_STATUS_UNAVAILABLE,  /* queue_wait/semaphore_wait is unavailable(value<=0, locked, etc)*/
	DSP_OS_QUEUE_SEM_STATUS_UNKNOWN
};


typedef void (*TASK_PROC_FUNC)(void *);

enum
{
	DSP_QUEUE_SEM_WAIT,
	DSP_QUEUE_SEM_NOWAIT
};

enum
{
	DSP_TASK_WAIT_FIFO,	
	DSP_TASK_WAIT_PRIO
};


enum
{
	DSP_DSM_TYPE_NULL, 
	DSP_DSM_TYPE_PES,
	DSP_DSM_TYPE_TS,
	DSP_DSM_TYPE_UNKNOWN
};


/**
 * get rand number no more than maxNUM
 * @return unsigned int32
 */
uint32 DSP_porting_random(uint32 maxNum);

/**
 * Get the time since 1970-1.1-00:00:00
 * @return seconds,milliseconds
 */
void DSP_porting_GetTotalTime(int32 *seconds, int32 *milliseconds);

uint32 DSP_porting_localfile_GetLength(int32 fp);

/**
 * function about mutex
 * return result TRUE or FAlSE
 * @return BOOL
 */
void DSP_porting_LockMutex(uint32 mutex);
void DSP_porting_UnlockMutex(uint32 mutex);
uint32 DSP_porting_CreateMutex(uint8 initCount, uint8 maxCount);
void DSP_porting_DestroyMutex(uint32 mutex);


/*Font*/
/**
 * init font context to draw text
 * @return NULL
 */
void DSP_porting_InitFontContext();


/**
 * destroy font context
 * @return NULL
 */
void DSP_porting_DestroyFontContext();


/**
 * set text height 
 * @param size : the text height to display
 * @return NULL
 */
void DSP_porting_SetFontHeight(uint16 size);



/**
 * get size of a text character
 * @param code : text code 
 * @return FLOAT
 */
float DSP_porting_GetFontSize(uint32 code);

/**
 * draw a text character in buffer(width,height) with x,y,color
 * @param color(rgba)
 * @return NULL
 */
void DSP_porting_DrawText(uint32 code, uint8 *buf, uint32 width, uint32 height, uint32 x, uint32 y, uint32 color);

void DSP_porting_GetCharInfo(uint32 ch, uint8 **mem, int32 *top, int32* left, int32* width, int32 *height, int32 *pitch );



/******************** for dsp_base.c **********************/
void *DSP_porting_malloc(int32 memsize);
void *DSP_porting_realloc(void *ptr, int32 size);
void DSP_porting_free(void* ptr);
int32 DSP_porting_dprintf(const int8 *fmt, ...);
uint32 DSP_porting_time_ms(void);
void *DSP_memcpy(void *s1, const void *s2, int32 n);
void DSP_porting_time_sleep(uint32 millseconds);


/******************** for dsp_os.c ************************/
void DSP_porting_initOS(void);
uint32 DSP_porting_getTaskId(void);
void DSP_porting_lock(void);
void DSP_porting_unlock(void);

uint32 DSP_porting_task_create(const int8 *name, TASK_PROC_FUNC func, void *param, int32 priority, uint32 stack_size, int32 detachment);
int32 DSP_porting_task_destroy(uint32 task_handle);
void DSP_porting_task_sleep(int32 ms);
uint32 DSP_porting_sem_create(const int8 *name, int32 initialTokenCount, uint32 taskWaitMode);
int32 DSP_porting_sem_destroy(uint32 sem_handle);
int32 DSP_porting_sem_wait(uint32 sem_handle, uint32 waitMode, int32 milliSecsToWait);
int32 DSP_porting_sem_release(uint32 sem_handle); 
int32 DSP_porting_queue_create(const int8 *name, uint32 maxQueueLength, uint32 taskWaitMode);
int32 DSP_porting_queue_destroy(uint32 queue_handle);
int32 DSP_porting_queue_send(uint32 queue_handle, void *msg, uint32 msg_size);
int32 DSP_porting_queue_recv(uint32 queue_handle, void *msg, uint32 msg_size, uint32 waitMode, int32 millSecsToWait);


/******************** for dsp_demux.c *********************/
int32 DSP_porting_demux_init(void);
int32 DSP_porting_demux_exit(void);
int32 DSP_porting_filter_open(uint16 PID, uint32 bufsize, uint32 wide, uint8 coef[], uint8 mask[], uint8 excl[]);
int32 DSP_porting_filter_close(uint32 slot);
int32 DSP_porting_filter_recv(uint32 slot, uint8 *buffer, int32 max_len);
int32 DSP_porting_filter_poll(uint32 slot, uint32 event[3]);

/******************** for dsp_file.c **********************/
//int32 DSP_porting_localfile_open(const int8 *filename, int32 mode);
//int32 DSP_porting_localfile_read(int32 fd, int8 *buffer, int32 nBytes);
//int32 DSP_porting_localfile_write(int32 fd, const int8 *buffer, int32 nBytes);
//int32 DSP_porting_localfile_close(int32 fd);

int32 DSP_porting_localfile_fopen(const int8 *path, const int8 *mode);
int32 DSP_porting_localfile_fclose(int32 fp);
uint32 DSP_porting_localfile_fread(void *buffer, int32 sz, int32 n, int32 fp);
uint32 DSP_porting_localfile_fwrite(const void *buffer, int32 sz, int32 n, int32 fp);
int32 DSP_porting_localfile_fseek(int32 fp, int32 offset, int32 origin);
int32 DSP_porting_localfile_fscanf(int32 fp, const int8 *format, ... );
int32 DSP_porting_localfile_feof(int32 fp);
int32 DSP_porting_localfile_fprintf(int32 fp, const int8 *format, ...);
DSPDIR *DSP_porting_opendir(const int8 *dirname);
int32 DSP_porting_readdir(DSPDIR *dir, int8 *outfile);
int32 DSP_porting_closedir(DSPDIR *dir);

/******************** for dsp_graphics.c ******************/
int32 DSP_porting_graphics_init(ScreenRect prect);
int32 DSP_porting_graphics_fini(void);
int32 DSP_porting_graphics_get_graphic_info(ScreenRect *rect,int32 *pitch, int8 **pbuffer);
int32 DSP_porting_draw_image(ScreenRect rect, int8 *srcbuffer);
#ifdef WIN32
void* DSP_porting_get_window_handle();
#endif



/******************** for dsp_dsm.c ***********************/
uint32 DSP_porting_descrambler_allocate(int32 encryptmode);
int32 DSP_porting_descrambler_free(uint32 deschandle);
int32 DSP_porting_descrambler_set_pid(uint32 deschandle, uint16 pid);
int32 DSP_porting_descrambler_set_oddkey(uint32 deschandle, uint8 *key, int32 keylen);
int32 DSP_porting_descrambler_set_evenkey(uint32 deschandle, uint8 *key, int32 keylen);
int32 DSP_porting_av_start(uint16 apid, uint16 vpid, uint16 pcr_pid,uint16 atype, uint16 vtype, uint16 ecm0, uint16 ecm1);
void DSP_porting_av_stop(void);
int32 DSP_porting_av_record_start(uint16 apid, uint16 vpid, uint16 pcr_pid, uint16 atype, uint16 vtype, uint16 ecm0, uint16 ecm1, int8 * name);
int32 DSP_porting_av_record_stop();

int32 DSP_decode_init();
int32 DSP_porting_video_setwindow(uint32 x, uint32 y, uint32 w, uint32 h);
int32 DSP_porting_audio_setvolume(uint32 volume, uint32 volume_max);
int32 DSP_porting_audio_setmute(uint32 mute);
/******************** for dsp_tuner.c ********************/
int32 DSP_porting_tune_getSignalQuality(void);
int32 DSP_porting_tune_getSignalStrength(void);
int32 DSP_porting_tune_getSignalBER(void);
int32 DSP_porting_tune_lock(uint32 device, void *buf, uint32 len, int32 tuneid, int32 flag);
int32 DSP_porting_tune_getSignalLocked(void);
int32 DSP_porting_tune_init(void);
int32 DSP_porting_tune_fini(void);

/******************** for dsp_nvram.c ********************/
int32 DSP_porting_nvram_info(uint8 **addr, int32 *numberofsections, int32 *sect_size, int32 flag);
int32 DSP_porting_nvram_read(uint32 flash_addr, uint8 *buff_addr, int32 nBytes);
int32 DSP_porting_nvram_burn(uint32 flash_addr, const char *buff_addr, int32 len);
int32 DSP_porting_nvram_status(uint32 flash_addr, int32 nBytes);


/******************** for dsp_socket.c ******************/
int32 DSP_porting_socket_open(int32 domain, int32 type, int32 protocol);
int32 DSP_porting_socket_connect(int32 sockfd, const IPADDR *serv_addr, int32 serv_port);
int32 DSP_porting_socket_send(int32 sockfd, const void *buffer, int32 len, int32 flags);
int32 DSP_porting_socket_recv(int32 sockfd, void *buffer, int32 len, int32 flags);
int32 DSP_porting_socket_close(int32 sockfd);
int32 DSP_porting_socket_bind(int32 sockfd, const IPADDR *serv_addr, int32 serv_port);
int32 DSP_porting_socket_listen(int32 sockfd, int32 backlog);
int32 DSP_porting_socket_accept(int32 sockfd, IPADDR *client_addr, int32 *client_port, int32 *addrlen);
#if 0
/************************* socket api for VOD specific *************************/
int32 DSP_porting_socket_inet_aton(const int8 *cp, UniIP *inp);
int8 *DSP_porting_socket_inet_ntoa(UniIP in);
int32 DSP_porting_socket_inet_pton(enum ADDRESS_FAMILY af, const int8 *src, IPADDR *dst);
const int8 *DSP_porting_socket_inet_ntop(enum ADDRESS_FAMILY af, const IPADDR *src, int8 *dst, int32 cnt);
HOSTENT *DSP_porting_socket_gethostbyname(const int8 *name);
int32 DSP_porting_socket_gethostname(int8 *name, int32 len);
int32 DSP_porting_socket_sendto(int32 sockfd, const void *msg, int32 len, int32 flags, const IPADDR *to, int32 tolen);
int32 DSP_porting_socket_sendmsg(int32 sockfd, const MSGHDR *msg, int32 flags);
int32 DSP_porting_socket_recvfrom(int32 sockfd, void *buf, int32 len, int32 flags, IPADDR *from, int32 *fromlen);
int32 DSP_porting_socket_getsockname(int32 sockfd, IPADDR *name, int32 *namelen);
int32 DSP_porting_socket_readv(int32 sockfd, const IOVEC *vector, int32 count);
int32 DSP_porting_socket_writev(int32 sockfd, const IOVEC *vector, int32 count);
int32 DSP_porting_socket_getsockopt(int32 sockfd, int32 level, int32 optname, void *optval, int32 *optlen);
int32 DSP_porting_socket_setsockopt(int32 sockfd, int32 level, int32 optname, const void *optval, int32 optlen);
int32 DSP_porting_socket_setblock(int32 sockfd, int32 isblock);
void DSP_porting_socket_FD_CLR(int32 sockfd, DSP_FD_SET *set);
void DSP_porting_socket_FD_ISSET(int32 sockfd, DSP_FD_SET *set);
void DSP_porting_socket_FD_SET(int32 sockfd, DSP_FD_SET *set);
void DSP_porting_socket_FD_ZERO(DSP_FD_SET *set);
int32 DSP_porting_socket_select(int32 sockfd, DSP_FD_SET *readfds, DSP_FD_SET *writefds, DSP_FD_SET *exceptfds, TIMEVAL *timeout);
int32 DSP_porting_socket_getaddrinfo(const int8 *node, const int8 *service, const ADDRINFO *hints, ADDRINFO **res);

/************************* decoder api for VOD specific *************************/
int32 DSP_porting_playpump_InitDecoder(int32 argc, int8 *argv[]);
int32 DSP_porting_playpump_ReleaseDecoder();
int32 DSP_porting_playpump_DecoderGetBuffer(uint8 **pbuf, uint32 *bufsize);
int32 DSP_porting_playpump_DecoderSendData(uint32 count);
int32 DSP_porting_playpump_DecoderReleaseBuffer();
int32 DSP_porting_playpump_SetVolume(int32 value);
int32 DSP_porting_playpump_Mute();
int32 DSP_porting_playpump_check_prebuf_state(uint32 buffersize);
int32 DSP_porting_playpump_STCSetTime(enum DemuxTriggerType triggermode);
int32 DSP_porting_playpump_ResetDecoder(enum DCCStopMode mode);
int32 DSP_porting_playpump_Reset();
int32 DSP_porting_playpump_PlayDecoder(uint32 devices, enum DCCVideoPlayCommand mode);
int32 DSP_porting_playpump_StopDecoder(uint32 devices, enum DCCStopMode mode);
int32 DSP_porting_playpump_PauseDecoder(uint32 devices);
int32 DSP_porting_playpump_GetPlayerState(uint32 *state);
int32 DSP_porting_playpump_SetPlayerState(uint32 state);
int32 DSP_porting_playpump_GetDemuxState(int32 *state);
int32 DSP_porting_playpump_GetVolumeIndex(int32 *index);
#endif
int32 DSP_font_init();


#ifdef __cplusplus
}
#endif

#endif


