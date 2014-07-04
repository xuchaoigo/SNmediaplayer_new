#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "RTSPClient_prv.h"

//extern RTSPClient_prv g_rtsp_session_server;
//extern RTSPClient_prv g_rtsp_streaming_server;
//pthread_t ss_keep_alive_id;
//pthread_t ms_keep_alive_id;

RTSPClient_prv::RTSPClient_prv()
{
	memset(&session_infor, 0, sizeof(SessionInfor_prv_t));
	memset(&server_infor, 0, sizeof(ServerInfor_prv_t));	
	memset(contentID, 0, MAX_STRING_LEN_prv);
}
RTSPClient_prv::~RTSPClient_prv()
{

}
void RTSPClient_prv::init()
{
	if(strlen(server_infor.ip)<5 ||server_infor.port ==0)
		printf("input server_infor error !!\n");
	
	memset(tx_buffer, 0, MAX_BUFFER_LEN_prv);
	memset(rx_buffer, 0, MAX_BUFFER_LEN_prv);
	bytes_received=0;
	thisSession.sock = -1;
	thisSession.cseq_tx = 0;	

	memset(&session_infor, 0, sizeof(SessionInfor_prv_t));
}

bool RTSPClient_prv::establishConnection()
{
	bool connectionStatus = true;
	struct addrinfo hints, *res, *resave;
	int error;
	int iret;
	int n;
	struct timeval timeout;
	socklen_t len;
	char empty[1];
	
	empty[0] = '\0';
	strcpy(rx_buffer, empty);
	strcpy(tx_buffer, empty);
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	thisSession.sock=-1;
	memset(port, 0, 8);
	sprintf(port, "%u", server_infor.port);
	
	printf("establishConnection: server_infor.ip=%s, port=%d\n",server_infor.ip, server_infor.port);
	error = getaddrinfo(server_infor.ip, port, &hints, &res);
	if(error) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		connectionStatus = false;
		return connectionStatus;
	}
	resave = res;
	do
	{
		thisSession.sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(thisSession.sock < 2)
			continue;
		int on =1;
		iret = setsockopt(thisSession.sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if(iret==-1)		
		{
			close(thisSession.sock);	
			fprintf(stderr,"setsockopt error\n");
			return false;
		}
		unsigned long ul = 1;
		iret = ioctl(thisSession.sock, FIONBIO, (unsigned long*)&ul);
		if(iret==-1)
		{
			close(thisSession.sock);
			fprintf(stderr, "ioctl error\n");
			return false;
		}
		fd_set rset, wset;
		FD_ZERO(&rset);
		FD_SET(thisSession.sock, &rset);
		wset=rset;
		timeout.tv_sec = 2; 
		timeout.tv_usec =0;
		if((n=connect(thisSession.sock, res->ai_addr, res->ai_addrlen))<0)
		{
			if(errno!=EINPROGRESS)
			{
				close(thisSession.sock);	
				perror("error");
				return false;
			}
		}
		if(n==0)
			break;
		if((iret = select(thisSession.sock+1, &rset, &wset, NULL, &timeout))==0)
		{
			close(thisSession.sock);
			thisSession.sock=-1;
			printf("timeout\n");
			continue;
		}
		if(FD_ISSET(thisSession.sock,&rset)||FD_ISSET(thisSession.sock,&wset))
		{
			len=sizeof(error);
			if(getsockopt(thisSession.sock, SOL_SOCKET, SO_ERROR , &error, &len)<0)
			{
				close(thisSession.sock);
				thisSession.sock=-1;
				continue;
			}
			break;
		}
	}while((res = res->ai_next) != NULL);
	if (res == NULL)
		connectionStatus=false;
	freeaddrinfo(resave);
	if(error)
	{
		close(thisSession.sock);
		thisSession.sock = -1;
		connectionStatus = false;
	}
	if(connectionStatus)
	{
		unsigned long ul1= 0;
		iret = ioctl(thisSession.sock, FIONBIO, (unsigned long*)&ul1);
		if(iret == -1)
		{
			fprintf(stderr,"ioctl error\n");
			close(thisSession.sock);
			thisSession.sock = -1;
			connectionStatus = false;
		}
	}
	if (thisSession.sock < 0) 
	{
		fprintf(stderr, "can't connect to the server\n");
		connectionStatus = false;
	}
	else
	{
		printf("[%s()] thisSession.sock %d\n", __func__, thisSession.sock);
		timeout.tv_sec = 3;
		timeout.tv_usec = 5000;
		setsockopt(thisSession.sock,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof(timeout));
		timeout.tv_sec = 3;
		timeout.tv_usec = 5000;
		setsockopt(thisSession.sock,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof(timeout));
		fprintf(stderr,"connect to the server\n");
	}
	return connectionStatus;
}

bool RTSPClient_prv::reConnection(void)
{
	destroyConnection();
	return establishConnection();
}

