

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/prctl.h>

#include "tlnettools.h"
#include "system_msg.h"
#include "dhcp_mng.h"

static int g_wireDhcpcStart = 0;
static pthread_t g_tid = 0;
static int g_nWireLeaseTime = 0;
static CBDhcpAddr DhcpCallback = NULL;

void dhcp_thread()
{
	FILE *fp = NULL;
	char buf[32] = {0};
	char csWireLeaseFile[32] = {0};
	pthread_detach(pthread_self());
	prctl(PR_SET_NAME, (unsigned long)"dhcpcdaemon", 0,0,0);

	sprintf(csWireLeaseFile, "/tmp/lease.%s", QMAPI_ETH_DEV);

	while (g_wireDhcpcStart)
	{
		if (g_nWireLeaseTime <= 0)
		{
			if(access(csWireLeaseFile, F_OK)==0)
			{
				fp = fopen(csWireLeaseFile, "r");
				if(fp)
				{
					fgets(buf, 32, fp);
					fclose(fp);
					g_nWireLeaseTime = atoi(buf);
					remove(csWireLeaseFile);

					// DHCP 成功之后，会修改IP地址
					if (DhcpCallback)
					{
						char IPAddr[16]={0}, IPMask[16]={0}, GatewayIpAddr[16]={0}, Dns1Addr[16]={0}, Dns2Addr[16]={0};

						get_ip_addr(QMAPI_ETH_DEV, IPAddr);
		                get_mask_addr(QMAPI_ETH_DEV, IPMask);
		                GetIPGateWay(QMAPI_ETH_DEV, GatewayIpAddr);
						get_dns_addr_ext(Dns1Addr, Dns2Addr);					
						DhcpCallback(IPAddr, IPMask, GatewayIpAddr, Dns1Addr, Dns2Addr);
					}
				}
			}
		}

		sleep(1);
		
		if (g_wireDhcpcStart == 0)
		{
		
		}
		else if(g_wireDhcpcStart)
			g_nWireLeaseTime--;
	}
}

int dhcpc_start(CBDhcpAddr Callback)
{
	if(g_wireDhcpcStart)
		return 0;

	char cmd[64] = {0};
	g_nWireLeaseTime = 0;
	g_wireDhcpcStart = 1;
	sprintf(cmd, "udhcpc -b -i %s -R", QMAPI_ETH_DEV); /* RM#1733: add -R (Release IP on quit), henry.li 2017/02/10 */
	SystemCall_msg(cmd, SYSTEM_MSG_NOBLOCK_RUN);

	if(g_tid == 0)
	{
		pthread_create(&g_tid, NULL, (void *)dhcp_thread, NULL);
		DhcpCallback = Callback;
	}

	return 0;
}

void dhcpc_stop()
{
	char cmd[128] = {0};
	if(g_wireDhcpcStart)
	{
		//sprintf(cmd, "ps -ef | grep udhcpc | grep %s | awk '{print $1}' | xargs kill -SIGKILL", QMAPI_ETH_DEV);
		sprintf(cmd, "%s", "killall -9 udhcpc"); /* RM#2770: kill all udhcpc,  henry.li  2017/03/17 */
		SystemCall_msg(cmd, SYSTEM_MSG_BLOCK_RUN);
		g_wireDhcpcStart = 0;
		g_tid = 0;
		g_nWireLeaseTime = 0;
		DhcpCallback = NULL;
	}
}


