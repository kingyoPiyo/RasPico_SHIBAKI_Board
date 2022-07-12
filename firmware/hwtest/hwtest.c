#include "hwtest.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include "hw.h"

static struct repeating_timer timer;

// Timer interrupt (L-tika)
static bool repeating_timer_callback(struct repeating_timer *t)
{
    static bool led3_state = true;

    gpio_put(HW_PINNUM_LED3, led3_state);
    led3_state = !led3_state;

    // true to continue repeating, false to stop.
    return true;
}

static void uart_debug_output(void)
{
    uint32_t io_status_now;
    static uint32_t io_status_old = 0xFFFFFFFF;

    // I/O sampling
    io_status_now = 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP0_TXFLT) ? 1 << 0 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP0_RXLOS) ? 1 << 1 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP0_SCL)   ? 1 << 2 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP0_SDA)   ? 1 << 3 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP1_TXFLT) ? 1 << 4 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP1_RXLOS) ? 1 << 5 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP1_SCL)   ? 1 << 6 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SFP1_SDA)   ? 1 << 7 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SMAIN0)     ? 1 << 8 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SMAIN1)     ? 1 << 9 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SW0)        ? 1 << 10 : 0;
    io_status_now |= gpio_get(HW_PINNUM_SW1)        ? 1 << 11 : 0;
    io_status_now |= gpio_get(HW_PINNUM_GPIO16)     ? 1 << 12 : 0;
    io_status_now |= gpio_get(HW_PINNUM_GPIO17)     ? 1 << 13 : 0;
    io_status_now |= gpio_get(HW_PINNUM_GPIO18)     ? 1 << 14 : 0;
    io_status_now |= gpio_get(HW_PINNUM_GPIO19)     ? 1 << 15 : 0;

    if (io_status_now != io_status_old)
    {
        printf("\033[H\033[2J");
        printf("## RasPico SHIBAKI Board  Demo firmware ##\r\n");
        printf("\r\n");

        // SFP Status
        printf("#### SFP Status ####\r\n");
        printf("SFP0\r\n");
        io_status_now & (1 << 0)    ? printf(" TXFLT   : High\r\n") : printf(" TXFLT   : Low\r\n");
        io_status_now & (1 << 1)    ? printf(" RXLOS   : High\r\n") : printf(" RXLOS   : Low\r\n");
        io_status_now & (1 << 2)    ? printf(" SCL     : High\r\n") : printf(" SCL     : Low\r\n");
        io_status_now & (1 << 3)    ? printf(" SDA     : High\r\n") : printf(" SDA     : Low\r\n");
        printf("SFP1\r\n");
        io_status_now & (1 << 4)    ? printf(" TXFLT   : High\r\n") : printf(" TXFLT   : Low\r\n");
        io_status_now & (1 << 5)    ? printf(" RXLOS   : High\r\n") : printf(" RXLOS   : Low\r\n");
        io_status_now & (1 << 6)    ? printf(" SCL     : High\r\n") : printf(" SCL     : Low\r\n");
        io_status_now & (1 << 7)    ? printf(" SDA     : High\r\n") : printf(" SDA     : Low\r\n");
        // SMA Input Status
        printf("#### SMA Input ####\r\n");
        io_status_now & (1 << 8)    ? printf(" SMA_IN0 : High\r\n") : printf(" SMA_IN0 : Low\r\n");
        io_status_now & (1 << 9)    ? printf(" SMA_IN1 : High\r\n") : printf(" SMA_IN1 : Low\r\n");
        // User SW Status
        printf("#### SW Level ####\r\n");
        io_status_now & (1 << 10)   ? printf(" SW0     : High\r\n") : printf(" SW0     : Low\r\n");
        io_status_now & (1 << 11)   ? printf(" SW1     : High\r\n") : printf(" SW1     : Low\r\n");
        // GPIO Level
        printf("#### GPIO Level ####\r\n");
        io_status_now & (1 << 12)   ? printf(" GPIO16  : High\r\n") : printf(" GPIO16  : Low\r\n");
        io_status_now & (1 << 13)   ? printf(" GPIO17  : High\r\n") : printf(" GPIO17  : Low\r\n");
        io_status_now & (1 << 14)   ? printf(" GPIO18  : High\r\n") : printf(" GPIO18  : Low\r\n");
        io_status_now & (1 << 15)   ? printf(" GPIO19  : High\r\n") : printf(" GPIO19  : Low\r\n");

        io_status_old = io_status_now;
        sleep_ms(20);
    }
}

// HW test
void hw_test(void)
{
    bool uart_loop_forever = false;

    // If SW0 pushed, uart debug message forever..
    if (!gpio_get(HW_PINNUM_SW0)) {
        uart_loop_forever = true;
    } else {
        uart_loop_forever = false;
    }

    // ALL LED ON
    gpio_put(HW_PINNUM_LED0, true);
    gpio_put(HW_PINNUM_LED1, true);
    gpio_put(HW_PINNUM_LED2, true);
    gpio_put(HW_PINNUM_LED3, true);
    sleep_ms(1000);
    // ALL LED OFF
    gpio_put(HW_PINNUM_LED3, false);
    sleep_ms(150);
    gpio_put(HW_PINNUM_LED2, false);
    sleep_ms(150);
    gpio_put(HW_PINNUM_LED1, false);
    sleep_ms(150);
    gpio_put(HW_PINNUM_LED0, false);

    do {
        uart_debug_output();
    } while (uart_loop_forever);

    // Timer interrupt
    add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer);

    sleep_ms(1000);
}