void RTSPClient_prv::destroyConnection(void)
{
	if (thisSession.sock >= 0)
	{
		printf("close socket %d\n", thisSession.sock);
		close(thisSession.sock);
	}
	thisSession.sock = -1;
}

int RTSPClient_prv::send_command()
{
	int len = 0;
	int bytes_sent = 0;

	struct iovec data_vec[1];
	len = strlen(tx_buffer);


	data_vec[0].iov_base = tx_buffer;
	data_vec[0].iov_len = len;
	bytes_sent = writev(thisSession.sock, &(data_vec[0]), 1);
	printf("bytes_sent=%d\n", bytes_sent);
	if(bytes_sent < len)
	{
		perror("writev() ");
		return 1;
	}
	return 0;
}
int RTSPClient_prv::get_response()
{

	int bytes_recv=0;
	struct iovec data_vec[1];

	data_vec[0].iov_base = rx_buffer;
	data_vec[0].iov_len = MAX_BUFFER_LEN_prv-1;
	bytes_recv = readv(thisSession.sock, &(data_vec[0]), 1);
	printf("bytes_recv=%d\n", bytes_recv);
	if(bytes_recv < 0)
	{
		perror("readv():");
		return -1;
	}
	rx_buffer[bytes_recv]='\0';
	return bytes_recv;


}

int RTSPClient_prv::get_response_timeout(int sec, int usec)
{

	int bytes_recv=0;
	struct iovec data_vec[1];
	fd_set readfds;
	struct timeval tv;
	int ret;
	if (sec > 0 || usec > 0)
	{
		tv.tv_sec = sec;
	    	tv.tv_usec = usec;

		FD_ZERO(&readfds);
		FD_SET(thisSession.sock, &readfds);
		ret = select(thisSession.sock+1, &readfds, NULL, NULL, &tv);
		if(ret <= 0)
		{
			perror("select():");
			return -1;
		}
	}
		   
	data_vec[0].iov_base = rx_buffer;
	data_vec[0].iov_len = MAX_BUFFER_LEN_prv-1;
	bytes_recv = readv(thisSession.sock, &(data_vec[0]), 1);
	printf("bytes_recv=%d\n", bytes_recv);
	if(bytes_recv<0)
	{
		perror("readv():");
		return -1;
	}
	rx_buffer[bytes_recv]='\0';
	return bytes_recv;


}

