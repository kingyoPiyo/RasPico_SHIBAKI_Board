/********************************************************
* Title    : RasPico SHIBAKI VBAN Sample
* Date     : 2022/07/25
* Note     : GP26 -- Audio L-ch
             GP27 -- Audio R-ch
* Design   : kingyo
********************************************************/
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hw.h"
#include "hwinit.h"
#include "vban.h"

static struct repeating_timer timer;

// Timer interrupt (L-tika)
static bool repeating_timer_callback(struct repeating_timer *t)
{
    static bool led0_state = true;

    gpio_put(HW_PINNUM_LED0, led0_state);
    led0_state = !led0_state;

    return true;
}


int main()
{
    stdio_init_all();
    hw_init();
    udp_init();
    vban_init();

    // L-tika ~~
    add_repeating_timer_ms(-500, repeating_timer_callback, NULL, &timer);
    
    while (true)
    {
        vban_main();
    }
}
