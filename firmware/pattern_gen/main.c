/********************************************************
* Title    : RasPico SHIBAKI Pattern Generator
* Date     : 2023/12/28
* Note     : 位相同期の取れたテストパターンを出力する
* Design   : kingyo
********************************************************/
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hw.h"
#include "hwinit.h"
#include "ser_tx.pio.h"

#define DEF_SER_NUM         (2)

// 1Word = 32bit のバッファサイズを指定する
// このバッファの内容をPIOのクロックで途切れなく出力する
// e.g.
//  バッファサイズ = 1000, clk = 125MHz の場合、
//  1000 * 32bit * 8ns = 256us
//  で1Loopとなる
#define DEF_BUF_SIZE        (1000)

int main()
{
    static PIO pio_ser_wr = pio0;
    uint32_t ser_buf[DEF_SER_NUM][DEF_BUF_SIZE] = {0};
    int dma_ch[DEF_SER_NUM];

    stdio_init_all();
    hw_init();
    
    // PIO init
    uint offset = pio_add_program(pio_ser_wr, &ser_tx_program);
    ser_tx_program_init(pio_ser_wr, 0, offset, HW_PINNUM_SFP0_TXD);     // SFP0
    ser_tx_program_init(pio_ser_wr, 1, offset, HW_PINNUM_SMAOUT0);      // SMA0

    // DMA channel setting
    for (int i = 0; i < DEF_SER_NUM; i++) {
        dma_ch[i] = dma_claim_unused_channel(true);
        dma_channel_config c0 = dma_channel_get_default_config(dma_ch[i]);
        channel_config_set_dreq(&c0, pio_get_dreq(pio_ser_wr, i, true));
        channel_config_set_transfer_data_size(&c0, DMA_SIZE_32);
        channel_config_set_read_increment(&c0, true);
        channel_config_set_write_increment(&c0, false);

        dma_channel_configure (
            dma_ch[i],              // Channel to be configured
            &c0,                    // The configuration we just created
            &pio_ser_wr->txf[i],    // Destination address
            ser_buf[i],             // Source address
            DEF_BUF_SIZE,           // Number of transfers
            false                   // Don't start yet
        );
    }


    // パターンデータの生成
    // データは右シフト（LSBから送信）で送出
    for (int i = 0; i < DEF_BUF_SIZE; i++) {
        ser_buf[0][i] = 0x55555555;
    }
    ser_buf[0][DEF_BUF_SIZE-1] = 0x0F555555;

    for (int i = 0; i < DEF_BUF_SIZE; i++) {
        ser_buf[1][i] = 0x00000000;
    }
    ser_buf[1][DEF_BUF_SIZE-1] = 0x0F000000;

    
    // 初回の転送開始位相を揃えるための時間稼ぎとして、
    // TX_FIFO 8Word分をゼロ埋めしておく
    for (int i = 0; i < 8; i++) {
        for (int sm = 0; sm < DEF_SER_NUM; sm++) {
            pio_sm_put_blocking(pio_ser_wr, sm, 0);
        }
    }


    // SMを同期して起動
    pio_enable_sm_mask_in_sync(pio_ser_wr, 0b11);

    // 動作表示
    gpio_put(HW_PINNUM_LED0, true);

    // ser_bufを途切れなく連続転送する
    while (true) {
        // DMA Readアドレス再設定
        dma_channel_set_read_addr(dma_ch[0], ser_buf[0], true);
        dma_channel_set_read_addr(dma_ch[1], ser_buf[1], true);
        
        // DMA転送完了待ち
        dma_channel_wait_for_finish_blocking(dma_ch[0]);
        dma_channel_wait_for_finish_blocking(dma_ch[1]);
    }
}