int RTSPClient_prv::rtspSETUP(int setup_type)     // 1 - vod, 0-sdv
{
	char* substr_pos=NULL;
	char* mid_pos = NULL;
	char* end_pos = NULL;
	char *ptr;
	char ret_code[4] = {'\0'};

	//strcpy(contentID,"D199A258-4379-45E6-477F-3ED9FDB139A9");
	
	if(setup_type == SETUP_SDV)
	{
		printf("rtspSETUP sdv\n");
		sprintf(tx_buffer, "SETUP rtsp://%s:%u/?EntitlementCode=%s&type=sdv %s\r\nCSeq: %lu\r\nUser-Agent: stb123\r\nTransport: MP2T/DVBC/QAM;unicast;ServiceGroup=%u\r\nx-userID: %s\r\nEntitlementCode: %s\r\n\r\n",
				server_infor.ip, server_infor.port, contentID, RTSP_VER, thisSession.cseq_tx++, ServiceGroup, userID, contentID);
	}
	
	else if(setup_type == SETUP_VOD)
	{
		printf("rtspSETUP vod\n");
		sprintf(tx_buffer, "SETUP rtsp://%s:%u/?EntitlementCode=%s %s\r\nCSeq: %lu\r\nUser-Agent: stb123\r\nTransport: MP2T/DVBC/QAM;unicast;bit_rate=%ld;ServiceGroup=%u\r\nx-userID: %s\r\nEntitlementCode: %s\r\n\r\n",
				server_infor.ip, server_infor.port, contentID, RTSP_VER, thisSession.cseq_tx++, bit_rate, ServiceGroup, userID, contentID);
	}
	
	else if(setup_type == SETUP_TVS  ||setup_type == SETUP_TVOD)
	{
		//test tvs
		//strcpy(contentID,"MCP1TX0000000077");		
		if(!media_stop_time) //tvs
		{
			printf("rtspSETUP tvs\n");
			sprintf(tx_buffer, "SETUP rtsp://%s:%u/?EntitlementCode=%s %s\r\nCSeq: %lu\r\nUser-Agent: stb123\r\nTransport: MP2T/DVBC/QAM;unicast;ServiceGroup=%u\r\nx-userID: %s\r\nEntitlementCode: %s\r\nRange: npt=%ld-\r\n\r\n",
					server_infor.ip, server_infor.port, contentID, RTSP_VER, thisSession.cseq_tx++, ServiceGroup, userID, contentID, media_start_time);
		}
		else				//tvod
		{
			printf("rtspSETUP tvod\n");
	      //test tvod
			//media_stop_time = 1315596603;
			//strcpy(contentID,"MCP1TD0000000077");		
			sprintf(tx_buffer, "SETUP rtsp://%s:%u/?EntitlementCode=%s %s\r\nCSeq: %lu\r\nUser-Agent: stb123\r\nTransport: MP2T/DVBC/QAM;unicast;ServiceGroup=%u\r\nx-userID: %s\r\nEntitlementCode: %s\r\nRange: npt=%ld-%ld\r\n\r\n",
					server_infor.ip, server_infor.port, contentID, RTSP_VER, thisSession.cseq_tx++, ServiceGroup, userID, contentID, media_start_time,media_stop_time);
		}
	}
	else
	{
		printf("setup_type error!\n");
		return 1;
	}
	
	if(thisSession.cseq_tx == 999999999)
		thisSession.cseq_tx = 1;
	
	printf("setup.tx_buffer:\n%s\n",tx_buffer);

	if(send_command())
		return 1;
	if(get_response() <= 0)
		return 1;
	
	printf("setup.rx_buffer:\n%s\n", rx_buffer);

	ptr = strchr(rx_buffer,' ');
	strncpy(ret_code, ptr+1, 3);
	printf("ret_code = %s\n",ret_code);
	if (strcmp(ret_code,"200") != 0)
	{
		printf("rtspSETUP failed. ret_code = %s\n", ret_code);
		if(strcmp(ret_code, "302") == 0)		
		{
			printf("recv Redirect\n");
			//Location=rtsp://192.168.10.60:554
			substr_pos = strstr(rx_buffer, "Location");
			if(!substr_pos)
			{
				printf("Redirect miss Location!\n");
				return 302;
			}
			substr_pos += strlen("Location: ");
			end_pos = strstr(substr_pos+7, ":");
			if(!end_pos)
			{
				printf("Redirect miss port!\n");
				return 302;
			}		
			memset(server_infor.ip, 0, MAX_STRING_LEN_prv);
			strncpy(server_infor.ip, substr_pos +7, end_pos -substr_pos -7); 
			server_infor.port  = atoi(end_pos+1);
			printf("Redirect: ip = %s, port = %d\n", server_infor.ip, server_infor.port);
			return 302;
		}
		else
			return 1;
	}

	substr_pos = strstr(rx_buffer, "GlobalSession:");
	end_pos = strstr(substr_pos, "\r\n");
	if(!substr_pos)
	{
		printf("GlobalSession not found\n");
		return 3;
	}
	strncpy(session_infor.GlobalSessionID, substr_pos+strlen("GlobalSession: "), end_pos-substr_pos-strlen("GlobalSession: "));

	printf("GlobalSessionID = %s\n",session_infor.GlobalSessionID);
	
//xuc strip2012.10.26 解析的ms信息都缓存在ms_xxx里，上层负责拷贝
	strcpy(ms_GlobalSessionID, session_infor.GlobalSessionID);

	substr_pos = strstr(substr_pos+strlen("GlobalSession: "), "Session:");  //both have "Session"!
	if(!substr_pos)
	{
		printf("Session not found !!!\n\n");
		return 2;
	}
	end_pos = strstr(rx_buffer, "timeout=");
	if(!end_pos)
	{
		printf("timeout not found !!!\n\n");
		end_pos = strstr(substr_pos, "\r\n");
		strncpy(session_infor.sessionID, substr_pos+strlen("Session: "), end_pos-substr_pos-strlen("Session: ")-1);
		session_timeout = DEFAULT_SM_TIMEOUT;
	}	
	else
	{
		strncpy(session_infor.sessionID, substr_pos+strlen("Session: "), end_pos-substr_pos-strlen("Session: ")-1);
		session_timeout= atoi(end_pos+strlen("timeout="));
	}
	printf("sessionID = %s\ntimeout = %d\n",session_infor.sessionID, session_timeout);

	if(setup_type == SETUP_VOD ||setup_type == SETUP_TVS||setup_type == SETUP_TVOD)  //vod的ControlSession字段
	{
		substr_pos = strstr(rx_buffer, "ControlSession:");
		if(!substr_pos)
		{
			printf("ControlSession not found\n");
			return 2;
		}

		substr_pos = strstr(rx_buffer, "ControlSession:");
		mid_pos = strstr(substr_pos, "ControlHost:");
		end_pos = strstr(substr_pos, "ControlPort:");
		
		if(!substr_pos || !mid_pos||!end_pos)
		{
			printf("Control Session not found\n");
			return 2;
		}

//xuc strip2012.10.26
		strncpy(ms_sessionID, substr_pos+strlen("ControlSession: "), mid_pos-substr_pos-strlen("ControlSession: ")-1);

		strncpy(ms_ip, mid_pos+strlen("ControlHost:"), end_pos-mid_pos-strlen("ControlHost:")-1);
		ms_port = atoi(end_pos+strlen("ControlPort:"));
		printf("ms_port:%d\n",ms_port);
		
//xuc strip2012.10.26
		substr_pos = strstr(rx_buffer, "ControlTimeout=");   //可选字段，无此字段则设为默认值
		if(!substr_pos)
			ms_session_timeout = DEFAULT_MS_TIMEOUT;
		else
			ms_session_timeout = atoi(substr_pos+strlen("ControlTimeout="));

		printf("ms_sessionID = %s\n",ms_sessionID);
		printf("ms_GlobalSessionID = %s\n",ms_GlobalSessionID);
		printf("ms_ip = %s, ms_port = %d, ms_session_timeout(ControlTimeout) =%d\n",    \
											ms_ip, ms_port,ms_session_timeout);

	}
	
	substr_pos = strstr(rx_buffer, "Transport");
	if(!substr_pos)
	{
		printf("Transport not found\n");
		return 3;
	}
	substr_pos = strstr(rx_buffer, "programid=");
	if(!substr_pos)return 3;
	play_param.pnID = atoi(substr_pos+strlen("programid="));
	
	substr_pos = strstr(rx_buffer, "frequency=");	
	if(!substr_pos)return 3;
	play_param.frequency = atoi(substr_pos+strlen("frequency="));  //只取MHz的整数部分

		
	substr_pos = strstr(rx_buffer, "symbol_rate=");
	if(!substr_pos)return 3;
	//printf("symbolRate substr_pos = %p\n",substr_pos);
	play_param.symbolRate= atoi(substr_pos+strlen("symbol_rate="))*1000 ;
	//printf("symbolRate = %d\n",play_param.symbolRate);   
	
	substr_pos = strstr(substr_pos, ".");	
	if(!substr_pos)return 3;
	//printf("symbolRate substr_pos = %p\n",substr_pos);
	play_param.symbolRate +=atoi(substr_pos+strlen("."));
	//printf("symbolRate = %d\n",play_param.symbolRate);              //M symbol/s, 整数和小数,转换成k


	
	substr_pos = strstr(rx_buffer, "modulation=");	
	if(!substr_pos)return 3;
	play_param.mode= atoi(substr_pos+strlen("modulation="));
	play_param.ready = 1;

	printf("setup done. \nplay_param.frequency = %d\nplay_param.annex = %d\nplay_param.mode = %d\nplay_param.symbolRate = %d\nplay_param.pnID = %d\n", \
				play_param.frequency, play_param.annex, play_param.mode, play_param.symbolRate, \
				play_param.pnID);	
	return 0;

}

