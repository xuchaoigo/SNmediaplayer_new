#ifndef __SHM_IPC_H__
#define __SHM_IPC_H__

#include <sys/ipc.h>

#include <sys/shm.h>
#include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>




#ifdef __cplusplus
extern "C"
{
#endif


#define DVBTEAM_SHM_KEYFILE  "/tmp/dsp_tre_shm_keyfile"
void Create_IPC_KeyFile(const char *key_file);
void  Remove_IPC_KeyFile(const char *key_file);
int GetShmAddr(unsigned char shm_index, unsigned shm_size, void **pShmAddr);
void DetachShm(const void *shm_addr);
void ReleaseShm(int shmid);

#ifdef __cplusplus
}
#endif


#endif
