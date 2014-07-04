#ifndef _XML_SETTING_H
#define _XML_SETTING_H


#ifdef __cplusplus
extern "C"{
#endif

int Read_Xml_Setting(char *xml_filename, char *type, char *value);

int Write_Xml_Setting(char *xml_filename, char *type, char *value);

#ifdef __cplusplus
}
#endif

#endif 
