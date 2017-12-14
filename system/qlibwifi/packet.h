#ifndef _WIFI_PACKET_H_
#define _WIFI_PACKET_H_

int open_raw_socket(const char *ifname, uint8_t *hwaddr, int if_index);
int send_packet(int s, int if_index, struct dhcp_msg *msg, int size,
                uint32_t saddr, uint32_t daddr, uint32_t sport, uint32_t dport);
int receive_packet(int s, struct dhcp_msg *msg);

#endif