//PlayTime == -1表示npt =now
int RTSPClient_prv::rtspPLAY(long  PlayTime)
{
	char *ptr;
	char ret_code[4] = {'\0'};
	char* substr_pos=NULL;
//	char* end_pos = NULL;


	if(PlayTime == -1)
	{
			sprintf(tx_buffer, "PLAY rtsp://%s:%u %s\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\nRange: npt=now-\r\nScale: 1\r\n\r\n",
			server_infor.ip, server_infor.port, RTSP_VER, 
			thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID);
	}
	else
	{
			sprintf(tx_buffer, "PLAY rtsp://%s:%u %s\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\nRange: npt=%ld-\r\nScale: 1\r\n\r\n",
			server_infor.ip, server_infor.port, RTSP_VER, 
			thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID, PlayTime);
	}
	
	if(thisSession.cseq_tx == 999999999)
		thisSession.cseq_tx = 1;

	printf("rtspPLAY.tx_buffer:\n%s\n",tx_buffer);

	if(send_command())
		return -1;
	if(get_response() <= 0)
		return -1;
	
	printf("rtspPLAY.rx_buffer\n%s\n", rx_buffer);

	ptr = strchr(rx_buffer,' ');
	strncpy(ret_code, ptr+1, 3);
	printf("ret_code = %s\n",ret_code);
	if (strcmp(ret_code,"200") != 0)
	{
		printf("rtspPLAY failed. ret_code = %s\n", ret_code);
		return -1;
	}
//解析Range
	substr_pos = strstr(rx_buffer, "npt=");
	//end_pos = strstr(substr_pos, "-");
	if (substr_pos != 0)
		playing_time = atoi(substr_pos + 4);
 	//play_stop_time = atoi(end_pos + 1);
	printf("playing_time = %ld\n\n", playing_time );

	return 0;

}

