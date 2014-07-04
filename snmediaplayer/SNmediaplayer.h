#ifndef _SNMEDIAPLAYER_H
#define _SNMEDIAPLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "RealizeSNmediaplayer.h"
using namespace std;

class SNmediaplayer
{
	
public:
	/*���캯��*/
	SNmediaplayer(void *mpobject, MediaplayerStateChangeCallback playstate_func);//�жϺ�instanceid����ֵ��Ĭ��Ϊ0

	/*��������*/
	virtual ~SNmediaplayer();

	/*���STB���ز�����ʵ����instanceID*/
	unsigned char getNativePlayerInstanceID(){return sn_instanceid;}

	/*��ȡ������״̬*/
	play_state getPlayState();

	/*	nativePlayerInstanceID:һ���ֽڳ��ȵ��޷���������0��255
		���أ�
		0�� ��ʾ�󶨳ɹ���
		��1�� ��ʾ��ʧ�ܣ�
		����ֵδ����
	*/
	int bindNativePlayerInstance(unsigned char nativePlayerInstanceID);//�޸�instanceidֵ

	/*��ʼ��MediaPlayer�����ԡ�
	���´�����Media Player��Ϊ�˺����е�STB����ĳ��ý�岥��ʵ��
	������ǰ����ҳ���д���Media Player����ʱ�����ģ����а󶨣�
	������øú�����
	���أ�
	0�� ��ʾ�󶨳ɹ���
	��1�� ��ʾ��ʧ�ܣ�
	����ֵδ����
	*/
	int initMediaPlayer(unsigned char nativePlayerinstanceID, int playlistFlag, int videoDisplayMode, 
		int height, int width, int left, int top, int muteFlag,  int useNativeUIFlag, int subtitleFlag, 
		int videoAlpha, int cycleFlag, int randomFlag, int autoDelFlag);

	/*�ͷŻ�����MediaPlayer�Ķ��󣬽�����ӦMediaPlayer���������ڡ�
	nativePlayerInstanceID:	һ���ֽڳ��ȵ��޷���������0��255
	���أ�
	0�� ��ʾ�ͷųɹ���
	��1�� ��ʾ�ͷ�ʧ�ܣ�
	����ֵδ����
	*/
	int releaseMediaPlayer (unsigned char nativePlayerInstanceID);//�ͷ�real_mediaplayer[nativePlayerinstanceID]��������

	/*���׹رձ��ز�����*/
	int releaseNativePlayer(unsigned char nativePlayerInstanceID);//ͬ��

	/*��ȡstb���ز��Ÿ���ʵ����instanceID
	type:0ȱʡʵ����1����������2���л�������������ֵԤ��*/
	unsigned char getNativePlayerInstanceIDByType(int type);
	/*���õ������ŵ�ý��*/
	int setSingleMedia(string mediaStr);

	/*�ڲ����б�ĩ�ˣ���ӵ���ý�塣
	index�����Ϊ��������-1������ʾĬ��������б�β����Append����
	��ָ����indexֵ���ڻ����playlist������ý����Ŀ���������playlistβ����
	�������Ϊ�����б�ĳ��λ�ã�Insert����
	index��������Ŀ���ƶ�/����/ɾ������̬�仯��
	��STB��֤��ǰplaylist��������Ŀ��index�Ǵ�0�����һ�����ġ�
	����entry_id��Ψһ��ʶplaylist��ÿ��ý����Ŀ��
	һ���ɹ�����ĳ��ý��󣬸�ֵ����仯��
	*/
	int addSingleMedia(int index, char *mediaStr){return 0;}//��ѡ��

	//�ڲ����б�ĩ�ˣ����һ��ý�塣
	int addBatchMedia(char *batchMediaStr){return 0;}//	��ѡ��

	/*	Ҫ���ն˷���ָ����Ƶ�������������ء�
	���ɱ�������Ϊ������Ƶ����Ҳ����-1��
	Ƶ����ַΪͨ��SetConfig���õ�Ƶ���б��еĵ�ַ	
	Userchannelid����������ʾ�û�Ƶ���š�
	����ֵ��0����ʾ�ɹ���
	-1����ʾƵ������Ч��
	*/
	int joinChannel(int userchannelid);

	/*Ҫ���ն��뿪ָ����Ƶ��������������
	����ֵ��0����ʾ�ɹ���
	-1����ʾƵ������Ч��
	*/
	int leaveChannel();

