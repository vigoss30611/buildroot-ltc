#ifndef __ONVIF_PROC_H__
#define __ONVIF_PROC_H__

int Onvif_Init();

void Onvif_DeInit();

void Onvif_ResetLinkState();

void onvif_send_hello();
void onvif_send_bye(int fd);

int ONVIFSearchCreateSocket(int *pSocketType);
int OnvifSearchRequestProc(int dmsHandle, int sockfd, int socketype);

#endif