int RTSPClient_prv::rtspTEARDOWN()
{
	//char *ptr;
	//char ret_code[4] = {'\0'};

	sprintf(tx_buffer, "TEARDOWN * %s\r\nCSeq: %lu\r\nUser-Agent: stb123\r\nSession: %s\r\nGlobalSession: %s\r\n\r\n",
			RTSP_VER, thisSession.cseq_tx++, session_infor.sessionID, session_infor.GlobalSessionID);

	printf("rtspTEARDOWN.tx_buffer:\n%s\n",tx_buffer);
	if(send_command())
		//return 1;
		printf("rtspTEARDOWN send_command failed\n");
	//if(get_response() <= 0)
	//	printf("rtspTEARDOWN recv reply timeout\n");
/*	if(get_response() <= 0)
		return 1;
	printf("rtspTEARDOWN.rx_buffer:\n%s\n", rx_buffer);

	ptr = strchr(rx_buffer,' ');
	strncpy(ret_code, ptr+1, 3);
	printf("ret_code = %s\n",ret_code);
	if (strcmp(ret_code,"200") != 0)
	{
		printf("rtspTEARDOWN failed. ret_code = %s\n", ret_code);
		return 1;
	}
*/
	close(thisSession.sock);
	return 0;
}

int RTSPClient_prv::rtspGETPARAMETER() 
{
  //发送、接收不带时间参数的心跳。  
  	char *ptr;
	char ret_code[4] = {'\0'};
//	char cmd[10] = {'\0'};

	printf("rtspGETPARAMETER\n");
	sprintf(tx_buffer, "GET_PARAMETER * %s\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\nContent-Length: 0\r\n\r\n",
			RTSP_VER, thisSession.cseq_tx++, session_infor.GlobalSessionID, session_infor.sessionID);
	if(thisSession.cseq_tx == 999999999)
		thisSession.cseq_tx = 1;
	
//	printf("rtspGETPARAMETER.tx_buffer:\n%s\n", tx_buffer);
	if(send_command())
		return -1;
	if(get_response() <= 0)
		return -1;
//	printf("rtspGETPARAMETER.rx_buffer:\n%s\n", rx_buffer);
/*
	strncpy(cmd, rx_buffer, 8);
	if (strcmp(cmd,"ANNOUNCE") == 0)
	{
		printf("recv ANNOUNCE!returns 1.\n");
		return -1;
	}
*/
	ptr = strchr(rx_buffer,' ');
	strncpy(ret_code, ptr+1, 3);
	printf("ret_code = %s\n",ret_code);
	if (strcmp(ret_code,"200") != 0)
	{
		printf("rtspGETPARAMETER failed. ret_code = %s\n", ret_code);
		return -1;
	}	
	
	return 0;
	
}