	/*�Ӳ����б���ɾ��ָ��ý��
	�����б���ĳ��ý����Ŀ��Ψһ��ʶ�������ý��ʱ���ò����ڸò����б��б��ֲ��䣩
	*/
	int removeMediaByEntryID(char *entryID){return 0;};//��ѡ��

	/*�Ӳ����б���ɾ��ָ��ý��
	index��������ý����ý���б��е�����ֵ
	*/
	int removeMediaByIndex(int index){return 0;}//	��ѡ��

	/*��ղ����б�
	����ֵ��0����ʾ�ɹ���
	-1����ʾʧ�ܡ�
	*/
	int clearAllMedia(){return 0;}//	��ѡ��

	/*	����index��ָ����ý���ڲ����б����ƶ�
	entryID: �����б���ĳ��ý����Ŀ��Ψһ��ʶ��
	toIndex����������Ҫ�ƶ���������ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaByIndex(char *entryID, int toIndex){return 0;}//	��ѡ��

	/*����ƫ������ָ����ý���ڲ����б����ƶ�
	entryID: �����б���ĳ��ý����Ŀ��Ψһ��ʶ��
	offset��ƫ��������������ʾ��ָ��ý�����б�ĩ���ƶ�����������ʾ��ָ��ý�����б���ʼ���ƶ�
	*/
	int moveMediaByOffset(char *entryID, int offset){return 0;}//��ѡ��

	/*����index��ָ����ý���ڲ����б����ƶ�
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	toIndex����������Ҫ�ƶ���������ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaByIndex1(int index, int toIndex){return 0;}//��ѡ��

	/*����ƫ������ָ����ý���ڲ����б����ƶ�
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	offset��ƫ��������������ʾ��ָ��ý�����б�ĩ���ƶ�����������ʾ��ָ��ý�����б���ʼ���ƶ�
	*/
	int moveMediaByOffset1(int index, int offset){return 0;}//��ѡ��

	/*��ָ����ý������*/
	int moveMediaToNext(char *entryID){return 0;}//	��ѡ��

	/*��ָ����ý������*/
	int moveMediaToPrevious(char *entryID){return 0;}//	��ѡ��

	/*��ָ����ý���Ƶ��б���*/
	int moveMediaToFirst(char *entryID){return 0;}//	��ѡ��

	/*��ָ����ý���Ƶ��б�ĩ��*/
	int moveMediaToLast(char *entryID){return 0;}//	��ѡ��

	/*��ָ����ý������
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToNext1(int index){return 0;}//	��ѡ��

	/*��ָ����ý������
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToPrevious1(int index){return 0;}//	��ѡ��

	/*��ָ����ý���Ƶ��б���
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToFirst1(int index){return 0;}//	��ѡ��

	/*��ָ����ý���Ƶ��б�ĩ��
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int moveMediaToLast1(int index){return 0;}//	��ѡ��

	/*��ý���ڲ����б��е�����ѡ��Ϊ��ǰ��ѡ���Ž�Ŀ
	index��������ָ����ý���ڲ����б��е�����ֵ��0����ʾ�����б�Ķ��ˡ�
	*/
	int selectMediaByIndex(int index){return 0;}//	��ѡ��

	/*���뵱ǰý��������ƫ����ѡ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ
	offset��ƫ��������������ʾ�ӵ�ǰý�����б�ĩ����ת����������ʾ�ӵ�ǰý�����б���ʼ����ת
	*/
	int selectMediaByOffset(int offset){return 0;}//	��ѡ��

	/*ѡȡ�����б��е���һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectNext(){return 0;}//	��ѡ��

	/*ѡȡ�����б��е���һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectPrevious(){return 0;}//	��ѡ��

	/*ѡȡ�����б��еĵ�һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectFirst(){return 0;}//	��ѡ��

	/*ѡȡ�����б��е����һ��ý�壬��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int selectLast(){return 0;}//	��ѡ��

	/*��ĳ��ý����Ŀ��Ψһ��ʶѡ��Ϊ��ǰ��ѡ���Ž�Ŀ*/
	int SelectMediaByEntryID(char *entryID){return 0;}//	��ѡ��

	/*	��ý����ʼ�㿪ʼ���š�
	��TV channel����ʵʱTV�ķ�ʽ��ʼ���š�
	*/
	int playFromStart();

