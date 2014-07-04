#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "RTSPClient.h"

extern "C"{
#include "rtsprtp/base64.h"
}


#if 1
RTSPClient::RTSPClient()
{    
}  
  
RTSPClient::~RTSPClient()
{ 

} 


void RTSPClient::cleanup()
{ 
	if(thisSession.sock > 0)
	{
		printf( "Closing Connection\n" );
		close(thisSession.sock);
		thisSession.sock = -1;
	}

} 

void RTSPClient::init(int rtpReceivePort, char *mode)
{

	bytes_received=0;

	thisSession.sock = -1;
	thisSession.cseq_tx = 0;


	
	memset(rx_buffer,0,MAX_BUFFER_LEN);
	memset(tx_buffer,0,MAX_BUFFER_LEN);
	memset(firstdir,0,MAX_STRING_LEN);

	thisSession.video_rxPort = rtpReceivePort;
	thisSession.audio_rxPort = rtpReceivePort+2;
	thisSession.video_rxPort_z=0;
	thisSession.audio_rxPort_z=0;


}



//#define writefile
int RTSPClient::run(const unsigned char *command, int rtpReceivePort, Transfer_Type mode, void *parameter,bool needdescribe)
{
	bool currentStatus = true;
	char * string_ptr1 = NULL;
	int vrtpstatus, artpstatus;
	static unsigned char remoteIP[16];
	bool is_ipv6;
	int remoteportbase;
	audiotrackid = 0;
	videotrackid = 0;

	mp4parameter_t *mp4parameter = NULL;
	mp2parameter_t *mp2parameter = NULL;

	t_mode = mode;
	memset(remoteIP,0,16);

	this->init(rtpReceivePort, "PLAY");
	currentStatus = parseRtspAddress(command); 
	//printf("parse\n");
	//if we were successful parsing the address, proceed 
	if(currentStatus == true)
	{
		if(t_mode == ISMA)
			mp4parameter = (mp4parameter_t *)parameter;
		else
			mp2parameter = (mp2parameter_t *)parameter;

		//printf("hostip: %s\nHOST: %s\nPORT: %d\nFILE: %s\n",hostip, hostname,thisSession.rtsp_txPort,filename);
		memcpy(hostname,hostip,MAX_STRING_LEN);
		string_ptr1= strchr(hostname, ':');
		if(string_ptr1 != NULL)
			is_ipv6=true;
		else
			is_ipv6=false;
		if(is_ipv6)
			printf("ipv6 vod support !!\n");
		else
			printf("ipv4 ffvod support !!\n");
		if(establishConnection())
		{
			alive=true;

			if(rtspOPTIONS() == -1)
			{
				close(thisSession.sock);
				return -1;
			}
						
			//mode PLAY_MODE
			if(needdescribe)
			{	
				if(rtspDESCRIBE()==-1)
					return -1;
				if(t_mode == ISMA)
				{
					if(parse_sdp(mp4parameter)==-1)
					{
						//zxy add 12.2
						set_rtsperrno(ERR_VOD_RTSP_SDP_PARSE);
						printf("parse adp err\n");
						return -1;
					}
				}
				else if(t_mode == TS)
				{
					
					if(parse_m2t(rx_buffer) == -1)
					{
						//zxy add 12.2
						set_rtsperrno(ERR_VOD_RTSP_SDP_PARSE);
						return -1;
					}
					thisSession.audio_rxPort = rtpReceivePort;
				}
			}

			//setup command with audio and video , TS need just one set up
			if(rtspVideoSETUP("PLAY",&(thisSession.video_rxPort_z))==-1)
			{
				//zxy add 12.2
				set_rtsperrno(ERR_VOD_RTSP_AUDIO_SETUP);
				return -1;
			}
			
			//if(t_mode == ISMA)
			if(audiotrackid)
			if(rtspAudioSETUP("PLAY",&(thisSession.audio_rxPort))==-1)
			{
				//zxy add 12.2
				set_rtsperrno(ERR_VOD_RTSP_VIDEO_SETUP);
				return -1;
			}
			

			//printf("video port is %d,audio port is %d\n",thisSession.video_rxPort_z,thisSession.audio_rxPort_z);
			remoteportbase=6972;
			if(thisSession.video_rxPort_z == 0)
				thisSession.video_rxPort_z = remoteportbase;
			if(thisSession.audio_rxPort_z == 0)
				thisSession.audio_rxPort_z = remoteportbase;

			if(is_ipv6)
			{
				if (inet_pton(AF_INET6, hostname, &remoteIP) != 1){
					printf("inet_pton(hostip) err\n");
					return -1;
				}
			}
			else
			{
				if (inet_pton(AF_INET, hostname, &remoteIP) != 1){
					printf("inet_pton(hostip) err\n");
					return -1;
				}
			}
			//initialize rtp object
			if( t_mode == ISMA)
			{
				mp4parameter->video_ssrc = video_ssrc;
				mp4parameter->audio_ssrc = audio_ssrc;
				mp4parameter->host_addr= hostname;
				mp4parameter->video_rx_port = rtpReceivePort;
				mp4parameter->video_tx_port = thisSession.video_rxPort_z;
				mp4parameter->audio_rx_port = rtpReceivePort;//+2;
				mp4parameter->audio_tx_port = thisSession.audio_rxPort_z;
			}
			else
			{		
				mp2parameter->my_ssrc = video_ssrc;
				mp2parameter->host_addr= hostname;
				mp2parameter->rx_port = rtpReceivePort;
				mp2parameter->tx_port = thisSession.video_rxPort_z;
			}
			return 0;

		}
		else
		{
			//zxy add 12.2
			set_rtsperrno(ERR_VOD_CONNECT_MSVOD);
			return -1;
		}
			
	}
	else
	{
		//zxy add 12.2
		set_rtsperrno(ERR_VOD_URL_INVALID);
		printf( "Error: Could not connect to Server" );
		return -1;
	}
	return 0;
}

