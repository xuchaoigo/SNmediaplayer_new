#include "voderrmsg.h"

static struct VodErrorStringStruct voderrorstrings[] =
{
 	{ ERR_VOD_CONNECT_MSMANAGER, "E100"},
 	{ ERR_VOD_CONNECT_MSVOD, "E101"},
 	{ ERR_VOD_URL_INVALID, "E102"},
	{ ERR_VOD_NO_SUCHFILE, "E103"},
	{ ERR_VOD_PARAERR, "E104"},
	{ ERR_VOD_SERVER_AVAIL, "E105"},
	{ ERR_VOD_SERVER_BUSY, "E106"},
 	{ ERR_VOD_RTSP_404NOT, "E107"},
 	{ ERR_VOD_RTSP_403FORBIDDEN, "E108"},
 	{ ERR_VOD_RTSP_500INNERERR, "E109"},
 	{ ERR_VOD_RTSP_SEVER_CLOSE, "E110"},
 	{ ERR_VOD_RTSP_AUDIO_SETUP, "E111"},
 	{ ERR_VOD_RTSP_VIDEO_SETUP, "E112"},
 	{ ERR_VOD_RTSP_SDP_PARSE, "E113"},
 	{ ERR_VOD_DECODER, "E114"},
 	
 	{ ERR_VOD_UNKOWN, NULL}	/* indicate the end of this list */
};

static enum VOD_ERRNO vod_errno=ERR_VOD_UNKOWN;

static char vodunknownerrstr[] ="Eundef";

void set_voderrno(enum VOD_ERRNO voderrno)
{
	vod_errno = voderrno;
}


char *VodGetErrorString()
{	
	unsigned char found;	/* found the information of @cmperrorno */
	unsigned char done;	/* searched to the end of error list */
	int i;

	if (vod_errno == ERR_VOD_UNKOWN)
		return "no err";

/*discard all player err*/
	/*if (vod_errno == ERR_VOD_SERVER_AVAIL)
	*/
	  return NULL;

	found = 0;
	done = 0;
	i = 0;
	while (!found && !done){
		if (voderrorstrings[i].errorvalue == ERR_VOD_UNKOWN)
			/* to the end of error list */
			done = 1;
		else if (voderrorstrings[i].errorvalue == vod_errno)
			found = 1;
		else
			i++;
	}

	if (found)
		return voderrorstrings[i].errormessage;	

	return vodunknownerrstr;
}

