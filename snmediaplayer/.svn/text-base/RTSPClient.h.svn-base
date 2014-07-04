#ifndef _RTSPCLIENT_H
#define _RTSPCLIENT_H


#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/ioctl.h>

#include "voderrmsg.h"


#define RTSP_VER         "RTSP/1.0"
#define PLAY_MODE        "play"        //play from server
#define RECORD_MODE      "record"      //record from a mic
#define RECORD_FILE_MODE "record_file" //record from a file

#define NULL_CHAR   '\0'

static const int MAX_STRING_LEN  = 256;
static const int MAX_BUFFER_LEN  = 3000;
static const int DEFAULT_TX_PORT = 554;
static const int DEFAULT_RX_PORT = 10778;
static const int OK_RESPONSE     = 200;

typedef struct rtspsession_t
{
	int sock;              //socket for connection
	int media_rxPort;      //port for receiving media
	int video_rxPort; 
	int audio_rxPort; 

	int video_rxPort_z;
	int audio_rxPort_z;
	
	int media_txPort;      //server port to send media
	int rtsp_txPort;       //server port
	long long session;     //session number
	unsigned int cseq_tx;  //transmission seq num
	       
} rtspsession_t;

//the data encapsulation mode
typedef enum{
	TS = 2005,
	ISMA = 2006
}Transfer_Type;

typedef struct mp4parameter_
{
	bool is_aac;
	bool is_264;
	double duration;
	unsigned char configvideo[256];
	unsigned char configaudio[256];
	unsigned long videodsisize;
	unsigned long audiodsisize;
	int max_hour,max_minute,max_second;
	unsigned long videotimescale;
	unsigned long audiotimescale;
	int videotrackid;
 	int audiotrackid;
	char *host_addr;
	unsigned int video_ssrc;
	unsigned int audio_ssrc;
	unsigned short audio_rx_port;
	unsigned short audio_tx_port;
	unsigned short video_rx_port;
	unsigned short video_tx_port;

} mp4parameter_t;

typedef struct mp2parameter_
{
	char *host_addr;
	unsigned int my_ssrc;
	unsigned short rx_port;
	unsigned short tx_port;
	
	
} mp2parameter_t;


class RTSPClient
{  

 public:
 		
   	RTSPClient();            // constuctor
   	
  	~RTSPClient();           // destuctor audioClient.h
  	void cleanup();

    //starts the client while mode is referred as ts(mpeg2) or mp4(mpeg-4)
	int run(const unsigned char *command, int rtpReceivePort, Transfer_Type mode, void *parameter,bool needdescribe);
	
	int parse_sdp(mp4parameter_t * mp4parameter);
	
	int parse_m2t(char *rx_buffer);
	
	int rtspVcrRequest(int speed);

	int rtspVcrRequest(int speed, double FastStepTime);
	
	int rtspOPTIONS();

	int rtspTEARDOWN();

	int rtspPAUSEPLAY();

	int rtspresumePlay();	

	int rtspPAUSE();
	
	int rtspnowPAUSE();

	int rtspStep(double FastStepTime);
	
	int rtspPLAY(double PlayTime);
	
	unsigned long rtspGetParameter();
	
	int rtspPLAY_seq(double PlayTime,unsigned short seqnum);
	
	unsigned long get_audiossrc(){ return audio_ssrc; }
	
	unsigned long get_videossrc(){ return video_ssrc; }
	
	int get_audiotrackid(){ return audiotrackid; }
	
	int get_videotrackid(){ return videotrackid; }
	
	double get_duration(){ return duration; }

	char *get_serverip(){return hostname;}

	unsigned long get_firstaudiotimestmp(){return FirstAudioTimeStamp; }

	unsigned long get_firstvideotimestmp(){return FirstVideoTimeStamp; }

 private:   

     //initialize vars and buffers
     void init(int rtpReceivePort, char *mode);


      //connect with rtsp server
      bool establishConnection();

      //parse network address from command-line str
      bool parseRtspAddress(const unsigned char *command);

      //sed to rtsp server
      bool sendData();

      //receive response from server
      bool getResponse();
    
      //rtsp calls
      int rtspDESCRIBE(); 
	
      int rtspAudioSETUP(char *mode,int*audio_port);

	int rtspVideoSETUP(char *mode,int*audio_port);

       int rtspRECORD();

	void set_rtsperrno(enum VOD_ERRNO inerrno){rtsp_errno= inerrno;}
       

	int t_mode;// tranfer mode ISMA or TS
	int rtsp_errno;
       rtspsession_t  thisSession;
	int bytes_received;	
       unsigned long audio_ssrc;
       unsigned long video_ssrc;
	unsigned long media_ssrc;
	int videotrackid;
 	int audiotrackid;
       char filename[MAX_STRING_LEN];       //name of media file
       char hostname[MAX_STRING_LEN];       //rtsp server name
       char tx_buffer[MAX_BUFFER_LEN];      //buffer to transmit data
       char rx_buffer[MAX_BUFFER_LEN];      //buffer to receive data

//media info in rtsp
	double nptstart;
	double nptend;
	unsigned long FirstVideoTimeStamp;
	unsigned long  FirstAudioTimeStamp;
	
	double duration;

	int speed;//vcr speed

	char hostip[MAX_STRING_LEN];//remote server ip
	char firstdir[MAX_STRING_LEN];//dir in url
	bool dir_true;//if there is a dir segment in url
	bool alive;
	
};


#endif



