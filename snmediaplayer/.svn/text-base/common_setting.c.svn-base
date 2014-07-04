#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common_setting.h"
#include "xml_setting.h"

#define VOLUME "Volume"
#define MUTE "Mute"
#define SETTINGXMLNAME "/tango/IXApp/setting.xml"

int ifmute()
{
	char xmlvalue[256];

	memset(xmlvalue,0,256);
	Read_Xml_Setting((char*)SETTINGXMLNAME,(char*)MUTE,xmlvalue);

	if(xmlvalue[0]=='1')
		return 1;

	return 0;	
}


int setmute()
{
	Write_Xml_Setting((char*)SETTINGXMLNAME,(char*)MUTE,(char *)"1");
	return 0;	
}

int unsetmute()
{
	Write_Xml_Setting((char*)SETTINGXMLNAME,(char*)MUTE,(char *)"0");
	return 0;	
}

int save_volume_index(int volume)
{
	char xmlvalue[256];

	memset(xmlvalue,0,256);
	sprintf(xmlvalue, "%d",volume);
	Write_Xml_Setting((char*)SETTINGXMLNAME,(char*)VOLUME,xmlvalue);

	system("sync");
	return 0;	
}

int read_volume_index(int *volume)
{
	char xmlvalue[2048];
	int ret;
	
	memset(xmlvalue,0,sizeof(xmlvalue));

	ret=Read_Xml_Setting((char*)SETTINGXMLNAME,(char*)VOLUME,xmlvalue);
	if(ret)
	{
		volume=0;
		return -1;
	}
	
	*volume = atoi(xmlvalue);

	return 0;	
}

