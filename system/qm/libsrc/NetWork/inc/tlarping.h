/******************************************************************************
******************************************************************************/

#ifndef _ARPING_H
#define _ARPING_H

int start_arp(char *srcaddr, char *dstaddr, int count);

void initial_arping();
void initial_Wifi_arping();
#endif

