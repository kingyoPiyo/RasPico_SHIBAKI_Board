#include "udp.h"

// 4B5B convert table
const static uint8_t tbl_4b5b[16] = {0b11110, 0b01001, 0b10100, 0b10101, 0b01010, 0b01011, 0b01110, 0b01111,
                                     0b10010, 0b10011, 0b10110, 0b10111, 0b11010, 0b11011, 0b11100, 0b11101};

// NRZI convert table
const static uint32_t tbl_nrzi[64] = {
         0, 16, 24,  8, 28, 12,  4, 20, 30, 14,  6, 22,  2, 18, 26, 10,
        31, 15,  7, 23,  3, 19, 27, 11,  1, 17, 25,  9, 29, 13,  5, 21,
        31, 15,  7, 23,  3, 19, 27, 11,  1, 17, 25,  9, 29, 13,  5, 21,
         0, 16, 24,  8, 28, 12,  4, 20, 30, 14,  6, 22,  2, 18, 26, 10
        };

static uint32_t crc_table[256];
static uint8_t  data_4b[DEF_UDP_BUF_SIZE*2];
static uint8_t  data_5b[(DEF_UDP_BUF_SIZE*2)+4];
static uint16_t ip_identifier;


static void _make_crc_table(void)
{
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (uint32_t j = 0; j < 8; j++) {
            c = c & 1 ? (c >> 1) ^ 0xEDB88320 : (c >> 1);
        }
        crc_table[i] = c;
    }
}


void udp_init(void)
{
    _make_crc_table();
}


