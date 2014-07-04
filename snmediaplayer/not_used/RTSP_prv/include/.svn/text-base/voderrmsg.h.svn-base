#ifndef _VODERRMSG_H
#define _VODERRMSG_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/igmp.h>
#include <netinet/ip.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include <arpa/inet.h>
#include <netdb.h>

/*VOD error no */
enum VOD_ERRNO{
	ERR_VOD_UNKOWN = 0,
	ERR_VOD_CONNECT_MSMANAGER = 100,
	ERR_VOD_CONNECT_MSVOD 	= 101,
	ERR_VOD_URL_INVALID = 102,
	ERR_VOD_NO_SUCHFILE	 = 103,
	ERR_VOD_PARAERR = 104,
	ERR_VOD_SERVER_AVAIL = 105,
	ERR_VOD_SERVER_BUSY	= 106,
	ERR_VOD_RTSP_404NOT	= 107,
	ERR_VOD_RTSP_403FORBIDDEN	=108,
	ERR_VOD_RTSP_500INNERERR = 109,
	ERR_VOD_RTSP_SEVER_CLOSE = 110,
	ERR_VOD_RTSP_AUDIO_SETUP = 111,
	ERR_VOD_RTSP_VIDEO_SETUP = 112,
	ERR_VOD_RTSP_SDP_PARSE = 113,
	ERR_VOD_DECODER = 114,
	ERR_VOD_DEMUX = 115,
	ERR_VOD_NO_PMT = 116
};
/*
enum RTSP_ERRNO{
	ERR_RTSP_UNKOWN = 0,
	ERR_RTSP_WRONG_REQUEST = 100,
	ERR_RTSP_INVALID_AUTHORIZATION 	= 101,
	ERR_RTSP_URL_INVALID = 102,
	ERR_RTSP_NO_SUCHFILE	 = 103,
	ERR_VOD_PARAERR = 104,
	ERR_VOD_SERVER_AVAIL = 105,
	ERR_VOD_SERVER_BUSY	= 106,
	ERR_VOD_RTSP_404NOT	= 107,
	ERR_VOD_RTSP_403FORBIDDEN	=108,
	ERR_VOD_RTSP_500INNERERR = 109,
	ERR_VOD_RTSP_SEVER_CLOSE = 110,
};
*/
struct VodErrorStringStruct{
 	int errorvalue;
	char *errormessage;
};	

char *VodGetErrorString();

void set_voderrno(enum VOD_ERRNO voderrno);

#endif
