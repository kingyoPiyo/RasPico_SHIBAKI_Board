/********************************************************
* Title    : VBAN.c
* Date     : 2022/07/25
* Design   : kingyo
* Note     : ADC0 -- Audio L-Ch
             ADC1 -- Audio R-Ch
********************************************************/
#include "vban.h"
#include "udp.h"
#include "hw.h"
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "hardware/adc.h"
#include "ser_100base_fx.pio.h"

#define VBAN_X4_OVERSAMPL_ON    // x4 Oversampling

#define VBAN_SR_MAXNUMBER           21
#define VBAN_PROTOCOL_AUDIO         0x00
#define VBAN_PROTOCOL_SERIAL        0x20
#define VBAN_PROTOCOL_TXT           0x40
#define VBAN_PROTOCOL_SERVICE       0x60
#define VBAN_PROTOCOL_UNDEFINED_1   0x80
#define VBAN_PROTOCOL_UNDEFINED_2   0xA0
#define VBAN_PROTOCOL_UNDEFINED_3   0xC0
#define VBAN_PROTOCOL_USER          0xE0

#define VBAN_DATATYPE_BYTE8         0x00
#define VBAN_DATATYPE_INT16         0x01
#define VBAN_DATATYPE_INT24         0x02
#define VBAN_DATATYPE_INT32         0x03
#define VBAN_DATATYPE_FLOAT32       0x04
#define VBAN_DATATYPE_FLOAT64       0x05
#define VBAN_DATATYPE_12BITS        0x06
#define VBAN_DATATYPE_10BITS        0x07

#define VBAN_CODEC_PCM              0x00
#define VBAN_CODEC_VBCA             0x10
#define VBAN_CODEC_VBCV             0x20
#define VBAN_CODEC_UNDEFINED_1      0x30
#define VBAN_CODEC_UNDEFINED_2      0x40
#define VBAN_CODEC_UNDEFINED_3      0x50
#define VBAN_CODEC_UNDEFINED_4      0x60
#define VBAN_CODEC_UNDEFINED_5      0x70
#define VBAN_CODEC_UNDEFINED_6      0x80
#define VBAN_CODEC_UNDEFINED_7      0x90
#define VBAN_CODEC_UNDEFINED_8      0xA0
#define VBAN_CODEC_UNDEFINED_9      0xB0
#define VBAN_CODEC_UNDEFINED_10     0xC0
#define VBAN_CODEC_UNDEFINED_11     0xD0
#define VBAN_CODEC_UNDEFINED_12     0xE0
#define VBAN_CODEC_USER             0xF0


static char stream_name[16] = "Stream1";
static uint32_t tx_buf_udp[DEF_UDP_BUF_SIZE+1] = {0};
static uint8_t udp_payload[DEF_UDP_PAYLOAD_SIZE] = {0};

static uint32_t DMA_SER_WR0;

static uint8_t sel = 0;
static uint32_t flg = 0;


typedef struct {
    T_VBAN_HEADER header;
    int16_t pcm[DEF_VBAN_PCM_SIZE/2];
} vban_payload_t;

static vban_payload_t vban_payload;

// ADC Buffer
static int16_t adc_buf[2][DEF_VBAN_PCM_SIZE/2];


// ADC Conversion Interrupt
static void __time_critical_func(adc_irq_handler) (void)
{
    static uint8_t lp = 0;
    static uint8_t os = 0;
    static int16_t l_tmp = 0;
    static int16_t r_tmp = 0;

    gpio_put(HW_PINNUM_SMAOUT0, true);

#ifdef VBAN_X4_OVERSAMPL_ON
    // x4 oversampling
    switch (os) {
        case 0:
            l_tmp = adc_fifo_get(); os++; break;
        case 1:
            r_tmp = adc_fifo_get(); os++; break;
        case 2:
            l_tmp += adc_fifo_get(); os++; break;
        case 3:
            r_tmp += adc_fifo_get(); os++; break;
        case 4:
            l_tmp += adc_fifo_get(); os++; break;
        case 5:
            r_tmp += adc_fifo_get(); os++; break;
        case 6:
            l_tmp += adc_fifo_get(); os++; break;
        case 7:
            r_tmp += adc_fifo_get(); os = 0;
            adc_buf[sel][lp++] = (l_tmp - 8192) * 2;
            adc_buf[sel][lp++] = (r_tmp - 8192) * 2;
            if (lp == 0) {
                sel = 1 - sel;
                flg = 1;
            }
            break;
    }
#else
    adc_buf[sel][lp++] = (adc_fifo_get() - 2048) * 8;
    if (lp == 0) {
        sel = 1 - sel;
        flg = 1;
    }
#endif

    gpio_put(HW_PINNUM_SMAOUT0, false);
}


