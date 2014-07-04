#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include "utf8_gb2312.h"
/*xuc 0718*/
/*
extern int gb2312_to_utf8(char * in_text, int in_len, char* out_utf8_text);
extern void Utf8ToGb2312(const unsigned char* inbuf, int len, unsigned char *outbuf);
extern int Gb2312ToUtf8(char * in_text, int in_len, char* out_utf8_text);//liucm 20110319
*/
int gb2312_to_utf8(char * in_text, int in_len, char* out_utf8_text)
{}

void Utf8ToGb2312(const unsigned char* inbuf, int len, unsigned char *outbuf)
{}
int Gb2312ToUtf8(char * in_text, int in_len, char* out_utf8_text)//liucm 20110319
{}


int  code_convert_new(char   *from_charset,char   *to_charset,char   *inbuf,int   inlen,char   *outbuf,int   outlen)   
{   
	iconv_t   cd;   
	int   rc;   
	char   **pin   =   &inbuf;   
	char   **pout   =   &outbuf;   

	cd   =   iconv_open(to_charset,from_charset);   
	if   ((int)cd<=0)   
	{
		printf("iconv_open error,cd ==0\n");
		iconv_close(cd);   
		return   -1;   
	}
	memset(outbuf,0,outlen);   
	if   (iconv(cd,pin,&inlen,pout,&outlen)==-1)   
	{
		perror("iconv error");

		iconv_close(cd);   
		return   -1;   
	}
	iconv_close(cd);   
	return   0;   
}   
//UNICODE码转为GB2312码   

int   u2g(char   *inbuf,int   inlen,char   *outbuf,int   outlen)   
{   
	//return   code_convert_new("UTF-8","GB2312",inbuf,inlen,outbuf,outlen);   
	memset(outbuf, 0, outlen);
	Utf8ToGb2312((unsigned char *)inbuf, inlen, (unsigned char *)outbuf);
	return 0;
}  


//GB2312码转为UNICODE码   
int   g2u(char   *inbuf,int   inlen,char   *outbuf,int   outlen)   
{   
	//return   code_convert_new("gb2312","utf-8",inbuf,inlen,outbuf,outlen);   
	//gb2312_to_utf8(inbuf, inlen, outbuf);
	Gb2312ToUtf8(inbuf, inlen, outbuf);
	return 0;

}   