bool RTSPClient::establishConnection()
{
	bool connectionStatus = true;
	//struct sockaddr_in server_addr;
	//struct hostent *he;
	struct addrinfo hints, *res, *resave;
	int iret;
	struct timeval timeout;
	int n;
	int error;
	socklen_t len;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	thisSession.sock=-1;
	error = getaddrinfo(hostname, "554", &hints, &res);
	if (error) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		connectionStatus=false;
		return connectionStatus;
	}

#if 1
	resave = res;
	do{
		thisSession.sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (thisSession.sock < 2)
			continue;

		int on =1;
		setsockopt(thisSession.sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		unsigned long ul = 1;
		iret = ioctl(thisSession.sock, FIONBIO, (unsigned long*)&ul);
		if(iret==-1)
		{
			close(thisSession.sock);
			fprintf(stderr,"ioctl error\n");
			return false;
		}

		fd_set rset,wset;
		FD_ZERO(&rset);
		FD_SET(thisSession.sock, &rset);
		wset=rset;
		timeout.tv_sec =10 ; 
		timeout.tv_usec =0;
		if((n=connect(thisSession.sock, res->ai_addr, res->ai_addrlen))<0)
			if(errno!=EINPROGRESS)
			{
				perror("error");
				return false;
			}
		if(n==0)
			break;
		if((iret =select(thisSession.sock+1,   &rset, &wset,NULL, &timeout))==0)
		{
			close(thisSession.sock);
			thisSession.sock=-1;
			printf("timeout \n");
			continue;
		}
		if(FD_ISSET(thisSession.sock,&rset)||FD_ISSET(thisSession.sock,&wset))
		{
			len=sizeof(error);
			if(getsockopt(thisSession.sock, SOL_SOCKET,SO_ERROR , &error,&len)<0)
			{
				close(thisSession.sock);
				thisSession.sock=-1;
				continue;
			}
			break;
		}

	}while((res = res->ai_next) != NULL);

	if (res == NULL){
		printf("connect to server not\n");
		connectionStatus=false;
	}
		freeaddrinfo(resave);
	if(error)
	{
		close(thisSession.sock);
		thisSession.sock=-1;
		connectionStatus=false;
	}
	if(connectionStatus)
	{
		unsigned long ul1= 0 ;
		iret = ioctl(thisSession.sock, FIONBIO, (unsigned long*)&ul1);
		if(iret==-1){
			fprintf(stderr,"ioctl error\n");
			close(thisSession.sock);
			thisSession.sock=-1;
			connectionStatus=false;
		}
	}
#endif



	if (thisSession.sock< 0) {
		fprintf(stderr, "can't connect %s\n", hostname);
		connectionStatus=false;
	}
	else
	{
		timeout.tv_sec=2;
		timeout.tv_usec=0;
		setsockopt(thisSession.sock,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));

		timeout.tv_sec=5;
		timeout.tv_usec=0;
		setsockopt(thisSession.sock,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
		//fprintf(stderr,"connect to the server\n");
	}

	return (connectionStatus);
}


int  RTSPClient::rtspDESCRIBE()
{
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "DESCRIBE rtsp://%s/%s/%d %s %s\r\nCSeq: %u\r\nAccept: application/sdp\r\nUser-Agent:3n media player\r\n\r\n",
			hostname,firstdir, thisSession.rtsp_txPort, filename, RTSP_VER, thisSession.cseq_tx++);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "DESCRIBE rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nAccept: application/sdp\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,firstdir,filename, RTSP_VER, thisSession.cseq_tx++);
		else
			sprintf(tx_buffer, "DESCRIBE rtsp://%s/%s %s\r\nCSeq: %u\r\nAccept: application/sdp\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,filename, RTSP_VER, thisSession.cseq_tx++);
	}

	printf("before SENT:rtspDESCRIBE\n");
	if(sendData()==false)
		return -1;
	//printf("have SENT:rtspDESCRIBE\n");
	return 0;
}

