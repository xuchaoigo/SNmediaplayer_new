#ifndef _NTP_INTERFACE_H_
#define _NTP_INTERFACE_H_

#define BUFFSIZE 128

#define NTP_VN           4
#define NTP_MODE_CLIENT  3
#define NTP_MODE_SERVER  4
#define NTP_BUFFSIZE 128
#define NTP_TIMEOUT_SEC  1
#define NTP_TIMEOUT_USEC 0
#define ntp_li(x)   ( (x>>6) & 3 )
#define ntp_vn(x)   ( (x>>3) & 7 )
#define ntp_mode(x) (  x     & 7 )
#define min( a, b ) ( (a>b)?(b):(a) )
#define my_abs( x )  ( (x<0)?(-x):(x) )

typedef struct {
    unsigned int sec;
    unsigned int fsec;
} ntp_time;

struct pkt {
    unsigned char li_vn_mode;      /* leap indicator, version and mode */
    unsigned char stratum;         /* peer stratum */
    unsigned char ppoll;           /* peer poll interval */
    char          precision;       /* peer clock precision */
    unsigned int  rootdelay;       /* distance to primary clock */
    unsigned int  rootdispersion;  /* clock dispersion */
    unsigned int  refid;           /* reference clock ID */
    ntp_time      reftime;         /* time peer clock was last updated */
    ntp_time      org;             /* originate time stamp */
    ntp_time      rec;             /* receive time stamp */
    ntp_time      xmt;             /* transmit time stamp */
};

#ifdef __cplusplus
extern "C" {
#endif

//从ntp服务器获取时间，设置本地时间
int SetTimeFromNtp(char * ntp_addr, unsigned short port);

#ifdef __cplusplus
}
#endif

#endif
