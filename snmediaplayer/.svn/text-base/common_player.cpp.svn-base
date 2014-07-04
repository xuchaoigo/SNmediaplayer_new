#include "common_player.h"
#include "showaudioinfo.h"
#include "ui_display.h"
#include "common_setting.h"

CommonPlayer::CommonPlayer()
{
}

CommonPlayer::~ CommonPlayer()
{
	printf("CommonPlayer  cleanup \n");
} 


int CommonPlayer::PlayerRecvStopKey()
{
	int ret = Player_RcvJD_Msg(&msgtype, msgdata); 
	
	if(ret >=0 && (msgtype == DSP_KEY_STOP))
		return 1;
	else
		return 0;
}

void CommonPlayer::process_common_msg(int cmdtype, int *processed)
{
	switch (cmdtype)
	{
		case DSP_KEY_V_PLUS:
		case DSP_KEY_V_MINUS:			

			if((cmdtype == DSP_KEY_V_MINUS) && (volumeindex>MIN_VOLUME))
				volumeindex--;
			else	if((cmdtype == DSP_KEY_V_PLUS) && (volumeindex<MAX_VOLUME))
				volumeindex++;
			
			show_jd_audio_text(volumeindex);
			if(ifmute())
			{
				UI_JD_disappear(NULL);
				unsetmute();
			}
			DSP_TSPlayer_SetVolumeIndex(TSPlayerHandle,volumeindex);
		       save_volume_index(volumeindex);

			*processed  = 1;
			break;
			
		case DSP_KEY_MUTE:
			if(ifmute())
			{
				UI_JD_disappear(NULL);
				DSP_TSPlayer_SetVolumeIndex(TSPlayerHandle,volumeindex);
				unsetmute();
				break;
			}
			else
			{
				UI_JD_display(STATE_MUTE, 0, NULL);
				DSP_TSPlayer_SetVolumeIndex(TSPlayerHandle,MIN_VOLUME);
				setmute();
			}	
			
			*processed  = 1;
			break;

		default:
			break;
	}
		
	return;
}

void CommonPlayer::Set_Player_Window(player_window *pplayerwindow)
{
	DSPRect rect;
	
	rect.x = pplayerwindow->x*PLAYER_Full_Win_WIDTH/DFB_OSD_WIDTH;
	rect.y = pplayerwindow->y*PLAYER_Full_Win_HEGHT/DFB_OSD_HEIGHT;
	rect.width = pplayerwindow->width*PLAYER_Full_Win_WIDTH/DFB_OSD_WIDTH;
	rect.height = pplayerwindow->height*PLAYER_Full_Win_HEGHT/DFB_OSD_HEIGHT;

	//DSP_graphics_clearscreen(rect.x, rect.y, rect.width, rect.height);

	printf("@@@@@@@@@@@@rect x%d, y%d w%d h%d\n",rect.x, rect.y, rect.width, rect.height);
	//printf("tsplayerHandle is %d pplayerwindow x%d, y%d w%d h%d\n",TSPlayerHandle,pplayerwindow->x, pplayerwindow->y, pplayerwindow->width, pplayerwindow->height);

	DSP_TSPlayer_Setlocation(TSPlayerHandle, pplayerwindow->x, pplayerwindow->y, pplayerwindow->width, pplayerwindow->height);
	return;
}
void CommonPlayer::check_prebuffering_state(unsigned int buffersize)
{
	bool quit_prebuf = false;
	static long long start_time;
	
	if (player_state != DSP_CMD_PREBUFFERING)
		return;
	
	quit_prebuf = ((buffersize == 0) || (prebuf_level >= prebuf_max)) ? true : false;
	//quit_prebuf = ((buffersize == 0) || (get_ustime() - start_time > 400000)) ? TRUE : FALSE;

	prebuf_level += buffersize;
			
	if (quit_prebuf) 
	{
		printf("exit prebuffering state %ld\n", prebuf_level);
		printf("setting play state\n");
		player_state = DSP_CMD_PLAYING;
		DSP_TSPlayer_Play(TSPlayerHandle, DSPPlaymode_PlayFwd, 1);
	}
	
	return;
}