int RTSPClient_prv::rtspGETPARAMETER_notime() 
{
  //1.接收不带时间参数的心跳
  //2.接收查询播放时间的返回结果
 //3.接收ms 发出的各类ANNOUNCE
 	char *ptr;
	char ret_code[4] = {'\0'};
	char cmd[10] = {'\0'};
	char* substr_pos=NULL;
//	char* end_pos = NULL;
	char  event_code[5] = {'\0'};
	int ret;
	unsigned long  npt_time;

	fd_set readfds;
	struct timeval tv;

	while(1)
	{
		pthread_testcancel();
		tv.tv_sec = session_timeout;
    		tv.tv_usec = 0;
			
		printf("rtspGETPARAMETER_notime:\n");
		sprintf(tx_buffer, "GET_PARAMETER * %s\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\nContent-Length: 0\r\n\r\n",
				RTSP_VER, thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID);
		if(thisSession.cseq_tx == 999999999)
			thisSession.cseq_tx = 1;
		
		printf("rtspGETPARAMETER_notime.tx_buffer:\n%s\n", tx_buffer);

		if(send_command())
		{
			printf("heartbeat send_command error!\n");
		//	return -1;
		}
		if(get_response() <= 0)
		{
			printf("rtspGETPARAMETER_notime:get_response ret <= 0\n");
			ms_keep_alive_miss_cnt++;
			printf("ms_miss = %d, sleep %d.\n", ms_keep_alive_miss_cnt, session_timeout );
			if(ms_keep_alive_miss_cnt == KEEP_ALIVE_MISS_MAX)
			{
				if_keep_alive_error = 1;
				return -1;
			}	
			/*
			else if (ms_keep_alive_miss_cnt == (unsigned short)((KEEP_ALIVE_MISS_MAX + 1) / 2))
			{
	
				if(!reConnection())
				{
					printf("ms_miss reconnect failed\n");
					if_keep_alive_error = 1;
					return -1;
				}
			}
			*/
			pthread_testcancel();
			sleep(3);
			continue;
		}

		strncpy(cmd, rx_buffer, 8);
		if (strcmp(cmd, RTSP_VER) == 0)
		{
			ptr = strchr(rx_buffer,' ');
			strncpy(ret_code, ptr+1, 3);
			printf("[DEBUG] RTSPClient.cpp:rtspGETPARAMETER_notime() ret_code = %s\n",ret_code);
				
			if(strstr(rx_buffer, "position") != NULL)
			{
				printf("rtspGETPARAMETER_notime recv gettime, goto parse_gettime!");
				goto parse_gettime;
			}			
			if (strcmp(ret_code,"200") != 0)
			{
				printf("rtspGETPARAMETER failed. ret_code = %s\n", ret_code);
				return -1;
			}	
			printf("recv heartbeat!\n");
			ms_keep_alive_miss_cnt = 0;
		}
		else if(strcmp(cmd,"ANNOUNCE") == 0)
		{
			printf("rtspGETPARAMETER_notime recv ANNOUNCE, goto parse_ANNOUNCE!");
			goto parse_ANNOUNCE;
		}
		else
		{
			printf("Invalid command!\n");
		}

		pthread_testcancel();
		printf("select start, timeout = %d\n",session_timeout);
	       FD_ZERO(&readfds);
	       FD_SET(thisSession.sock, &readfds);
	       ret = select(thisSession.sock+1, &readfds, NULL, NULL, &tv);
		 if(!ret)
             {
                   printf("select time out\n");
     		   	continue;		
             }
		else if(ret == -1)
		{
			printf("select error\n");
			return -1;
		}
		else
		{
			if(FD_ISSET(thisSession.sock, &readfds))
		       {
				if(get_response() <= 0)
				{
					printf("select get_response ret <= 0!\n");
					continue;
					//return -1;		
				}

				printf("select.rx_buffer:\n\n%s\n", rx_buffer);
				
				strncpy(cmd, rx_buffer, 8);
				if (strcmp(cmd,"ANNOUNCE") == 0)
				{
	parse_ANNOUNCE:
					printf("recv ANNOUNCE!\n");
					//eg.  notice: 2101 End-of-Stream Reached Event-Date=utc-time npt=123
					substr_pos = strstr(rx_buffer, "notice:");
					if(!substr_pos)
					{
						printf("ANNOUNCE notice not found!\n");
						//return -1;
					}
					strncpy(event_code, substr_pos+strlen("notice: "), 4);  //length of event code
					printf("event_code = %s\n", event_code);
					
					switch(atoi(event_code))
					{
							case EOS_REACHED:
								player_status = PLAYER_QUIT;
								sprintf(tx_buffer, "%s %s End-of-Stream Reached\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\n\r\n",
									RTSP_VER, event_code, thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID);
								break;
							case TVS_REACHED_SDV:
								player_status = PLAYER_QUIT;
								sprintf(tx_buffer, "%s %s Transition\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\n\r\n",
									RTSP_VER, event_code, thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID);
								break;
							case BOS_REACHED:
								player_status = PLAYER_PLAY;
								sprintf(tx_buffer, "%s %s Begin-of-Stream Reached\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\n\r\n",
									RTSP_VER, event_code, thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID);
								break;
							default:
								printf("Unknown event_code!\n");
								continue;;
					}
					if(thisSession.cseq_tx == 999999999)
						thisSession.cseq_tx = 1;
					if(send_command())
					{
						printf("send ANNOUNCE reply error!\n");
						//return -1;
					}
					printf("ANNOUNCE reply.tx_buffer:\n%s\n", tx_buffer);
					continue;                   //心跳时间刷新

				}
				else if(strstr(rx_buffer, "position") != NULL)			//接收到时间回复
				{
parse_gettime:
					printf("recv gettime!\n");

					ptr = strchr(rx_buffer,' ');
					strncpy(ret_code, ptr+1, 3);
					printf("ret_code = %s\n",ret_code);
					if (strcmp(ret_code,"200") != 0)
					{
						printf("rtspGETPARAMETER gettime failed. ret_code = %s\n", ret_code);
						npt_time = 0;
						continue;
						//return -1;
					}	
	
					substr_pos = strstr(rx_buffer, "position");	
					if(!substr_pos)
					{
						printf("rtspGETPARAMETER_gettime: cannot get current npt time!\n");
						npt_time = 0;
						continue;
						//return -1;
					}
					npt_time = atoi(substr_pos+strlen("position:")); 
					printf("\nnpt_time = %ld\n\n",npt_time);
					playing_time = npt_time;
					continue;
				}
				else
				{
					printf("\n\nrecv invalid response!!!\n\n\n");   //可能接收到VCR响应?
					continue;
				}

			 }
			else
			{
				printf("readfds error!\n");
				return -1;
			}
		  }
	 }
	
	return 0;			//never get here
}
	