int RTSPClient::rtspVideoSETUP(char *mode,int *video_port)
{
	int len =0;
	char *item_ptr;
	bool currentStatus = true;

//以下为视频请求
	 if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	 {
	 	sprintf(tx_buffer, "SETUP rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nTransport: RTP/AVP;unicast",
			hostname, thisSession.rtsp_txPort, firstdir,filename, RTSP_VER, thisSession.cseq_tx++);
	}
	 else
	 {
	 	if(dir_true)
	 		sprintf(tx_buffer, "SETUP rtsp://%s/%s/%s/trackID=%d %s\r\nCSeq: %u\r\nTransport: RTP/AVP;unicast",
				hostname,firstdir, filename, videotrackid, RTSP_VER, thisSession.cseq_tx++);
		else
	 		sprintf(tx_buffer, "SETUP rtsp://%s/%s/trackID=%d %s\r\nCSeq: %u\r\nTransport: RTP/AVP;unicast",
				hostname,filename, videotrackid, RTSP_VER, thisSession.cseq_tx++);
	}
    
	len = strlen(tx_buffer);

	sprintf(tx_buffer+len,";client_port=%u-%u;mode=%s\r\nUser-Agent:3n media player\r\n\r\n",thisSession.video_rxPort, thisSession.video_rxPort+1,mode);

	printf("SEND rtspSETUP Video\n");
	if(sendData()==false)
		return -1;

	//get session number from response
	item_ptr = strstr(rx_buffer, "Session: ");
	if(item_ptr)
	{
		if(!sscanf(item_ptr, "Session: %lld", &(thisSession.session))) 
		{
			currentStatus = false;
			printf("Error: No Session number provided");
		}
	}
	else
	{
		printf("Error: No video Session number provided");
		return -1;
	}
	//get server rtp receive-port if recording data
	item_ptr = strstr(rx_buffer, "ssrc=");
	if(item_ptr)
	{
		if(!sscanf(item_ptr, "ssrc=%x", &video_ssrc))
		{
			currentStatus = false;
			printf("Error: video_ssrc provided");
		}
		printf("video ssrc %ld\n",video_ssrc);
	}
	else
	{
		printf("Error: No video_ssrc provided");
		return -1;
	}
	
 	item_ptr = strstr(rx_buffer, "server_port=");
	if(item_ptr)
	{
	 	if(!sscanf(item_ptr, "server_port=%d-", video_port))
	 	{
	 		currentStatus = false;
	 		printf("Error: No Server RTP Port Provided\n");
	 	}
	}
	else
	{
		printf("no video port provided\n");
		return -1;
	}
	return 0;
}

int RTSPClient::rtspAudioSETUP(char *mode,int*audio_port)
{
	int len =0;
	char *item_ptr;
	bool currentStatus = true;
	//以下为音频请求，加入thisSession.session
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "SETUP rtsp://%s:%s/%d/%s %s\r\nCSeq: %u\r\nTransport: RTP/AVP;unicast",
			hostname, firstdir, thisSession.rtsp_txPort, filename, RTSP_VER, thisSession.cseq_tx++);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "SETUP rtsp://%s/%s/%s/trackID=%d %s\r\nCSeq: %u\r\nTransport: RTP/AVP;unicast",
				hostname, firstdir,filename, audiotrackid, RTSP_VER, thisSession.cseq_tx++);
		else
			sprintf(tx_buffer, "SETUP rtsp://%s/%s/trackID=%d %s\r\nCSeq: %u\r\nTransport: RTP/AVP;unicast",
				hostname,filename, audiotrackid, RTSP_VER, thisSession.cseq_tx++);
	}
	   
	len = strlen(tx_buffer);

	printf("send rtspAudioSETUP \n");
	//sprintf(tx_buffer+len,";client_port=%u-%u\r\nSession: %lld\r\n\r\n",thisSession.audio_rxPort, thisSession.audio_rxPort+1,thisSession.session); 
	sprintf(tx_buffer+len,";client_port=%u-%u;mode=%s\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",thisSession.audio_rxPort, thisSession.audio_rxPort+1,mode,thisSession.session); 
	//printf("SEND rtspSETUP Audio\n");
	if(sendData()==false)
		return -1;
	 
	//get session number from response
	item_ptr = strstr(rx_buffer, "Session: ");
	if(item_ptr)
	{
		if(!sscanf(item_ptr, "Session: %lld", &(thisSession.session)))
		{
			currentStatus = false;
			printf("Error: No Session number provided");
		}
	}
	else
	{
		printf("Error: No audio Session number provided");
		return -1;
	}

	item_ptr = strstr(rx_buffer, "ssrc=");
	if(item_ptr)
	{
		if(!sscanf(item_ptr, "ssrc=%x", &audio_ssrc))
		{
			currentStatus = false;
			printf("Error: audio_ssrcprovided");
		}
		printf("audio ssrc %ld\n",audio_ssrc);
	}
	else
	{
		printf("Error: No audio_ssrc provided");
		return -1;
	}
	
	 item_ptr = strstr(rx_buffer, "server_port=");
	if(item_ptr)
	{
	 	if(!sscanf(item_ptr, "server_port=%d-", audio_port))
	 	{
	 		currentStatus = false;
	 		printf("Error: No Server RTP Port Provided\n");
	 	}
	}
	else
	{
		printf("no audio port provided\n");
		return -1;
	}
		
	return 0;
}