void udp_packet_gen(uint32_t *buf, uint8_t *udp_payload)
{
    // Etherent Frame
    const uint16_t    eth_type            = 0x0800; // IP

    // UDP Header
          uint16_t    udp_chksum          = 0;

    // IPv4 Header
    const uint8_t     ip_version          = 4;      // IP v4
    const uint8_t     ip_head_len         = 5;
    const uint8_t     ip_type_of_service  = 0;
    const uint16_t    ip_total_len        = 20 + DEF_UDP_LEN;

    // Calculate the ip check sum
    const uint32_t    ip_chk_sum1 = 0x0000C512 + ip_identifier + ip_total_len + (DEF_IP_ADR_SRC1 << 8) + DEF_IP_ADR_SRC2 + (DEF_IP_ADR_SRC3 << 8) + DEF_IP_ADR_SRC4 +
                                    (DEF_IP_DST_DST1 << 8) + DEF_IP_DST_DST2 + (DEF_IP_DST_DST3 << 8) + DEF_IP_DST_DST4;
    const uint32_t    ip_chk_sum2 = (ip_chk_sum1 & 0x0000FFFF) + (ip_chk_sum1 >> 16);
    const uint32_t    ip_chk_sum3 = ~((ip_chk_sum2 & 0x0000FFFF) + (ip_chk_sum2 >> 16));

    //////////////////////////////////////////////////////
    uint8_t     ob;
    uint32_t    i, j, idx = 0, ans;

    ip_identifier++;

    // Preamble
    for (i = 0; i < 15; i++) {
        data_4b[idx++] = 0x05;
    }
    // SFD
    data_4b[idx++] = 0x0d;
    // Destination MAC Address
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 40) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 44) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 32) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 36) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 24) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 28) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 16) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 20) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >>  8) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >> 12) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >>  0) & 0x0F;
    data_4b[idx++] = (DEF_ETH_DST_MAC >>  4) & 0x0F;
    // Source MAC Address
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 40) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 44) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 32) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 36) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 24) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 28) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 16) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 20) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >>  8) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >> 12) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >>  0) & 0x0F;
    data_4b[idx++] = (DEF_ETH_SRC_MAC >>  4) & 0x0F;
    // Ethernet Type
    data_4b[idx++] = (eth_type >>  8) & 0x0F;
    data_4b[idx++] = (eth_type >> 12) & 0x0F;
    data_4b[idx++] = (eth_type >>  0) & 0x0F;
    data_4b[idx++] = (eth_type >>  4) & 0x0F;
    // IP Header
    data_4b[idx++] = ip_head_len;
    data_4b[idx++] = ip_version;
    data_4b[idx++] = (ip_type_of_service >>  0) & 0x0F;
    data_4b[idx++] = (ip_type_of_service >>  4) & 0x0F;
    data_4b[idx++] = (ip_total_len >>  8) & 0x0F;
    data_4b[idx++] = (ip_total_len >> 12) & 0x0F;
    data_4b[idx++] = (ip_total_len >>  0) & 0x0F;
    data_4b[idx++] = (ip_total_len >>  4) & 0x0F;
    data_4b[idx++] = (ip_identifier >>  8) & 0x0F;
    data_4b[idx++] = (ip_identifier >> 12) & 0x0F;
    data_4b[idx++] = (ip_identifier >>  0) & 0x0F;
    data_4b[idx++] = (ip_identifier >>  4) & 0x0F;
    data_4b[idx++] = 0;
    data_4b[idx++] = 0;
    data_4b[idx++] = 0;
    data_4b[idx++] = 0;
    data_4b[idx++] = 0;
    data_4b[idx++] = 8;
    data_4b[idx++] = 1;
    data_4b[idx++] = 1;
    // IP Check SUM
    data_4b[idx++] = (ip_chk_sum3 >>  8) & 0x0F;
    data_4b[idx++] = (ip_chk_sum3 >> 12) & 0x0F;
    data_4b[idx++] = (ip_chk_sum3 >>  0) & 0x0F;
    data_4b[idx++] = (ip_chk_sum3 >>  4) & 0x0F;
    // IP Source
    data_4b[idx++] = (DEF_IP_ADR_SRC1 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC1 >>  4) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC2 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC2 >>  4) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC3 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC3 >>  4) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC4 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_ADR_SRC4 >>  4) & 0x0F;
    // IP Destination
    data_4b[idx++] = (DEF_IP_DST_DST1 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST1 >>  4) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST2 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST2 >>  4) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST3 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST3 >>  4) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST4 >>  0) & 0x0F;
    data_4b[idx++] = (DEF_IP_DST_DST4 >>  4) & 0x0F;
    // UDP header
    data_4b[idx++] = (DEF_UDP_SRC_PORTNUM >>  8) & 0x0F;
    data_4b[idx++] = (DEF_UDP_SRC_PORTNUM >> 12) & 0x0F;
    data_4b[idx++] = (DEF_UDP_SRC_PORTNUM >>  0) & 0x0F;
    data_4b[idx++] = (DEF_UDP_SRC_PORTNUM >>  4) & 0x0F;
    data_4b[idx++] = (DEF_UDP_DST_PORTNUM >>  8) & 0x0F;
    data_4b[idx++] = (DEF_UDP_DST_PORTNUM >> 12) & 0x0F;
    data_4b[idx++] = (DEF_UDP_DST_PORTNUM >>  0) & 0x0F;
    data_4b[idx++] = (DEF_UDP_DST_PORTNUM >>  4) & 0x0F;
    data_4b[idx++] = (DEF_UDP_LEN >>  8) & 0x0F;
    data_4b[idx++] = (DEF_UDP_LEN >> 12) & 0x0F;
    data_4b[idx++] = (DEF_UDP_LEN >>  0) & 0x0F;
    data_4b[idx++] = (DEF_UDP_LEN >>  4) & 0x0F;
    data_4b[idx++] = (udp_chksum >>  8) & 0x0F;     // 後で計算
    data_4b[idx++] = (udp_chksum >> 12) & 0x0F;
    data_4b[idx++] = (udp_chksum >>  0) & 0x0F;
    data_4b[idx++] = (udp_chksum >>  4) & 0x0F;
    // UDP payload
    for (i = 0; i < DEF_UDP_PAYLOAD_SIZE; i++) {
        data_4b[idx++] = *(udp_payload + i) & 0x0F;
        data_4b[idx++] = (*(udp_payload + i) >> 4) & 0x0F;
    }


    ///////////////////////////////////////////////////
    // FCS Calc
    ///////////////////////////////////////////////////
    uint32_t crc = 0xffffffff;
    for (i = 16; i < idx; i+=2) {
        crc = (crc >> 8) ^ crc_table[(crc ^ ((data_4b[i+1] << 4) + data_4b[i])) & 0xFF];
    }
    crc ^= 0xffffffff;

    data_4b[idx++] = (crc >>  0) & 0xF;
    data_4b[idx++] = (crc >>  4) & 0xF;
    data_4b[idx++] = (crc >>  8) & 0xF;
    data_4b[idx++] = (crc >> 12) & 0xF;
    data_4b[idx++] = (crc >> 16) & 0xF;
    data_4b[idx++] = (crc >> 20) & 0xF;
    data_4b[idx++] = (crc >> 24) & 0xF;
    data_4b[idx++] = (crc >> 28) & 0xF;

    /////////////////////////////////////////////////
    // Encording 4b5b
    /////////////////////////////////////////////////
    data_5b[0]  = 0b11000;  // J
    data_5b[1]  = 0b10001;  // K
    for (i = 2; i < idx; i++) {
        data_5b[i] = tbl_4b5b[data_4b[i]];
    } 
    data_5b[i++] = 0b01101; // T
    data_5b[i++] = 0b00111; // R
    data_5b[i++] = 0b11111; // IDLE
    data_5b[i++] = 0b11111; // IDLE

    /////////////////////////////////////////////////
    // NRZI Encoder
    /////////////////////////////////////////////////
    for (i = 0, j = 0, ob = 0; i < (DEF_UDP_BUF_SIZE*2)+4; i += 2) {
        ans  = tbl_nrzi[(ob << 5) + data_5b[i]];
        ans |= tbl_nrzi[((ans >> 4) << 5) + data_5b[i+1]] << 5;
        ob = ans >> 9;
        buf[j++] = ans;
    }
}

