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
static uint8_t  data_8b[DEF_UDP_BUF_SIZE*2];
static uint8_t  data_5b[(DEF_UDP_BUF_SIZE*2)+4];
static uint16_t ip_identifier = 0;
static uint32_t ip_chk_sum1, ip_chk_sum2, ip_chk_sum3;

// Etherent Frame
static const uint16_t  eth_type            = 0x0800; // IP

// IPv4 Header
static const uint8_t   ip_version          = 4;      // IP v4
static const uint8_t   ip_head_len         = 5;
static const uint8_t   ip_type_of_service  = 0;
static const uint16_t  ip_total_len        = 20 + DEF_UDP_LEN;


static void _make_crc_table(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (uint32_t j = 0; j < 8; j++) {
            c = c & 1 ? (c >> 1) ^ 0xEDB88320 : (c >> 1);
        }
        crc_table[i] = c;
    }
}


void udp_init(void) {
    _make_crc_table();
}


void udp_packet_gen(uint32_t *buf, uint8_t *udp_payload) {
    uint16_t udp_chksum = 0;
    uint32_t i, j, idx = 0, ans;

    // Calculate the ip check sum
    ip_chk_sum1 = 0x0000C512 + ip_identifier + ip_total_len + (DEF_IP_ADR_SRC1 << 8) + DEF_IP_ADR_SRC2 + (DEF_IP_ADR_SRC3 << 8) + DEF_IP_ADR_SRC4 +
                  (DEF_IP_DST_DST1 << 8) + DEF_IP_DST_DST2 + (DEF_IP_DST_DST3 << 8) + DEF_IP_DST_DST4;
    ip_chk_sum2 = (ip_chk_sum1 & 0x0000FFFF) + (ip_chk_sum1 >> 16);
    ip_chk_sum3 = ~((ip_chk_sum2 & 0x0000FFFF) + (ip_chk_sum2 >> 16));

    //////////////////////////////////////////////////////
    ip_identifier++;

    // Preamble
    for (i = 0; i < 7; i++) {
        data_8b[idx++] = 0x55;
    }
    // SFD
    data_8b[idx++] = 0xD5;
    // Destination MAC Address
    data_8b[idx++] = (DEF_ETH_DST_MAC >> 40) & 0xFF;
    data_8b[idx++] = (DEF_ETH_DST_MAC >> 32) & 0xFF;
    data_8b[idx++] = (DEF_ETH_DST_MAC >> 24) & 0xFF;
    data_8b[idx++] = (DEF_ETH_DST_MAC >> 16) & 0xFF;
    data_8b[idx++] = (DEF_ETH_DST_MAC >>  8) & 0xFF;
    data_8b[idx++] = (DEF_ETH_DST_MAC >>  0) & 0xFF;
    // Source MAC Address
    data_8b[idx++] = (DEF_ETH_SRC_MAC >> 40) & 0xFF;
    data_8b[idx++] = (DEF_ETH_SRC_MAC >> 32) & 0xFF;
    data_8b[idx++] = (DEF_ETH_SRC_MAC >> 24) & 0xFF;
    data_8b[idx++] = (DEF_ETH_SRC_MAC >> 16) & 0xFF;
    data_8b[idx++] = (DEF_ETH_SRC_MAC >>  8) & 0xFF;
    data_8b[idx++] = (DEF_ETH_SRC_MAC >>  0) & 0xFF;
    // Ethernet Type
    data_8b[idx++] = (eth_type >>  8) & 0xFF;
    data_8b[idx++] = (eth_type >>  0) & 0xFF;
    // IP Header
    data_8b[idx++] = (ip_version << 4) | (ip_head_len & 0x0F);
    data_8b[idx++] = (ip_type_of_service >>  0) & 0xFF;
    data_8b[idx++] = (ip_total_len >>  8) & 0xFF;
    data_8b[idx++] = (ip_total_len >>  0) & 0xFF;
    data_8b[idx++] = (ip_identifier >>  8) & 0xFF;
    data_8b[idx++] = (ip_identifier >>  0) & 0xFF;
    data_8b[idx++] = 0x00;
    data_8b[idx++] = 0x00;
    data_8b[idx++] = 0x80;
    data_8b[idx++] = 0x11;
    // IP Check SUM
    data_8b[idx++] = (ip_chk_sum3 >>  8) & 0xFF;
    data_8b[idx++] = (ip_chk_sum3 >>  0) & 0xFF;
    // IP Source
    data_8b[idx++] = DEF_IP_ADR_SRC1;
    data_8b[idx++] = DEF_IP_ADR_SRC2;
    data_8b[idx++] = DEF_IP_ADR_SRC3;
    data_8b[idx++] = DEF_IP_ADR_SRC4;
    // IP Destination
    data_8b[idx++] = DEF_IP_DST_DST1;
    data_8b[idx++] = DEF_IP_DST_DST2;
    data_8b[idx++] = DEF_IP_DST_DST3;
    data_8b[idx++] = DEF_IP_DST_DST4;
    // UDP header
    data_8b[idx++] = (DEF_UDP_SRC_PORTNUM >>  8) & 0xFF;
    data_8b[idx++] = (DEF_UDP_SRC_PORTNUM >>  0) & 0xFF;
    data_8b[idx++] = (DEF_UDP_DST_PORTNUM >>  8) & 0xFF;
    data_8b[idx++] = (DEF_UDP_DST_PORTNUM >>  0) & 0xFF;
    data_8b[idx++] = (DEF_UDP_LEN >>  8) & 0xFF;
    data_8b[idx++] = (DEF_UDP_LEN >>  0) & 0xFF;
    data_8b[idx++] = (udp_chksum >>  8) & 0xFF;
    data_8b[idx++] = (udp_chksum >>  0) & 0xFF;
    // UDP payload
    for (i = 0; i < DEF_UDP_PAYLOAD_SIZE; i++) {
        data_8b[idx++] = *(udp_payload + i);
    }

    ///////////////////////////////////////////////////
    // FCS Calc
    ///////////////////////////////////////////////////
    uint32_t crc = 0xffffffff;
    for (i = 8; i < idx; i++) {
        crc = (crc >> 8) ^ crc_table[(crc ^ data_8b[i]) & 0xFF];
    }
    crc ^= 0xffffffff;

    data_8b[idx++] = (crc >>  0) & 0xFF;
    data_8b[idx++] = (crc >>  8) & 0xFF;
    data_8b[idx++] = (crc >> 16) & 0xFF;
    data_8b[idx++] = (crc >> 24) & 0xFF;

    /////////////////////////////////////////////////
    // Encording 4b5b
    /////////////////////////////////////////////////
    data_5b[0]  = 0b11000;  // J
    data_5b[1]  = 0b10001;  // K
    for (i = 0, j = 2; i < idx; i++) {
        data_5b[j++] = tbl_4b5b[(data_8b[i] >> 0) & 0x0F];
        data_5b[j++] = tbl_4b5b[(data_8b[i] >> 4) & 0x0F];
    }
    data_5b[j++] = 0b01101; // T
    data_5b[j++] = 0b00111; // R
    data_5b[j++] = 0b11111; // IDLE
    data_5b[j++] = 0b11111; // IDLE

    /////////////////////////////////////////////////
    // NRZI Encoder
    /////////////////////////////////////////////////
    uint8_t ob;
    for (i = 0, j = 0, ob = 0; i < (DEF_UDP_BUF_SIZE*2)+4; i += 2) {
        ans  = tbl_nrzi[(ob << 5) + data_5b[i]];
        ans |= tbl_nrzi[((ans >> 4) << 5) + data_5b[i+1]] << 5;
        ob = ans >> 9;
        buf[j++] = ans;
    }
}