void vban_init(void)
{
    uint offset = 0;
    PIO pio_ser_wr = pio0;
    uint sm0 = 0;

    udp_init();

    // VBAN Header
    vban_payload.header.vban = ('N' << 24) + ('A' << 16) + ('B' << 8) + ('V' << 0);
    vban_payload.header.format_SR = 16;      // 44.1kHz
    vban_payload.header.format_nbs = 128-1;  // 128 Samples/frame
    vban_payload.header.format_nbc = 2-1;    // 2CH (Stereo)
    vban_payload.header.format_bit = VBAN_DATATYPE_INT16;   // Bit resolution (16bit)
    for (uint8_t i = 0; i < 16; i++) {
        vban_payload.header.streamname[i] = stream_name[i];
    }
    vban_payload.header.nuFrame = 0;

    // 100BASE-FX Serializer PIO init
    offset = pio_add_program(pio_ser_wr, &ser_100base_fx_program);
    ser_100base_fx_program_init(pio_ser_wr, sm0, offset, HW_PINNUM_SFP0_TXD);

    // DMA channel setting (SFP0)
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

    // ADC Settings
    adc_select_input(0);                        // Start is ADC0(L-Ch)
#ifdef VBAN_X4_OVERSAMPL_ON
    adc_set_clkdiv((48000000.0/(44100*8))-1.0); // 44.1kHz x 4 * 2CH = 352.8kS/s Round-Robin
#else
    adc_set_clkdiv((48000000.0/(44100*2))-1.0); // 44.1kHz * 2CH = 88.2kS/s Round-Robin
#endif
    adc_set_round_robin(0b00000011);            // ADC0 & ADC1

    // Set IRQ handler
    irq_add_shared_handler(ADC_IRQ_FIFO, &adc_irq_handler, 128);
    irq_set_enabled(ADC_IRQ_FIFO, true);

    // ADC FIFO
    adc_fifo_setup	(
        true,       // Enables write each conversion result to the FIFO
        false,      // Enable DMA requests when FIFO contains data
        1,          // Threshold for DMA requests/FIFO IRQ if enabled.
        false,      // If enabled, bit 15 of the FIFO contains error flag for each sample
        false       // Shift FIFO contents to be one byte in size (for byte DMA) - enables DMA to byte buffers.
    );

    adc_irq_set_enabled(true);
    irq_set_priority(ADC_IRQ_FIFO, 0);
    sleep_ms(1);    // A nap will help you concentrate.
    adc_run(true);  // ADC Free running start!
}

void vban_main(void)
{
    if (flg) {
        flg = 0;

        gpio_put(HW_PINNUM_SMAOUT1, true);

        // Increment VBAN Frame counter
        vban_payload.header.nuFrame++;

        // Copy PCM data
        for (uint32_t i = 0; i < (DEF_VBAN_PCM_SIZE/2); i++) {
            vban_payload.pcm[i] = adc_buf[1-sel][i];
        }
        udp_packet_gen(tx_buf_udp, (uint8_t *)&vban_payload);

        // DMA Start (100BASE-FX)
        dma_channel_set_read_addr(DMA_SER_WR0, tx_buf_udp, true);

        gpio_put(HW_PINNUM_SMAOUT1, false);
    }
}