int RTSPClient_prv::rtspGETPARAMETER_gettime() 
{
  //发送请求时间信令。结果由rtspGETPARAMETER_notime处理。回应错误，或接收到Anounce,返回-1, 触发Teardown.
 // 返回获取的npt-sec时间
	//	char *ptr;
	//char ret_code[4] = {'\0'};
	//char cmd[10] = {'\0'};
	//int npt_time;
	//char* substr_pos=NULL;

	printf("rtspGETPARAMETER_gettime\n");
	sprintf(tx_buffer, "GET_PARAMETER * %s\r\nCSeq: %lu\r\nUser-Agent: stb123\r\nSession: %s\r\nGlobalSession: %s\r\nContent-Length: %d\r\n\r\nposition\r\n",
			RTSP_VER, thisSession.cseq_tx++, ms_sessionID, ms_GlobalSessionID, strlen("position"));
	if(thisSession.cseq_tx == 999999999)
		thisSession.cseq_tx = 1;
	
	printf("rtspGETPARAMETER_gettime.tx_buffer:\n%s\n", tx_buffer);
	if(send_command())
		return -1;

/*	
		if(get_response() <= 0)
			{
			printf("get_response() <= 0\n");

			return -1;
			}
			printf("recv gettime!\n");
			printf("gettime.rx_buffer:\n%s\n", rx_buffer);

			ptr = strchr(rx_buffer,' ');
			strncpy(ret_code, ptr+1, 3);
			printf("ret_code = %s\n",ret_code);
			if (strcmp(ret_code,"200") != 0)
			{
				printf("rtspGETPARAMETER failed. ret_code = %s\n", ret_code);
				return -1;
			}	

			substr_pos = strstr(rx_buffer, "position");	
			if(!substr_pos)
			{
				printf("rtspGETPARAMETER_gettime: cannot get current npt time!\n");
				return -1;
			}
			npt_time = atoi(substr_pos+strlen("position:")); 
			printf("npt_time = %d\n",npt_time);
*/
	return 0;
}

int RTSPClient_prv::ss_keep_alive_begin(void * sm_ka_args)
{
	int ret = 0;
	ret = pthread_create(&ss_keep_alive_id, NULL, ss_keep_alive_thread, sm_ka_args);
	if(ret != 0)
	{
		printf("can't create thread: %s\n",strerror(ret));
		return 1;
	}

	return 0;
}

int RTSPClient_prv::ms_keep_alive_begin(void * ms_ka_args)
{
	int ret = 0;
	ret = pthread_create(&ms_keep_alive_id, NULL, ms_keep_alive_thread, ms_ka_args);
	if(ret != 0)
	{
		printf("can't create thread: %s\n",strerror(ret));
		return 1;
	}

	return 0;
}

int RTSPClient_prv::ss_keep_alive_end()
{
	int ret = -1;
	printf("ss_keep_alive_end\n");
	if(ss_keep_alive_id)
	{
		ret = pthread_cancel(ss_keep_alive_id);
		ret |= pthread_join(ss_keep_alive_id, NULL);
		if(ret)
		{
			printf("ss_keep_alive_end error!!!ret = %d\n",ret);
			return -1;
		}
		ss_keep_alive_id = 0;
		printf("ss_keep_alive_end end,ret = %d\n",ret);
	}
	return 0;
}

int RTSPClient_prv::ms_keep_alive_end()
{
	int ret = -1;
	printf("ms_keep_alive_end\n");
	ret = pthread_cancel(ms_keep_alive_id);
	ret |= pthread_join(ms_keep_alive_id, NULL);
	if(ret)
	{
		printf("ms_keep_alive_end error!!!ret = %d\n",ret);
		return -1;
	}
	printf("ms_keep_alive_end end,ret = %d\n",ret);
	ms_keep_alive_id = 0;
	player_status = PLAYER_NONE;
	return 0;
}


int RTSPClient_prv::rtsp_forward(float scale)
{
	char* substr_pos=NULL;

 	sprintf(tx_buffer, "PLAY rtsp://%s:%u %s\r\nCSeq: %lu\r\nUser-Agent:123\r\nGlobalSession:%s\r\nSession:%s\r\nRange:npt=now-\r\nScale:%f\r\n\r\n",

	server_infor.ip, server_infor.port, RTSP_VER, 
	thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID, scale);

	printf("rtsp_forward.tx_buffer:\n%s\n", tx_buffer);

	if(send_command())
		return 1;
	if(get_response() <= 0)
		return 1;

	printf("rtsp_forward.rx_buffer:\n%s\n", rx_buffer);
//解析Range
	substr_pos = strstr(rx_buffer, "npt=");
	//end_pos = strstr(substr_pos, "-");
	if (substr_pos != 0)
		playing_time = atoi(substr_pos + 4);
 	//play_stop_time = atoi(end_pos + 1);
	printf("\nplaying_time = %ld\n\n", playing_time );

	return 0;

}

