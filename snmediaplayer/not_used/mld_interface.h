#ifndef _MLD_INTERFACE_H
#define _MLD_INTERFACE_H

#define NTP_INTERVALS_INITIAL 180      //initial ntp intervals until ntp ok
#define NTP_INTERVALS     79800 //ntp intervals every 24 hours
enum {
	/* Max_response_time of MLD header is always 0,
	in order to be compatible with igmpv2.
	value of "maxtime" field of MLD header */
	MLD_HDR_MAXTIME = (0),

	/* values of "type" field of MLD header */
	MLD_TP_JOINREQ 		= 0x83,	/* BMG->SR->MS: joining request */
	MLD_TP_LEAVEREQ		= 0x84,	/* BMG->SR->MS: leaving request */
	//MLD_TP_JOINAWS 		= 0x72,	/* SR->BMG: answer of joining request */	
	//MLD_TP_LEAVEAWS		= 0x78,	/* SR->BMG: answer of leaving request */
	MLD_TP_QUERY                 = 0x82,
	MLD_TP_FORCELEAVE	= 0x7b	/* SR->BMG: force to leave multicast group */	
};

/* error no */
typedef enum{
	OK_MLD=0,//成功
	ERR_MLD_SOCKNOTCREATE = -1,
	ERR_MLD_CONNALREADYCREATED	= -2,
	ERR_MLD_CANTCREATESOCKET = -3,
	ERR_MLD_INVALIDPARAM	 = -4,
	ERR_MLD_INVALIDUSERID = -5,
	ERR_MLD_CANTRECVFROM		= -6,
	ERR_MLD_CANTSENDTO			= -7,
	ERR_MLD_CANTSIGNAL			= -8,
	ERR_MLD_NOTAMULTICASTADDRESS	= -9,
	ERR_MLD_AWSTIMEOUT			= -10,
	ERR_MLD_AWSREFUSE			= -11,
	ERR_MLD_MLDCREATE_FAILED	= -12,
	ERR_MLD_GETLINKLOCALADDR_FAILED	= -13,
	ERR_MLD_INET_PTON=-14
}MLD_RET_STATUS;

enum {
	/* size of user ID */
	MLD_SIZE_USERID = 16,
	/* size of MD5 sum, reference: RFC1321 */
	MLD_MD5_SIGNATURE_SIZE = 16,
	/* length of all fields needed to be calcuated MD5 digest:
 		group_ip(16) + bmg_ip(16) + user_id(MLD_SIZE_USERID) */
	MLD_SIZE_MD5_NEEDCALCUATE = 48,
	/* time of waiting for answer of join/leave-request */
	MLD_TIMEOUTSEC = 3,	
	/* the max number of join-request can be sent */
	MLD_MAXTIMES_SENDJOIN = 1		
};

struct hopbyhophdr{
	unsigned char nexthdr;
	unsigned char length;
	unsigned char ralert[4];
	unsigned short pad;
};

struct ipv6hdr {
	unsigned char priority:4,
				version:4;
	unsigned char	flow_lbl[3];

	unsigned short payload_len;
	unsigned char	nexthdr;
	unsigned char	hop_limit;

	struct	in6_addr	saddr;
	struct	in6_addr	daddr;
	struct    hopbyhophdr hophdr;
};

#define GetRandom( min, max ) ((rand() % (int)(((max) + 1) - (min))) + (min))

typedef struct Service_IP_Info_
{
	char MS_IP[46];
	char VASSIP[2][46];//两个
	char DRMSIP[46];
	char URSIP[46];
	char NTPIP[46];
}Service_IP_Info;

struct mld{
	unsigned char type;
	unsigned char code;
	unsigned short checksum;
	unsigned short maxtime;
	unsigned short reserved;
 	unsigned char groupaddr[16];
};

#ifdef __cplusplus
extern "C" {
#endif

//Mld响应服务器的mld查询报文
int MldQuery();
//Mld的done报文
MLD_RET_STATUS MldDone(char * mcastip);
//Mld的join报文
MLD_RET_STATUS MldJoin(char * mcastip);

int SetQueryMcastaddr(char *multicastip);

int UNSetQueryMcastaddr();

#ifdef __cplusplus
}
#endif
#endif