unsigned long RTSPClient::rtspGetParameter()
{
	char * timestamp_ptr1;
	char * timestamp_ptr2;
	char * tmp=NULL;
	double gettime=0;
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "GET_PARAMETER rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nLocation: npt\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "GET_PARAMETER rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nLocation: npt\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++,thisSession.session);
		else
			sprintf(tx_buffer, "GET_PARAMETER rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nLocation: npt\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, filename, RTSP_VER, 
				thisSession.cseq_tx++,thisSession.session);
	}
	//printf("rtspGetParameter,tx_buffer is %s\n",tx_buffer);
	if(sendData()==false)
		return 0;
	//printf("rtspGetParameter,buf is %s\n",rx_buffer);
	tmp=strstr(rx_buffer,"Location: ntp=");
	if(tmp)
		sscanf(tmp, "Location: ntp=%lf", &gettime);
	//printf("rtspGetParameter,get time is %lf\n",gettime);
	return (unsigned long)gettime;
}

int RTSPClient::rtspPLAY(double PlayTime)
{
	char * timestamp_ptr1;
	char * timestamp_ptr2;
	printf("the npt time before nptend %f\n",nptend);
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nSpeed: 1.000\r\nRange: npt=%f-%f\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session,PlayTime,nptend);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nSpeed: 1.000\r\nRange: npt=%f-\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++,thisSession.session,PlayTime);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nSpeed: 1.000\r\nRange: npt=%f-\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, filename, RTSP_VER, 
				thisSession.cseq_tx++,thisSession.session,PlayTime);
	}

	printf("SENT:rtspPLAY time %f\n",PlayTime);
	if(sendData()==false)
		return -1;

	return 0;
}

int RTSPClient::rtspPLAY_seq(double PlayTime,unsigned short seqnum)
{
	char * timestamp_ptr1;
	char * timestamp_ptr2;
	printf("the npt time before nptend %f seqnum %u\n",nptend,seqnum);
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=%f-%f\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session,PlayTime,nptend);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSeqNumber: %u\r\nSession: %lld\r\nRange: npt=%f-\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++,seqnum,thisSession.session,PlayTime);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s %s\r\nCSeq: %u\r\nSeqNumber: %u\r\nSession: %lld\r\nRange: npt=%f-\r\n\r\n",
				hostname, filename, RTSP_VER, 
				thisSession.cseq_tx++,seqnum,thisSession.session,PlayTime);
	}

	printf("SENT:rtspPLAY time %f\n",PlayTime);
	if(sendData()==false)
		return -1;

	return 0;
}

int RTSPClient::rtspPAUSEPLAY()
{
	char * timestamp_ptr1;
	char * timestamp_ptr2;
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,  filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);
	}
    
	//printf(":rtspPAUSEPLAY\n");
	if(sendData()==false)
		return -1;

	return 0;
}


int RTSPClient::rtspresumePlay()
{
	char * timestamp_ptr1;
	char * timestamp_ptr2;
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=now-%f\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,  filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session,nptend);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=now-%f\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session,nptend);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=now-%f\r\n\r\n",
				hostname,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session,nptend);
	}
    
	printf(":rtspresumePLAY\n");
	if(sendData()==false)
		return -1;

	return 0;
}


int RTSPClient::rtspStep(double FastStepTime)
{
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=%f-\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir, filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session, FastStepTime);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=%f-\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session, FastStepTime);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=%f-\r\n\r\n",
				hostname, filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session, FastStepTime);
	}

	printf("SENT:step to%f\n",FastStepTime);
	if(sendData()==false)
		return -1;
	return 0;
}

//不带时间的vcr请求
int RTSPClient::rtspVcrRequest(int speed)
{
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir, filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s/trackID= %d %s\r\nCSeq: %u\r\nSpeed: %d\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename,videotrackid,RTSP_VER, 
				thisSession.cseq_tx++, speed,thisSession.session);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/trackID= %d  %s\r\nCSeq: %u\r\nSpeed: %d\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, filename, videotrackid,RTSP_VER, 
				thisSession.cseq_tx++, speed, thisSession.session);
	}
	printf("send vcr request %d no time\n",speed);
	if(sendData()==false)
		return -1;
	return 0;
}


/*带时间的vcr请求*/
int RTSPClient::rtspVcrRequest(int speed, double FastStepTime)
{
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PLAY rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nRange: npt=%f-\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir, filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session, FastStepTime);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/%s/trackID= %d %s\r\nCSeq: %u\r\nSpeed: %d\r\nSession: %lld\r\nRange: npt=%f-\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename,videotrackid,RTSP_VER, 
				thisSession.cseq_tx++, speed,thisSession.session, FastStepTime);
		else
			sprintf(tx_buffer, "PLAY rtsp://%s/%s/trackID= %d  %s\r\nCSeq: %u\r\nSpeed: %d\r\nSession: %lld\r\nRange: npt=%f-\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, filename, videotrackid,RTSP_VER, 
				thisSession.cseq_tx++,speed, thisSession.session, FastStepTime);
	}

	printf("SENT:vcr  %f speed %d with time\n",FastStepTime,speed);
	if(sendData()==false)
		return -1;
	return 0;
}
int RTSPClient::rtspRECORD()
{
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "RECORD rtsp://%s:%d/%s %s\r\nCSeq: %u\r\nSession: %lld\r\n\r\n",
			hostname, thisSession.rtsp_txPort, filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		sprintf(tx_buffer, "RECORD rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\n\r\n",
			hostname, filename, RTSP_VER, 
			thisSession.cseq_tx++, thisSession.session);
	}

	printf("SENT:rtspRECORD\n");
	if(sendData()==false)
		return -1;
	return 0;
}