int RTSPClient_prv::rtsp_backward(float scale)
{
	char* substr_pos=NULL;

	sprintf(tx_buffer, "PLAY rtsp://%s:%u %s\r\nCSeq: %lu\r\nUser-Agent:123\r\nGlobalSession:%s\r\nSession:%s\r\nRange:npt=now-\r\nScale:-%f\r\n\r\n",
	server_infor.ip, server_infor.port, RTSP_VER, 
	thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID, scale);
	
	printf("rtsp_backward.tx_buffer:\n%s\n", tx_buffer);
	if(send_command())
		return 1;
	if(get_response() <= 0)
		return 1;

	printf("rtsp_backward.rx_buffer:\n%s\n", rx_buffer);
//解析Range
	substr_pos = strstr(rx_buffer, "npt=");
	//end_pos = strstr(substr_pos, "-");
	if (substr_pos != 0)
		playing_time = atoi(substr_pos + 4);
 	//play_stop_time = atoi(end_pos + 1);
	printf("playing_time = %ld\n\n", playing_time );

	return 0;

}
int RTSPClient_prv::rtsp_jump(long PlayTime)
{
  	sprintf(tx_buffer, "PLAY rtsp://%s:%u %s\r\nCSeq: %lu\r\nUser-Agent:123\r\nGlobalSession:%s\r\nSession:%s\r\nRange:npt=%ld-\r\nScale:1\r\n\r\n",

	server_infor.ip, server_infor.port, RTSP_VER, 
	thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID, PlayTime);
	
	printf("rtsp_jump.tx_buffer:\n%s\n", tx_buffer);

	if(send_command())
		return 1;
	if(get_response() <= 0)
		return 1;

	printf("rtsp_jump.rx_buffer:\n%s\n", rx_buffer);
	return 0;
}


int RTSPClient_prv::rtspPAUSE()
{
	char* substr_pos=NULL;
//	char* end_pos = NULL;
		

	sprintf(tx_buffer, "PAUSE * %s\r\nCSeq: %lu\r\nUser-Agent: 123\r\nGlobalSession: %s\r\nSession: %s\r\n\r\n",
	RTSP_VER, thisSession.cseq_tx++, ms_GlobalSessionID, ms_sessionID);

	printf("rtspPAUSE.tx_buffer:\n%s\n", tx_buffer);

	if(send_command())
		return 1;
	if(get_response() <= 0)
		return 1;
	
	printf("rtspPAUSE.rx_buffer:\n%s\n", rx_buffer);
	
//解析Range
	substr_pos = strstr(rx_buffer, "npt=");
	//end_pos = strstr(substr_pos, "-");
	if (substr_pos != 0)
		playing_time = atoi(substr_pos + 4);
 	//play_stop_time = atoi(end_pos + 1);
	printf("playing_time = %ld\n\n", playing_time );

	return 0;

}

void RTSPClient_prv::getPlayParam(PlayParam_t *pPlayParam)
{
	*pPlayParam = play_param;
	return;
}


void * ss_keep_alive_thread(void * sm_ka_args)
{
	printf("\nss_keep_alive_thread starts..\n");
	KeepAliveArgs_t * pkaa;
	pkaa = (KeepAliveArgs_t *) sm_ka_args;
	
	//g_rtsp_session_server.if_keep_alive_error = 0;
	pkaa->pRTSPClient->if_keep_alive_error =0;
	pkaa->pRTSPClient->ss_keep_alive_miss_cnt = 0;
	while(1)
	{
		pthread_testcancel();
		if(pkaa->pRTSPClient->rtspGETPARAMETER() == 0)
		{
			printf("sleep %d.\n",pkaa->timeout);
			sleep(pkaa->timeout );			    //心跳正常
		}
		else
		{
			pkaa->pRTSPClient->ss_keep_alive_miss_cnt++;
			printf("ss_miss = %d, sleep %d.\n",pkaa->pRTSPClient->ss_keep_alive_miss_cnt, pkaa->timeout);
			sleep(pkaa->timeout);
			if(pkaa->pRTSPClient->ss_keep_alive_miss_cnt == KEEP_ALIVE_MISS_MAX)
			{
				pkaa->pRTSPClient->if_keep_alive_error = 1;
				return NULL;
			}
		}


	}
	return NULL;
}


void * ms_keep_alive_thread(void * ms_ka_args)
{
	printf("\nms_keep_alive_thread starts..\n");

	KeepAliveArgs_t * pkaa;
	pkaa = (KeepAliveArgs_t *) ms_ka_args;
	
	pkaa->pRTSPClient->if_keep_alive_error = 0;
	pkaa->pRTSPClient->ms_keep_alive_miss_cnt = 0;
	//g_rtsp_streaming_server.rtspGETPARAMETER_gettime();    	
	pkaa->pRTSPClient->rtspGETPARAMETER_notime();
	
	//异常情况下才会返回
	printf("\nms_keep_alive_thread returns!\n");
	pkaa->pRTSPClient->if_keep_alive_error = 1;

	return NULL;
/*
	while(1)
	{
		if(g_rtsp_streaming_server.rtspGETPARAMETER_notime() == 0)
		{
			printf("sleep %d.\n",*(int *)ptimeout);
			sleep(*(int *)ptimeout );				 //心跳正常
		}
		else
		{
			g_rtsp_streaming_server.rtspTEARDOWN();//会话关闭
			return NULL;
		}		
	}
	
	return NULL;
	*/
}