	/*	�ӵ�ǰý���ĳ��ʱ��㿪ʼ����ý��
	����playlist������ָ��ǰ�Ѿ�ѡ�е�ý�壩��
	��ʵʱ���ŵ�TV channel�õ�����Ч��
	���Դ���ʱ��״̬��TV channel��Ч��
	-type:
	1���μ�RFC2326�е�Normal Play Time (NPT)
	2���μ�RFC2326�е�Absolute Time (Clock Time)
	-timestamp:
	�μ�RFC2326�е�Normal Play Time (NPT)��Absolute Time (Clock Time)����ʱ�����͵ĸ�ʽ
	timestamp:��VoD�����Ǵ�ý����ʼ�㿪ʼ��������ʱ�䣻��TVoD����ʱ�����ý����Ծ��Ǿ���ʱ�䡣
	-speed��
	�����ٶȣ���ѡ������
	*/
	int playByTime(int type, int timestamp,int speed);

	/*��ͣ���ڲ��ŵ�ý��*/
	int pause()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->pause();
	}

	/*	-speed��2��32*/
	int fastForward(int speed)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->fastForward(speed);
	}

	/*	-speed��-2��-32*/
	int fastRewind(int speed)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->fastRewind(speed);
	}

	/*�ӵ�ǰý�����ͣ/���/����״̬�ָ��������š�*/
	int resume()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->resume();
	}

	/*����ý��ĩ�˲���*/
	int gotoEnd()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->gotoEnd();
	}//��ѡ��

	/*����ý����ʼ�㲥��*/
	int gotoStart()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->gotoStart();
	}//��ѡ��

	/*ֹͣ���ڲ��ŵ�ý�塣���ͷŻ����б��ز������������Դ��*/
	int stop();

	/*	����videoDisplayMode, vedioDisplayArea����, ������Ƶ����ʾ��*/
	int refreshVideoDisplay();

	/*�л�������ѭ�����С�*/
	int switchAudioChannel()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->switchAudioChannel();

	}//��ѡ��

	/*�л����죬ѭ�����С�*/
	int switchAudioTrack()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->switchAudioTrack();
	}//��ѡ��

	/*�л���Ļ��ѭ�����С�*/
	int switchSubtitle()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->switchSubtitle();
	}//��ѡ��

	/*�������̵�������չ�������ɳ����Զ���*/
	int sendVendorSpecificCommand(char *xml){return 0;}

	/*������ƵPID��
	������ڲ��ŷ��ص�ǰ���ŵ�PID�����򷵻�ȱʡ/��һ��PID��
	*/
	int getAudioPID()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getAudioPID();
	}//��ѡ��

	/*	����һ������������ƵPID��Ϣ�Ķ���
	�������������Ϣ�� 
	pids ��ǰý�����а�������ƵPID���� 
	pidarr[1..pids]�C��ƵPID���飺 
	AudioPID�CPIDֵ����ֵ�� 
	AudioLanguage�CPID�����Ա�־���ַ�����
	*/
	pid_info *getAudioPIDs()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getAudioPIDs();
	}//��ѡ��

	/*ָ��ʹ�õ���Ƶpid*/
	void setAudioPID(int pid)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return ;
		return sn_real_mediaplayer[sn_instanceid]->setAudioPID(pid);
	}

	/*������ĻPID��������ڲ��ŷ��ص�ǰ���ŵ�PID�����򷵻�ȱ ʡ/��һ��PID*/
	int getSubtitlePID()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getSubtitlePID();
	}

	/*	����һ������������ĻPID��Ϣ�Ķ��󡣶������������Ϣ�� 
	pids�C��ǰý�����а�������ĻPID���� 
	pidarr[1..pids]�C��ĻPID���飺 
	SubtitlePID -PIDֵ����ֵ�� 
	SubtitleLanguage- PID�����Ա�־���ַ�����
	*/
	pid_info *getSubtitlePIDs()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getSubtitlePIDs();
	}

	/*ָ��ʹ�õ���Ļpid*/
	void setSubtitlePID(int pid)
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return;
		return sn_real_mediaplayer[sn_instanceid]->setSubtitlePID(pid);
	}

	/*Volume��0-100����ʾ������0Ϊ������100Ϊ�������*/
	void setVolume(int volume_value);

	/*����ֵ����ǰϵͳ������0-100*/
	int getVolume();

	/*	ioStr:Ҫ���õĲ��������ơ������������ɻ����ж��壩
		wrStr��Ҫ���õĲ�����ֵ��
	*/
	int set(string ioStr, string wrStr){return 0;};//��չʹ��

	/*ioStr:Ҫ��ȡ�Ĳ��������ơ������������ɻ����ж��壩*/
	string get(string ioStr){return NULL;};//��չʹ��

	/*���ò���������ģʽ
	   0: ��ý��Ĳ���ģʽ (Ĭ��ֵ)
	   1: �����б�Ĳ���ģʽ
	*/
	void setSingleOrPlaylistMode(int plmode){return;}//{return sn_real_mediaplayer[sn_instanceid]->setSingleOrPlaylistMode(plmode);}

	/*��ȡ����������ģʽ*/
	int getSingleOrPlaylistMode(){return 0;}//{return sn_real_mediaplayer[sn_instanceid]->getSingleOrPlaylistMode();}


	/*0: ��setVideoDisplayArea()���趨��Height, Width, Left, Top������ָ����λ�úʹ�С����ʾ��Ƶ
		   1: ȫ����ʾ����ȫ���߶ȺͿ����ʾ(Ĭ��ֵ)
		   2: �������ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ�������ʾ
		   3: ���߶���ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ���߶���ʾ
		   255: ��Ƶ��ʾ���ڽ����رա������ڱ���ý�������ӵ�ǰ���£�������Ƶ���ڡ������ý�岥��û�б���ͣ��������������Ƶ��

	   MediaPlayer�����Ӧ����Ƶ���ڵ���ʾģʽ.
		 ÿ�ε��øú�������Ƶ��ʾ���ڲ����ᱻ��������ˢ��
		 �Է�ӳ���ĺ����ʾЧ��ֻ�еȵ���ʽ����refreshVideoDisplay()��
		 �Ż�ˢ��	
	  */
	void setVideoDisplayMode(int vDisplayMode);

	int getVideoDisplayMode(){return sn_videoDisplayMode;}

	/*	MediaPlayer�����Ӧ����Ƶ���ڵ���ʾģʽ.
	ÿ�ε��øú�������Ƶ��ʾ���ڲ����ᱻ��������ˢ��
	�Է�ӳ���ĺ����ʾЧ��ֻ�еȵ���ʽ����refreshVideoDisplay()��
	�Ż�ˢ��
	*/
	void setVideoDisplayArea (int left, int top, int width, int height);
	
	int getVideoDisplayLeft(){return sn_videoDisplayrect.x;}

	int getVideoDisplayTop(){return sn_videoDisplayrect.y;}

	int getVideoDisplayWidth(){return sn_videoDisplayrect.width;}

	int getVideoDisplayHeight(){return sn_videoDisplayrect.height;}

	/*	0:	����Ϊ���� (Ĭ��ֵ)
		1:	����Ϊ����
	*/
	void setMuteFlag(int mute_flag);

	int getMuteFlag();//jiangy 

	/*	0: ��ʹ��Player�ı���UI��ʾ����
		1: ʹ��Player�ı���UI��ʾ���� (Ĭ��ֵ)
	*/
	void setNativeUIFlag (int nativeUI_flag);
	int getNativeUIFlag(){return sn_nativeUIFlag;}//��ѡ������

	/*	0: ��ʹ�ܾ�����ʾ�ı���UI��ʾ����
		1: ʹ�ܾ�����ʾ�ı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setMuteUIFlag (int muteUI_flag);

	int getMuteUIFlag(){return sn_muteUIFlag;}//��ѡ������

	/*	0: ��ʹ���������ڵı���UI��ʾ����
		1: ʹ���������ڵı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setAudioVolumeUIFlag (int audioVolumeUI_flag);

	int getAudioVolumeUIFlag(){return sn_audioVolumeUIFlag;}//��ѡ������

	/*	0: ��ʹ������ѡ��ı���UI��ʾ����
		1: ʹ������ѡ��ı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setAudioTrackUIFlag (int audioTrackUI_flag);

	int getAudioTrackUIFlag(){return sn_audioTrackUIFlag;}//��ѡ������

	/*	0: ��ʹ�ܽ������ı���UI��ʾ����
		1: ʹ�ܽ������ı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setProgressBarUIFlag (int progressBarUI_flag);

	int getProgressBarUIFlag(){return sn_progressBarUIFlag;}//��ѡ������

	/*	0: ��ʹ��Ƶ���ŵı���UI��ʾ����
		1: ʹ��Ƶ���ŵı���UI��ʾ���ܣ�Ĭ��ֵ��
	*/
	void setChannelNoUIFlag (int channelNoUI_flag);

	int getChannelNoUIFlag(){return sn_channelNoUIFlag;}//��ѡ������

	/*	0:	����ʾ��Ļ (Ĭ��ֵ)
		1:	��ʾ��Ļ
	*/
	void setSubtitileFlag (int subtitile_flag);

	int getSubtitileFlag(){return sn_subtitileFlag;}//��ѡ������

	/*	0��100֮�������ֵ��0��ʾ��͸����100��ʾ��ȫ͸����
		(Ĭ��ֵΪ0)
	*/
	void setVideoAlpha (int vAlpha);

	int getVideoAlpha(){return sn_videoAlpha;}//��ѡ������

	/*	0: ����TrickMode����
		1: ������TrickMode���� (Ĭ��ֵ)
	*/
	void setAllowTrickmodeFlag (int allowTrickmode_flag);

	int getAllowTrickmodeFlag(){return sn_allowTrickmodeFlag;}

	/*    0����Ϊѭ�����Ž�Ŀ(Ĭ��ֵ)
		1��ѭ��,���β���
	*/
	void setCycleFlag (int cycleFlag);

	int getCycleFlag(){return sn_cycleFlag;}

	/*    0����Ϊ������Ž�Ŀ(Ĭ��ֵ)
		1˳�򲥷�
	*/
	void setRandomFlag (int randomFlag);

	int getRandomFlag(){return sn_randomFlag;}

	/*	�������̵���չ�������úͻ�ȡ����ʽ�ɳ����Լ�ȷ��*/
	void setVendorSpecificAttr (char * vendorSpecificAttr){return;}

	char * getVendorSpecificAttr(){return NULL;}

	/*���õ�ǰ������Ϊ���л�ģʽ
	mode:1���л�������:��׼ģʽ*/
	void setPIPVideoMode(int pipmode);;

	/*��ȡ��ǰ�������Ļ��л�����*/
	int getPIPVideoMode(){return sn_pipmode;}
	/*	��ȡ��ǰ���ŵ�ý��ı�ʶ�� 
	MediaCode��ý���Ψһ��ʶ����Ŀ��Ƶ���ȣ���
	ʹ��ϵͳ�����ContentID�����������ڲ����б�ģʽʱ��
	MediaCode�Ƕ�̬�ı�ġ�
	*/
	string getMediaCode()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getMediaCode();
	}

	/*��ȡ��ǰ���ŵ�ý�����ʱ��,����������Ϊ��λ*/
	int getMediaDuration()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getMediaDuration();
	}

	/*	��ȡý�岥�ŵ��ĵ�ǰʱ���
	��VoDΪ�Ӹ�ý����ʼ�㿪ʼ��������ʱ�䣬����Ϊ��λ��
	��TVoDΪ��ǰ���ŵ�ľ���ʱ�䣻
	��Channel����������
	�μ�RFC2326�е�Normal Play Time (NPT)��Absolute Time (Clock Time)����ʱ�����͵ĸ�ʽ
	*/
	unsigned long getCurrentPlayTime()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getCurrentPlayTime();
	}

	/*�������ĵ�ǰ����ģʽ
	�������ٰ���������ģʽ���͡�ģʽ��ز����� ������Ϣ��
	����ģʽ�֣�Normal Play��Pause��Trickmode��
	��ģʽΪTrickmodeʱ�����2x/-2x, 4x/-4x, 8x/-8x, 16x/-16x, 32x/-32x��������ʾ���/���˵��ٶȲ���
	*/
	char * getPlaybackMode()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getPlaybackMode();
	}

	/*	���ڻ�ȡ��һ�η����Ĵ������*/
	int GetLastError()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->GetLastError();
	}//��ѡ

	/*���ػ����е�ǰ���ŵ�Ƶ���ţ�
	���ܻ����Ч��Ƶ����ʱ������-1��
	*/
	int getChannelNum()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return -1;
		return sn_real_mediaplayer[sn_instanceid]->getChannelNum();
	}

	/*	��ȡ��ǰ����������
	�ַ���Left��Right��Stereo��JointStereo
	*/
	char * getCurrentAudioChannel()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getCurrentAudioChannel();
	}

	/*��ȡ��ǰ�ĵ���������
	�ַ���Chinese��English��
	*/
	char * getAudioTrack()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getAudioTrack();
	}

	/*��ȡ��ǰѡ�����Ļ����
	�ַ���Chinese��English��
	*/
	char * getSubtitle()
	{
		if(!sn_real_mediaplayer[sn_instanceid])
			return NULL;
		return sn_real_mediaplayer[sn_instanceid]->getSubtitle();
	}

	/*��ȡ�����б��е�ý������*/
	int getMediaCount(){return sn_playlist_media_num;}//��ѡ��

	/*��ȡ��ǰ����ý�壬�ڲ����б��е�����ֵ
	0����ʾý���б�Ķ���
	*/
	int getCurrentIndex(){return 0;}//��ѡ��

	/*��ȡ��ǰ���ŵ�ý���EntryID*/
	string getEntryID();//��ѡ��

	/*�ַ�������JSON��ʽ���ַ�������װ�����б����ݡ�*/
	string getPlaylist();//��ѡ��

	
	static int registerChannel(string channel_info);

	static int setevent(MediaPlayErrorType ineventtype);

	static int registerChannelInfo(string name, string value);


