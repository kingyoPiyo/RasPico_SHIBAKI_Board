/********************************************************
* Title    : RasPico SHIBAKI H/W Test
* Date     : 2022/07/03
* Note     : MATOMONI UGOKAN ZOI
* Design   : kingyo
********************************************************/
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/dma.h"
#include "pico/multicore.h"
#include "hw.h"
#include "hwinit.h"
#include "ser_100base_fx.pio.h"
#include "ser_10b.pio.h"
#include "tbl_8b10b.h"
#include "udp.h"
#include "hwtest.h"
#include "rx_func.h"

static PIO pio_ser_wr = pio0;
static uint sm0 = 0;
static uint sm1 = 1;

// 8b10b TX Interrupt (1us period)
static void __time_critical_func(ser_10b_irq)(void)
{
    static uint8_t cnt_10b = 0;     // tx Byte counter
    uint32_t tx_buf_8b10b;          // tx 10bit data
    uint8_t tx_8b;                  // tx 8bit data

    gpio_put(HW_PINNUM_SMAOUT1, true);

    tx_8b = gpio_get(HW_PINNUM_SW1) ? 0x00 : 0x01;
    gpio_put(HW_PINNUM_LED2, tx_8b & 0x01);
    if (++cnt_10b == 255) {
        // K28.5
        tx_buf_8b10b = tbl_8b10b_enc(0x00, true);
    } else {
        tx_buf_8b10b = tbl_8b10b_enc(tx_8b, false);
    }
    ser_10b_tx(pio_ser_wr, sm1, tx_buf_8b10b << 22);

    gpio_put(HW_PINNUM_SMAOUT1, false);
}

int main()
{
    uint32_t DMA_SER_WR0;
    uint32_t tx_buf_udp[DEF_UDP_BUF_SIZE+1] = {0};
    uint8_t udp_payload[DEF_UDP_PAYLOAD_SIZE] = {0};
    uint32_t lp_cnt = 0;
    uint offset = 0;

    stdio_init_all();
    hw_init();
    hw_test();

    
    // PIO (Serializer) init
    offset = pio_add_program(pio_ser_wr, &ser_100base_fx_program);
    ser_100base_fx_program_init(pio_ser_wr, sm0, offset, HW_PINNUM_SFP0_TXD);       // for 100BASE-FX 100Mbps
    offset = pio_add_program(pio_ser_wr, &ser_10b_program);
    ser_10b_program_init(pio_ser_wr, sm1, offset, HW_PINNUM_SFP1_TXD, 10000000);    // for 8b10b 10Mbps

    // 8b10b FIFO Empty IRQ, IRQ No = 7(PIO0_IRQ_0)
    pio_set_irq0_source_enabled(pio_ser_wr, PIO_INTR_SM1_TXNFULL_LSB, true);
    irq_set_exclusive_handler(PIO0_IRQ_0, &ser_10b_irq);
    //irq_add_shared_handler(PIO0_IRQ_0, &ser_10b_irq, 255);
    irq_set_enabled(PIO0_IRQ_0, true);

    // RX Task
    //  MADA UGOKANE YO ~~~~~~~~~
    //multicore_launch_core1(rx_main);

    // UDP
    udp_init();
    sprintf(udp_payload, "Hello RasPico SHIBAKI Board !!");
    udp_packet_gen(tx_buf_udp, udp_payload);

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
    
    while (true)
    {
        // Send data only while button is pressed.
        if (!gpio_get(HW_PINNUM_SW0)) {
            gpio_put(HW_PINNUM_SMAOUT0, true);

            gpio_put(HW_PINNUM_LED1, true);

            // UDP Packet update
            lp_cnt++;
            sprintf(udp_payload, "Hello RasPico SHIBAKI Board !! lp_cnt:%d", lp_cnt);
            udp_packet_gen(tx_buf_udp, udp_payload);

            // DMA Start
            dma_channel_set_read_addr(DMA_SER_WR0, tx_buf_udp, true);

            // Wait for DMA
            dma_channel_wait_for_finish_blocking(DMA_SER_WR0);

            gpio_put(HW_PINNUM_SMAOUT0, false);
        } else {
            gpio_put(HW_PINNUM_LED1, false);
        }

        // Loop wait
        sleep_us(1000);
    }
}
