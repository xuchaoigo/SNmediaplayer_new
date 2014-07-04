#ifndef __RTSPCLIENT_H_
#define __RTSPCLIENT_H_

#define RTSP_VER         "RTSP/1.0"
#define SETUP_SDV       0		//sm
#define SETUP_VOD       1		//sm
#define NO_SETUP       2          //ms: ֻ�����ӣ�����setup
#define SETUP_TVS     3		//tvs
#define SETUP_TVOD     4		//tvod


//ANNOUNCE�´���Event Code
#define EOS_REACHED 2101
#define BOS_REACHED 2102
#define TVS_REACHED_SDV 2103


static const int MAX_STRING_LEN_prv = 256;
static const int MAX_BUFFER_LEN_prv = 3000;

#define SETUP_RETRY_TIMES 1
#define DEFAULT_SM_TIMEOUT 60
#define DEFAULT_MS_TIMEOUT 300
#define RESPONSE_TIMEOUS_SEC 2
#define RESPONSE_TIMEOUS_USEC 0

#define KEEP_ALIVE_MISS_MAX 3


typedef enum
{
	PLAYER_NONE,
	PLAYER_URLCHANGE,
	PLAYER_VOLUMECHANGE,
	PLAYER_VOLUMEMUTE,
	PLAYER_PLAY,
	PLAYER_PAUSE,
	PLAYER_FASTFORWARD,
	PLAYER_FASTBACKWARD,
	PLAYER_SEEK,
	PLAYER_REALTIMETV,
	PLAYER_QUIT
}PlayerStatus;

typedef struct rtspsession
{
	int sock;              //socket for connection
	int media_rxPort;      //port for receiving media
	int video_rxPort; 
	int audio_rxPort; 

	int video_rxPort_z;
	int audio_rxPort_z;
	
	int media_txPort;      //server port to send media
	int rtsp_txPort;       //server port
	unsigned long cseq_tx;  //transmission seq num
	       
} rtspsession_prv_t;
typedef struct ServerInfor_s
{
	char ip[MAX_STRING_LEN_prv];
	unsigned short port;
	

}ServerInfor_prv_t;
typedef struct SessionInfor_s
{
	char sessionID[32];
	char GlobalSessionID[32];
	
}SessionInfor_prv_t;

typedef struct PlayParam_s
{
	unsigned char ready;
	unsigned frequency;     //MHz
	unsigned char annex;
	unsigned short mode;   // 1,2,3,4,5  -> 16,32,64,128,256
	unsigned symbolRate;  //MSymbol/s
	unsigned pnID;
	

}PlayParam_t;

typedef struct KeepAliveArgs_s
{
	class RTSPClient_prv * pRTSPClient;
	unsigned int timeout;	
}KeepAliveArgs_t;
//��keep alive�̴߳���������RTSPClient����Ϣ

class RTSPClient_prv
{
public:
 		
   	RTSPClient_prv();             	
  	~RTSPClient_prv();         
	void init();
//	int rtspOPTIONS();
//	int rtspDESCRIBE();
	int rtspSETUP(int setup_type);
	int rtspPLAY(long PlayTime);
	int rtspPAUSE();
	int rtspTEARDOWN();
	int rtspGETPARAMETER(); 			  //sm �����շ�
	int rtspGETPARAMETER_notime() ;//1���շ�����2������gettime���ص�ʱ��3������ ANNOUNCE�� 
	int rtspGETPARAMETER_gettime();//����GETPARAMETER_gettime���ȴ�rtspGETPARAMETER_notime����ʱ��
	int ss_keep_alive_begin(void * sm_ka_args);
	int ms_keep_alive_begin(void * ms_ka_args);
	int ss_keep_alive_end();
	int ms_keep_alive_end();
	int rtsp_forward(float scale);
	int rtsp_backward(float scale);
	int rtsp_jump(long PlayTime);
	bool establishConnection();
	void destroyConnection(void);
	bool reConnection(void);	
	void getPlayParam(PlayParam_t *pPlayParam);
/*xuc */
	ServerInfor_prv_t server_infor;
	SessionInfor_prv_t session_infor;	
	unsigned int session_timeout;	             //��,����session�ĳ�ʱʱ��
	unsigned int ms_session_timeout;		 //��,�����ms session�ĳ�ʱʱ��
	char contentID[MAX_STRING_LEN_prv];   //��Entitlement code ���contentID
	char userID[MAX_STRING_LEN_prv];      //��xml������x-userID 
	unsigned int ServiceGroup;		// ��NIT��ȡ��,1-65535
	char license[MAX_STRING_LEN_prv]; //��json�´���vod license
	unsigned long bit_rate;                           //sm�巨��ȡ��Ŀ����ʱ�����Ż���ȡ

	char ms_ip[MAX_STRING_LEN_prv];		//saved media server ip
	unsigned short ms_port;			//saved media server port
	char ms_sessionID[32];				//setup ���ص�����ID
	char ms_GlobalSessionID[32];				//setup ���ص�Globalsession ID
	
	unsigned long media_start_time;	//tvs��setupʱ���ݵĿ�ʼʱ��
	unsigned long media_stop_time;	//tvs��setupʱ���ݵĽ���ʱ��
	//����������������
	//vod: ����ʹ��
	//tvs: ʹ��media_start_time
	//tvod:��ʹ��
	
	unsigned long playing_time ;                   //��ǰ����ʱ��

	PlayParam_t play_param;
	int player_status;
	unsigned short if_keep_alive_error;
	//int msgid; 					//����streamMediaPlayer ״̬��msgqueue ID

	unsigned short ss_keep_alive_miss_cnt; 
	unsigned short ms_keep_alive_miss_cnt; 
	pthread_t ss_keep_alive_id;
	pthread_t ms_keep_alive_id;   //xuc 2012.10.29
	
private:
	
	int send_command();
	int get_response();
	int get_response_timeout(int sec, int usec);
	
	rtspsession_prv_t  thisSession;
	int bytes_received;	
	char hostname[MAX_STRING_LEN_prv];       //rtsp server name
       char tx_buffer[MAX_BUFFER_LEN_prv];      //buffer to transmit data
       char rx_buffer[MAX_BUFFER_LEN_prv];      //buffer to receive data

//media info in rtsp
	double nptstart;
	double nptend;
	unsigned long FirstVideoTimeStamp;
	unsigned long  FirstAudioTimeStamp;
	
	double duration;

	int speed;//vcr speed
	char port[8];
	char hostip[MAX_STRING_LEN_prv];//remote server ip
	char firstdir[MAX_STRING_LEN_prv];//dir in url
	bool dir_true;//if there is a dir segment in url
	bool alive;



};

void * ss_keep_alive_thread(void * sm_ka_args);  //with session server
void * ms_keep_alive_thread(void * ms_ka_args); //with media server

#endif
