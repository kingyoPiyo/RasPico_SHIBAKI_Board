/********************************************************
* Title    : UDP Raw pulse TX test...
* Date     : 2023/12/28
* Note     : 
* Design   : kingyo
********************************************************/
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/dma.h"
#include "pico/multicore.h"
#include "hw.h"
#include "hwinit.h"
#include "ser_100base_fx.pio.h"
#include "udp.h"

static PIO pio_ser_wr = pio0;
static uint sm0 = 0;

int main()
{
    uint32_t DMA_SER_WR0;
    uint32_t tx_buf_udp[DEF_UDP_BUF_SIZE+1] = {0};
    uint8_t udp_payload[DEF_UDP_PAYLOAD_SIZE] = {0};
    uint32_t lp_cnt = 0;
    uint offset = 0;

    stdio_init_all();
    hw_init();
    udp_init();
    
    // PIO (Serializer) init
    offset = pio_add_program(pio_ser_wr, &ser_100base_fx_program);
    ser_100base_fx_program_init(pio_ser_wr, sm0, offset, HW_PINNUM_SFP0_TXD);

    // DMA channel setting(SFP0)
    DMA_SER_WR0 = dma_claim_unused_channel(true);
    dma_channel_config c0 = dma_channel_get_default_config(DMA_SER_WR0);
    channel_config_set_dreq(&c0, pio_get_dreq(pio_ser_wr, sm0, true));
    channel_config_set_transfer_data_size(&c0, DMA_SIZE_32);
    channel_config_set_read_increment(&c0, true);
    channel_config_set_write_increment(&c0, false);
    dma_channel_configure (
        DMA_SER_WR0,            // Channel to be configured
        &c0,                    // The configuration we just created
        &pio_ser_wr->txf[0],    // Destination address
        tx_buf_udp,             // Source address
        (DEF_UDP_BUF_SIZE+1),   // Number of transfers
        false                   // Don't start yet
    );
    
    static bool flg = 0;
    static uint8_t seq = 0;
    while (true)
    {
        if (flg)
            udp_payload[0] = 0xff;
        else
            udp_payload[0] = 0x00;
        flg = !flg;

        udp_payload[1] = seq;
        seq++;

        // if (!gpio_get(HW_PINNUM_SW0)) {
        //     udp_payload[0] |= 0x01;
        // }
        // gpio_put(HW_PINNUM_LED1, !gpio_get(HW_PINNUM_SW0));

        // if (!gpio_get(HW_PINNUM_SW1)) {
        //     udp_payload[0] |= 0x02;
        // }
        // gpio_put(HW_PINNUM_LED2, !gpio_get(HW_PINNUM_SW1));

        //sprintf(udp_payload, "Hello:%d", lp_cnt);
        udp_packet_gen(tx_buf_udp, udp_payload);

        gpio_put(HW_PINNUM_SMAOUT0, true);
        dma_channel_set_read_addr(DMA_SER_WR0, tx_buf_udp, true);
        dma_channel_wait_for_finish_blocking(DMA_SER_WR0);
        gpio_put(HW_PINNUM_SMAOUT0, false);

        // Loop wait
        sleep_us(1000);
    }
}
