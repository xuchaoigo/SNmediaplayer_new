#ifndef _TVMEDIAPLAYER
#define _TVMEDIAPLAYER

#include "SNmediaplayer.h"
#include "RealizeSNmediaplayer.h"

class TVMediaPlayer:class RealizeSNmediaplayer
{
	
public:
	/*���캯��*/
	TVMediaPlayer();

	/*��������*/
	~ TVMediaPlayer();

	play_state getPlayState();

	player_setvolume(int volumeindex);

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
	
	/*	0: ����TrickMode����
		1: ������TrickMode���� (Ĭ��ֵ)
	*/
	void setAllowTrickmodeFlag (int allowTrickmode);

/*	��ý����ʼ�㿪ʼ���š�
	��TV channel����ʵʱTV�ķ�ʽ��ʼ���š�
	*/
	 int playFromStart() ;

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
	 int playByTime(int type, int timestamp,int speed) ;

	/*��ͣ���ڲ��ŵ�ý��*/
	 int pause() ;

	/*	-speed��2��32*/
	 int fastForward(int speed) ;

	/*	-speed��-2��-32*/
	 int fastRewind(int speed) ;

	/*�ӵ�ǰý�����ͣ/���/����״̬�ָ��������š�*/
	 int resume() ;

	/*����ý��ĩ�˲���*/
	 int gotoEnd() ;//��ѡ��

	/*����ý����ʼ�㲥��*/
	 int gotoStart();//��ѡ��

	/*ֹͣ���ڲ��ŵ�ý�塣���ͷŻ����б��ز������������Դ��*/
	 int stop()  ;

	/*ֹͣ���ڲ��ŵ�ý�塣���ͷŻ����б��ز������������Դ��*/
	 int player_setwindowparms(int top, int left, int width, int height)  ;

	/*�л�������ѭ�����С�*/
	 int switchAudioChannel() ;//��ѡ��

	/*�л����죬ѭ�����С�*/
	 int switchAudioTrack();//��ѡ��

	/*�л���Ļ��ѭ�����С�*/
	 int switchSubtitle();//��ѡ��

	/*��ȡ��ǰ���ŵ�ý�����ʱ��,����������Ϊ��λ*/
	 int getMediaDuration();

	/*	��ȡý�岥�ŵ��ĵ�ǰʱ���
	��VoDΪ�Ӹ�ý����ʼ�㿪ʼ��������ʱ�䣬����Ϊ��λ��
	��TVoDΪ��ǰ���ŵ�ľ���ʱ�䣻
	��Channel����������
	�μ�RFC2326�е�Normal Play Time (NPT)��Absolute Time (Clock Time)����ʱ�����͵ĸ�ʽ
	*/
	 unsigned long getCurrentPlayTime();

	/*�������ĵ�ǰ����ģʽ
	�������ٰ���������ģʽ���͡�ģʽ��ز����� ������Ϣ��
	����ģʽ�֣�Normal Play��Pause��Trickmode��
	��ģʽΪTrickmodeʱ�����2x/-2x, 4x/-4x, 8x/-8x, 16x/-16x, 32x/-32x��������ʾ���/���˵��ٶȲ���
	*/
	 char * getPlaybackMode() ;

	/*	���ڻ�ȡ��һ�η����Ĵ������*/
	 int GetLastError();//��ѡ

	/*���ػ����е�ǰ���ŵ�Ƶ���ţ�
	���ܻ����Ч��Ƶ����ʱ������-1��
	*/
	 int getChannelNum() ;

};

#endif