protected:
		//ע��Ƶ��
	static vector<string> sn_channel_list;
	static vector<int> userchannelid_list;
	static RealizeSNmediaplayer *sn_real_mediaplayer[MAX_PLAYER_NUM];
	unsigned char sn_instanceid;//0��1
	int sn_volume;//0-100����ʾ������0Ϊ������100Ϊ�������
	int sn_playlist_media_num;//��¼�����б���ý������
	int sn_playmode;//	0: ��ý��Ĳ���ģʽ (Ĭ��ֵ);1: �����б�Ĳ���ģʽ
	/*0: ��setVideoDisplayArea()���趨��Height, Width, Left, Top������ָ����λ�úʹ�С����ʾ��Ƶ
      	   1: ȫ����ʾ����ȫ���߶ȺͿ����ʾ(Ĭ��ֵ)
      	   2: �������ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ�������ʾ
      	   3: ���߶���ʾ��ָ�ڲ��ı�ԭ��ͼ���ݺ�ȵ�����°�ȫ���߶���ʾ
      	   255: ��Ƶ��ʾ���ڽ����رա������ڱ���ý�������ӵ�ǰ���£�������Ƶ���ڡ������ý�岥��û�б���ͣ��������������Ƶ��
      	*/
	int sn_videoDisplayMode;
	PlayerRect sn_videoDisplayrect;
	int sn_muteflag;//0:  ����Ϊ���� (Ĭ��ֵ);1:  ����Ϊ����
	int sn_nativeUIFlag;//0: ��ʹ��Player�ı���UI��ʾ����;1: ʹ��Player�ı���UI��ʾ���� (Ĭ��ֵ)
	int sn_muteUIFlag;//: ��ʹ�ܾ�����ʾ�ı���UI��ʾ����;1: ʹ�ܾ�����ʾ�ı���UI��ʾ���ܣ�Ĭ��ֵ��
	int sn_audioVolumeUIFlag;//0: ��ʹ���������ڵı���UI��ʾ����;1: ʹ���������ڵı���UI��ʾ���ܣ�Ĭ��ֵ��
	int sn_audioTrackUIFlag;//0: ��ʹ������ѡ��ı���UI��ʾ����;1: ʹ������ѡ��ı���UI��ʾ���ܣ�Ĭ��ֵ��
	int sn_progressBarUIFlag;//0: ��ʹ�ܽ������ı���UI��ʾ����;1: ʹ�ܽ������ı���UI��ʾ���ܣ�Ĭ��ֵ��
	int sn_channelNoUIFlag;//0: ��ʹ��Ƶ���ŵı���UI��ʾ����;1: ʹ��Ƶ���ŵı���UI��ʾ���ܣ�Ĭ��ֵ��
	int sn_subtitileFlag;//0:  ����ʾ��Ļ (Ĭ��ֵ);	1:  ��ʾ��Ļ
	int sn_videoAlpha;//0��100֮�������ֵ��0��ʾ��͸����100��ʾ��ȫ͸��(Ĭ��ֵΪ0)
	int sn_allowTrickmodeFlag;//0: ����TrickMode����;1: ������TrickMode���� (Ĭ��ֵ)
	int sn_playlistFlag;
	int sn_cycleFlag;
	int sn_randomFlag;
	int sn_autoDelFlag;
	int sn_pipmode;// 1���л�������:��׼ģʽ
	string sn_entryID;// �����б���������
	//string mediacode;//ý���Ψһ��ʶ
	int sn_speed;//-32,-16,-8,-4,-2,0,...32
	string sn_json_str;
	void *sn_mpobject;
	
	MediaplayerStateChangeCallback sn_playstate_func;
};

#endif
