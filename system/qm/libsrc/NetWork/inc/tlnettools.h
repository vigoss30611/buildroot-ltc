/******************************************************************************
******************************************************************************/

#ifndef _NET_TOOLS_H__
#define _NET_TOOLS_H__

int check_IP_Valid(const char* p);
int IsSubnetMask(char* pSubnet);
int check_gateway(char *strIp, char *strMask, char *strGateway);

int send_icmp_packet(char *addr);
int recv_icmp_packet();

int get_gateway_addr(char *gateway_addr);
int set_gateway_addr(char *gateway_addr);
int del_gateway_addr(char *gateway_addr);

char * safe_strncpy(char *dst, const char *src, size_t size);
int net_get_hwaddr(char *ifname, unsigned char *mac);
int get_ip_addr(char *name,char *net_ip);
int set_ip_addr(char *name,char *net_ip);
int get_brdcast_addr(char *name,char *net_brdaddr);

int set_mask_addr(char *name,char *mask_ip); /* name : eth0 eth1 lo and so on */
int get_mask_addr(char *name,char *net_mask);

int ifconfig_up_down(char *name, char *action);

int set_net_phyaddr(char *name,char *addr);  /* name : eth0 eth1 lo and so on */

int get_net_phyaddr(char *name, char *addr , char *bMac);

int get_dns_addr_wlan(char *address);
int get_dns_addr_ext_wlan(char *dns1, char *dns2);
int get_dns_addr(char *address);
int set_dns_addr(char *address);

int set_gateway_addr_if(char *gateway_addr,char *netif);

int refresh_gateway_if(char* netif,char *newif);

int get_gateway_netif(char * netif);

char* nslookup_raw(char* host_name);

int checkLink(char *sDev, char *csESSID);

int del_gateway_if(char * netif);
int del_route_netif(unsigned int dwAddr, unsigned int dwMask, char *netif);
int del_gateway_netif(char *netif);
int get_netlink_status(const char *netif);
int set_gateway_netif(char *gateway,char *netif);
int set_route_netif( unsigned int dwAddr, unsigned int dwMask, char *netif);

int SetNetWorkGateWay(unsigned int dwGateway, unsigned int dwAddr, unsigned int dwMask, char *netif);

int GetIPGateWay(const char *netif, char *gateway);
int get_dns_addr_ext(char *dns1, char *dns2);

unsigned long change_ip_string_value(const char *pIp);

#endif /* _NET_TOOLS_H__ */

