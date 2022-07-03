#ifndef __UDP_H__
#define __UDP_H__

#include <stdint.h>

#define DEF_TX_BUF_SIZE     (1024)

void udp_init(void);
void udp_packet_init(uint32_t *buf, uint32_t in_data);
void udp_payload_update(uint32_t *buf, uint32_t in_data);

#endif //__UDP_H__
