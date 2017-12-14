
#ifndef _OSA_IPC_
#define _OSA_IPC_

#include <osa.h>

#define OSA_IPC_FLAG_OPEN      (0x0)
#define OSA_IPC_FLAG_CREATE    (0x1)

#define OSA_IPC_KEY_INVALID    ((Uint32)-1)

#define OSA_IPC_MBX_MSG_SIZE_MAX    (KB)

typedef struct {

  int id;
  char *ptr;

} OSA_IpcShmHndl;

typedef struct {

  int id;

} OSA_IpcMbxHndl;

typedef struct {

  int id;
  int members;
  int maxVal;

} OSA_IpcSemHndl;

typedef struct {

  long  type;
  Uint8 data[OSA_IPC_MBX_MSG_SIZE_MAX];

} OSA_IpcMsgHndl;

Uint32 OSA_ipcMakeKey(char *path, char id);

// shared memory
char *OSA_ipcShmOpen(OSA_IpcShmHndl *hndl, Uint32 key, Uint32 size, Uint32 flags);
int   OSA_ipcShmClose(OSA_IpcShmHndl *hndl);

// message box
int OSA_ipcMbxOpen(OSA_IpcMbxHndl *hndl, Uint32 key, Uint32 flags);
int OSA_ipcMbxSend(OSA_IpcMbxHndl *hndl, OSA_IpcMsgHndl *msgData, Uint32 msgSize);
int OSA_ipcMbxRecv(OSA_IpcMbxHndl *hndl, OSA_IpcMsgHndl *msgData, Uint32 msgSize);
int OSA_ipcMbxClose(OSA_IpcMbxHndl *hndl);

// semaphore
int OSA_ipcSemOpen(OSA_IpcSemHndl *hndl, Uint32 key, Uint32 members, Uint32 initVal, Uint32 maxVal, Uint32 flags);
int OSA_ipcSemLock(OSA_IpcSemHndl *hndl, Uint32 member, Uint32 timeout);
int OSA_ipcSemUnlock(OSA_IpcSemHndl *hndl, Uint32 member, Uint32 timeout);
int OSA_ipcSemGetVal(OSA_IpcSemHndl *hndl, Uint32 member);
int OSA_ipcSemClose(OSA_IpcSemHndl *hndl);

#endif  /*  _OSA_IPC_ */




