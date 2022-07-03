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

// HW test
void hw_test(void)
{
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

    // UART Message
    printf("## RasPico SHIBAKI Board  Demo firmware ##\r\n");
    printf("\r\n");

    // SFP Status
    printf("#### SFP Status ####\r\n");
    printf("SFP0\r\n");
    gpio_get(HW_PINNUM_SFP0_TXFLT)  ? printf(" TXFLT   : High\r\n") : printf(" TXFLT   : Low\r\n");
    gpio_get(HW_PINNUM_SFP0_RXLOS)  ? printf(" RXLOS   : High\r\n") : printf(" RXLOS   : Low\r\n");
    gpio_get(HW_PINNUM_SFP0_SCL)    ? printf(" SCL     : High\r\n") : printf(" SCL     : Low\r\n");
    gpio_get(HW_PINNUM_SFP0_SDA)    ? printf(" SDA     : High\r\n") : printf(" SDA     : Low\r\n");
    printf("SFP1\r\n");
    gpio_get(HW_PINNUM_SFP1_TXFLT)  ? printf(" TXFLT   : High\r\n") : printf(" TXFLT   : Low\r\n");
    gpio_get(HW_PINNUM_SFP1_RXLOS)  ? printf(" RXLOS   : High\r\n") : printf(" RXLOS   : Low\r\n");
    gpio_get(HW_PINNUM_SFP1_SCL)    ? printf(" SCL     : High\r\n") : printf(" SCL     : Low\r\n");
    gpio_get(HW_PINNUM_SFP1_SDA)    ? printf(" SDA     : High\r\n") : printf(" SDA     : Low\r\n");
    // SMA Input Status
    printf("#### SMA Input ####\r\n");
    gpio_get(HW_PINNUM_SMAIN0)      ? printf(" SMA_IN0 : High\r\n") : printf(" SMA_IN0 : Low\r\n");
    gpio_get(HW_PINNUM_SMAIN1)      ? printf(" SMA_IN1 : High\r\n") : printf(" SMA_IN1 : Low\r\n");
    // User SW Status
    printf("#### SW Level ####\r\n");
    gpio_get(HW_PINNUM_SW0)         ? printf(" SW0     : High\r\n") : printf(" SW0     : Low\r\n");
    gpio_get(HW_PINNUM_SW1)         ? printf(" SW1     : High\r\n") : printf(" SW1     : Low\r\n");
    // GPIO Level
    printf("#### GPIO Level ####\r\n");
    gpio_get(HW_PINNUM_GPIO16)      ? printf(" GPIO16  : High\r\n") : printf(" GPIO16  : Low\r\n");
    gpio_get(HW_PINNUM_GPIO17)      ? printf(" GPIO17  : High\r\n") : printf(" GPIO17  : Low\r\n");
    gpio_get(HW_PINNUM_GPIO18)      ? printf(" GPIO18  : High\r\n") : printf(" GPIO18  : Low\r\n");
    gpio_get(HW_PINNUM_GPIO19)      ? printf(" GPIO19  : High\r\n") : printf(" GPIO19  : Low\r\n");

    // Timer interrupt
    add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer);

    sleep_ms(1000);
}