int RTSPClient::rtspPAUSE()
{
	//char * timestamp_ptr;
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PAUSE rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,  filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PAUSE rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);
		else
			sprintf(tx_buffer, "PAUSE rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);			
	}

	//printf("SENT:rtspPAUSE\n");
	if(sendData()==false)
		return -1;
	return 0;
}


int RTSPClient::rtspnowPAUSE()
{
	//char * timestamp_ptr;
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "PAUSE rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort, firstdir,  filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "PAUSE rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);
		else
			sprintf(tx_buffer, "PAUSE rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);			
	}

	//printf("SENT:rtspPAUSE\n");
	if(sendData()==false)
		return -1;
	return 0;
}


int RTSPClient::rtspOPTIONS()
{
	char * location_ptr1;
	char * location_ptr2;
	char *ptr;
	bool currentStatus;
	char command[128];
	int numFields;
	char port[64];

	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		if(dir_true)
			sprintf(tx_buffer, "OPTIONS rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,thisSession.rtsp_txPort,firstdir,filename, RTSP_VER, thisSession.cseq_tx++);
		else
			sprintf(tx_buffer, "OPTIONS rtsp://%s:%d/%s %s\r\nCSeq: %u\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,thisSession.rtsp_txPort,filename, RTSP_VER, thisSession.cseq_tx++);	
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "OPTIONS rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nUser-Agent:VLC media player\r\n\r\n",
				hostname,firstdir,filename, RTSP_VER, thisSession.cseq_tx++);
		else
			sprintf(tx_buffer, "OPTIONS rtsp://%s/%s %s\r\nCSeq: %u\r\nUser-Agent:VLC media player\r\n\r\n",
				hostname,filename, RTSP_VER, thisSession.cseq_tx++);		
	}

	printf("SENT:rtspoptions\n");
	if(sendData()==false)
		return -1;

	location_ptr1 = strstr(rx_buffer,"Location: ");
	if(location_ptr1!=NULL)
	{
		//printf("location: %s\n",location_ptr1);
		location_ptr2 = strstr(location_ptr1+1,"\r");
		if(location_ptr2)
		{
			memset(command,0,sizeof(command));
			if(location_ptr2-location_ptr1-10 <128)
				memcpy(command,location_ptr1+10, location_ptr2-location_ptr1-10);
			else
				return -1;
			//printf("new get location %s\n",command);
			//printf("a 0x%x com 0x%x\n",strstr(command, "rtsp://"),command);
			if(strstr(command, "rtsp://")==command)
			{
				ptr = command + 7;
				printf("host name %s\n",ptr);
				location_ptr1 = strchr(ptr, ':');
				if(location_ptr1)
				{
					memset(hostname,0,sizeof(hostname));
					memcpy(hostname,ptr,location_ptr1-ptr);
					memset(port,0,sizeof(port));
					memcpy(port,location_ptr1+1,strlen(ptr)-strlen(hostname)-1);
					thisSession.rtsp_txPort= atoi(port);
				}
				else
					strcpy(hostname,ptr);
			}
			else 
				return -1;
			
			//if(currentStatus == true)
			{
				close(thisSession.sock);
				thisSession.cseq_tx = 0;
				if(!establishConnection())
					return -1;
			}

		}
	}
	return 0;
}


int RTSPClient::rtspTEARDOWN()
{
	if(thisSession.rtsp_txPort != DEFAULT_TX_PORT)
	{
		sprintf(tx_buffer, "TEARDOWN rtsp://%s:%d/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
			hostname, thisSession.rtsp_txPort,firstdir, filename, 
			RTSP_VER, thisSession.cseq_tx++, thisSession.session);
	}
	else
	{
		if(dir_true)
			sprintf(tx_buffer, "TEARDOWN rtsp://%s/%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname, firstdir, filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);
		else
			sprintf(tx_buffer, "TEARDOWN rtsp://%s/%s %s\r\nCSeq: %u\r\nSession: %lld\r\nUser-Agent:3n media player\r\n\r\n",
				hostname,  filename, RTSP_VER, 
				thisSession.cseq_tx++, thisSession.session);
	}

	if(sendData()==false)
		return -1;
	return 0;
}


bool RTSPClient::sendData()
{
	int len = 0;
	int bytes_sent = 0;
	bool currentStatus = true;
	struct iovec data_vec[1];

	len = strlen(tx_buffer);
	//printf("send data %d\n",len);
	if(!alive)
		return false;
	
	data_vec[0].iov_base = tx_buffer;
	data_vec[0].iov_len = len;
	//	sleep(1);
	bytes_sent = writev(thisSession.sock,  &(data_vec[0]), 1);

	if(bytes_sent < len)
	{
		currentStatus = false;
		//zxy add 12.2
		set_rtsperrno(ERR_VOD_RTSP_SEVER_CLOSE);
		perror("Error: Failed to send" );	
		//printf("send data %d\n",bytes_sent);
	//	sleep(1);
		return currentStatus;
	}
	else
	{
		//printf("send data %d\n",bytes_sent);
		currentStatus = getResponse( );
	}
	return(currentStatus);
} 

bool RTSPClient::getResponse( )
{
	bool currentStatus = true;
	char ver[MAX_STRING_LEN];
	char message[MAX_STRING_LEN];
	int statusCode;
	char *end;
	struct iovec data_vec[1];
	data_vec[0].iov_base = rx_buffer;
	data_vec[0].iov_len = MAX_BUFFER_LEN-1;
	/*for decribe infomay be devided into two packets,we should receive again*/
	bytes_received = 0;
	int bytes_received2;
	if((bytes_received = readv(thisSession.sock,  &(data_vec[0]), 1)) <=0)
	{
		//zxy add 12.2
		set_rtsperrno(ERR_VOD_RTSP_SEVER_CLOSE);
		printf("****sbyte received %d\n",bytes_received);
		alive=false;
		currentStatus = false;
		return currentStatus;
	}
	else
	{
		//put string terminator at end of data
		printf("byte received %d\n",bytes_received);
		rx_buffer[bytes_received] = NULL_CHAR;
		sscanf(rx_buffer, "%s%d%s", ver, &statusCode, message);
		if(statusCode != OK_RESPONSE)
		{
			currentStatus = false;
			printf( "Error: Server returned error:%d %s\n    " ,statusCode, message );
			//zxy add 12.2
			if(statusCode == 404)
				set_rtsperrno(ERR_VOD_RTSP_404NOT);
			else if(statusCode == 403)
				set_rtsperrno(ERR_VOD_RTSP_403FORBIDDEN);
			else
				set_rtsperrno(ERR_VOD_RTSP_500INNERERR);
		}
		else if(!(strstr(rx_buffer+bytes_received-4, "\r\n")))
		{
			//printf("receive again\n");
			data_vec[0].iov_base = rx_buffer+bytes_received;
			data_vec[0].iov_len = MAX_BUFFER_LEN-bytes_received-1;
			if((bytes_received2 = readv(thisSession.sock,  &(data_vec[0]), 1)) == -1)
			{
				currentStatus = false;
				perror("recv bad\n");
				//printf("decribe d \n");
			}
			else
				rx_buffer[bytes_received+bytes_received2] = NULL_CHAR;
			bytes_received+=bytes_received2;
			printf("byte received2 %d\n",bytes_received2);
		}
		
	}
	return(currentStatus);
}

bool RTSPClient::parseRtspAddress(const unsigned char *command)
{
	char * string_ptr = NULL;
	char * string_ptr1=NULL ;
	char *string_ptr2=NULL;

	
	int numFields,num;
	bool currentStatus = true;

	numFields = sscanf((char *)command, "rtsp://%s", hostname);
	//printf("hostname %s\n",hostname);

	if(numFields == 1)
	{	
		string_ptr = strchr(hostname, '/');

		if(string_ptr != NULL)
		{
			thisSession.rtsp_txPort = DEFAULT_TX_PORT;
			memcpy(hostip,hostname,string_ptr-hostname);
			hostip[string_ptr-hostname]='\0';
			printf("host ip is %s\n",hostip);
			
			string_ptr1=string_ptr;
			do{
				string_ptr2=string_ptr1;
				string_ptr1=strchr(string_ptr2+1, '/');
			}while(string_ptr1!=NULL);
			
			if(string_ptr2==string_ptr)
			{
				printf("no dir in url\n");
				strcpy(filename, string_ptr+1);
				currentStatus=true;
				dir_true=false;
			}
			else
			{
				int dirlen=string_ptr2-string_ptr;
				memcpy(firstdir,string_ptr+1,dirlen-1);
				firstdir[dirlen-1]='\0';
				//printf("dir %s\n",firstdir);
				strcpy(filename, string_ptr2+1);
				//printf("filename %s\n",filename);	
				dir_true=true;
				currentStatus=true;
			}
		}
		else
		{
			currentStatus = false;
		}   			
	}
    else
    {
	currentStatus = false;
    }
    
    if(currentStatus == false)
    {
	printf("Error in address: %s", command );
	     	
    }
	//printf("FILENAME $$$$$$$$$$$$$  %s\n",filename);
      return(currentStatus);    
}




#endif

int RTSPClient::parse_sdp(mp4parameter_t *mp4parameter)
{
	char * id_ptr=NULL;
	char *id_config=NULL;
	unsigned char configdata[1024];
	id_ptr = strstr(rx_buffer, "a=range:npt=");

	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "a=range: npt = %lf", &nptstart)) 
		{
			printf("Error: 11No NPT provided\n");
			return -1;
		}
	}
	else
	{
		printf("not range information\n");
		//return -1;
	}

	id_ptr = strstr(id_ptr+1, "-");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "-%lf", &nptend)) 
		{
			printf("Error: 22No NPT provided\n");
			return -1;
		}
	}
	else
	{
		printf("not npt information\n");
		//return -1;
	}		

	printf("\n\nNPT@@@@ %g  %f\n\n\n", nptstart,nptend);
	mp4parameter->duration=nptend;
	duration = nptend;
	mp4parameter->max_hour=(int)nptend/3600;
	mp4parameter->max_minute=((int)nptend%3600)/60;
	mp4parameter->max_second=(int)nptend%60;

	id_ptr = strstr(rx_buffer+1, "a=rtpmap:96");
	if(id_ptr!=NULL)
	{
		printf("id_ptr  a=rtpmap: %s\n",id_ptr);
		if(sscanf(id_ptr, "a=rtpmap:96 MP4V-ES/%ld", &mp4parameter->videotimescale)) 
		{
			mp4parameter->is_264 = false;
		}
		else if(sscanf(id_ptr, "a=rtpmap:96 H264/%ld", &mp4parameter->videotimescale)) 
		{
			mp4parameter->is_264 =true;
		}
		else
		{
			printf("Error: No video timescale provided");
			return -1;
		}
		printf("video time scale %ld \n",mp4parameter->videotimescale);

	}
	else
	{
		printf("not media video  information\n");
		return -1;
	}
	
	id_ptr = strstr(rx_buffer+1, "a=rtpmap:97");
	if(id_ptr!=NULL)
	{
		//id_ptr=strstr(id_ptr+1,"a=rtpmap:96MP4V-ES/");
		if(!sscanf(id_ptr, "a=rtpmap:97 mpeg4-generic/%ld", &mp4parameter->audiotimescale)) 
		{
			printf("Error: No audio timescale provided");
			mp4parameter->is_aac=false;
			mp4parameter->audiotimescale= 90000;
		}
		else
			mp4parameter->is_aac=true;
		printf("audio time scale %ld \n",mp4parameter->audiotimescale);

	}
	else
	{
		mp4parameter->is_aac=false;
		printf("not AAC\n");
		//return -1;
	}

	
	id_ptr = strstr(rx_buffer, "a=control:trackID=");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "a=control:trackID=%d", &videotrackid)) 
		{
			printf("Error: No video ID provided");
			return -1;
		}
	}
	else
	{
		printf("not video id information\n");
		//return -1;
	}
	fprintf(stderr," videotrackid %d\n",videotrackid);

	mp4parameter->videotrackid = videotrackid;

	
	id_ptr = strstr(id_ptr+1, "a=control:trackID=");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "a=control:trackID=%d", &audiotrackid)) 
		{
			printf("Error: No AUDIO ID provided");
			return -1;
		}
	}
	else
	{
		printf("not audio trackid information\n");
		//return -1;
	}
	fprintf(stderr," audiotrackid %d\n",audiotrackid);

	mp4parameter->audiotrackid = audiotrackid;
	memset(configdata,0,1024);
	//fprintf(stderr," audiotrackid %d\n",audiotrackid);
	//	sleep(1);

	/*video dsi*/
	id_config = rx_buffer;
	if(!mp4parameter->is_264)
	{
	 	id_ptr = strstr(rx_buffer,"config=");
		if(id_ptr!=NULL)
		{
			 if(!sscanf(id_ptr,"config=%s",configdata))
			 {
				  printf("!!!no video config information found\n");
				  return -1;
			 }
		}
		else
		{
			printf("not video config information\n");
			return -1;
		}
		id_config=strstr(id_ptr+1,"\r");
		if(id_config==NULL)
		{
			printf("no config end information\n");
			return -1;
		}
			
	 	mp4parameter->videodsisize=id_config-id_ptr-7;
	 	unsigned long i;
		memset(mp4parameter->configvideo,0,256);
		mp4parameter->configvideo[mp4parameter->videodsisize]='\0';
	 
	 	for(i=0;i<mp4parameter->videodsisize;i++)
	 	{
	 		if(configdata[i]>=0x30 && configdata[i]<=0x39)
	 	  		mp4parameter->configvideo[i/2]  =mp4parameter->configvideo[i/2] | ((configdata[i]&0x0F)<<(4-(i%2)*4));
	 	 	else if(configdata[i]>=0x61 && configdata[i]<=0x66)
	 			mp4parameter->configvideo[i/2]  =mp4parameter->configvideo[i/2] | ((configdata[i]-0x57)<<(4-(i%2)*4));
	 		else
	 			mp4parameter->configvideo[i/2] =mp4parameter->configvideo[i/2] | ( (configdata[i]-0x37)<<(4-(i%2)*4));
	 	}

		printf("\n video ConfigInfo:\n");
		for(i=0;i<mp4parameter->videodsisize/2;i++)
			printf("%.2x",*(mp4parameter->configvideo+i));
		mp4parameter->videodsisize/=2;

	}
	else
	{
		//get 264 nal paras
		id_ptr = strstr(rx_buffer,"sprop-parameter-sets=");
		if(id_ptr == NULL)
		{
			printf("not sprop-parameter-sets  config information\n");
			return -1;
		}
		
		id_config=strstr(id_ptr+1,"\r\n");
		if(id_config==NULL)
		{
			printf("no config end information\n");
			return -1;
		}
		char *ptr_tmp = strstr(id_ptr+1,";");
		if(ptr_tmp)
		{
			if(ptr_tmp < id_config)
			{
				//find ; before \r\n,so use ; as the delimitation
				id_config = ptr_tmp;
			}
		}

			
	 	int size =id_config-id_ptr-21;
		unsigned char header_patch[]={0x0,0x0,0x0,0x1};
		memcpy(configdata, id_ptr+21, size);
		printf("configdata %s\n",configdata);
	 	unsigned long i=0,j,ret_base64;
		unsigned char *ptr_head, *ptr_tail;
		memset(mp4parameter->configvideo,0,256);
		ptr_head = configdata;
		while((ptr_tail = (unsigned char *)strstr((char *)ptr_head,","))&& ptr_tail < configdata+size)
		{
			memcpy(mp4parameter->configvideo + i, header_patch, sizeof(header_patch));
			i += sizeof(header_patch);
			printf("len %d ptr tail %s\n",ptr_tail-ptr_head,ptr_tail);
			ret_base64 = base64decode(ptr_head, ptr_tail-ptr_head, mp4parameter->configvideo + i, 256);
			if(ret_base64 < 0)
			{
				printf("cannot basedecode parameter\n");
				return -1;
			}
			i += ret_base64;
			ptr_head = ptr_tail+1;
			for(j=0;j<i;j++)
				printf("%.2x",*(mp4parameter->configvideo+j));	
			printf("\n");
		}
		
		memcpy(mp4parameter->configvideo + i, header_patch, sizeof(header_patch));
		i += sizeof(header_patch);
		ret_base64 = base64decode(ptr_head, configdata+size-ptr_head, mp4parameter->configvideo + i, 256);
		if(ret_base64 < 0)
		{
			printf("cannot basedecode parameter\n");
			return -1;
		}
		i += ret_base64;
		mp4parameter->videodsisize = i;
		mp4parameter->configvideo[mp4parameter->videodsisize]='\0';
		for(j=0;j<i;j++)
			printf("%.2x",*(mp4parameter->configvideo+j));	
		printf("\n");
	 
		
	}
	if(mp4parameter->is_aac)
	{
		int i = 0;
		memset(configdata,0,1024);
		id_ptr = strstr(id_config+1,"config=");
		if(id_ptr!=NULL)
		{
		
			memset(mp4parameter->configaudio,0,256);
			mp4parameter->audiodsisize=4;
			mp4parameter->configaudio[mp4parameter->audiodsisize]='\0';
 			//memcpy(configdata,"1190",4);
 			if(rx_buffer+strlen(rx_buffer)>id_ptr+7+4)
 				memcpy(configdata,id_ptr+7,4);
			else
			{
				printf("no audio end information%d %d\n",sizeof(rx_buffer),strlen(rx_buffer));
				return -1;
			}
		}
		else
		{
			printf("not audio config information\n");
			return -1;
		}
	 	for(i=0;i<mp4parameter->audiodsisize;i++)
	 	{
	 		if(configdata[i]>=0x30 && configdata[i]<=0x39)
	 	  		mp4parameter->configaudio[i/2]  = mp4parameter->configaudio[i/2] | ((configdata[i]&0x0F)<<(4-(i%2)*4));
 		 	else if(configdata[i]>=0x61 && configdata[i]<=0x66)
 				mp4parameter->configaudio[i/2]  = mp4parameter->configaudio[i/2] | ((configdata[i]-0x57)<<(4-(i%2)*4));
 			else
 				mp4parameter->configaudio[i/2] = mp4parameter->configaudio[i/2] | ( (configdata[i]-0x37)<<(4-(i%2)*4));
 		}

		printf("\n audio ConfigInfo:\n");
		for(i=0;i<mp4parameter->audiodsisize/2;i++)
			printf("%.2x",*(mp4parameter->configaudio+i));
		mp4parameter->audiodsisize/=2;
		printf("\n");
	}
	return 0;
}



int RTSPClient::parse_m2t(char *rx_buffer)
{
	char * id_ptr=NULL;
	char *id_config=NULL;
	unsigned char configdata[1024];

	id_ptr = strstr(rx_buffer, "a=range:npt=");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "a=range: npt = %lf", &nptstart)) 
		{
			printf("Error: 11No NPT provided\n");
			return -1;
		}
	}
	else
	{
		printf("not range information\n");
		//return -1;
	}
	id_ptr = strstr(id_ptr+1, "-");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "-%lf", &nptend)) 
		{
			printf("Error: 22No NPT provided\n");
			return -1;
		}
	}
	else
	{
		printf("not npt information\n");
		//return -1;
	}		

	printf("\n\nNPT@@@@ %g  %f\n\n\n", nptstart,nptend);
	duration=nptend;



	
	id_ptr = strstr(rx_buffer, "a=control:trackID=");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "a=control:trackID=%d", &videotrackid)) 
		{
			printf("Error: No video ID provided");
			return -1;
		}
	}
	else
	{
		printf("not video id information\n");
		//return -1;
	}
	fprintf(stderr," videotrackid %d\n",videotrackid);



	
	id_ptr = strstr(id_ptr+1, "a=control:trackID=");
	if(id_ptr!=NULL)
	{
		if(!sscanf(id_ptr, "a=control:trackID=%d", &audiotrackid)) 
		{
			printf("Error: No AUDIO ID provided");
			return -1;
		}
	}
	else
	{
		printf("not audio trackid information\n");
		//return -1;
	}

	fprintf(stderr," audiotrackid %d\n",audiotrackid);
	return 0;
	
}








