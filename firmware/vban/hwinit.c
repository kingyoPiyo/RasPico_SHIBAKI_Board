#include "hwinit.h"
#include "hw.h"
#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"

void hw_init()
{
    // Core voltage
    //vreg_set_voltage(VREG_VOLTAGE_1_10); // default, not work.
    //vreg_set_voltage(VREG_VOLTAGE_1_15); // not work.
    //vreg_set_voltage(VREG_VOLTAGE_1_20); // not work.
    //vreg_set_voltage(VREG_VOLTAGE_1_25); // good!
    //sleep_ms(10);

    // System clock
    set_sys_clock_khz(250000, true);    // 250MHz

    // SFP0
    gpio_init(HW_PINNUM_SFP0_RXD);
    gpio_set_dir(HW_PINNUM_SFP0_RXD, GPIO_IN);
    gpio_init(HW_PINNUM_SFP0_TXD);
    gpio_set_dir(HW_PINNUM_SFP0_TXD, GPIO_OUT);
    i2c_init(i2c0, 100000);
    gpio_set_function(HW_PINNUM_SFP0_SDA, GPIO_FUNC_I2C);
    gpio_set_function(HW_PINNUM_SFP0_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(HW_PINNUM_SFP0_SDA);
    gpio_pull_up(HW_PINNUM_SFP0_SCL);
    gpio_init(HW_PINNUM_SFP0_TXFLT);
    gpio_set_dir(HW_PINNUM_SFP0_TXFLT, GPIO_IN);
    gpio_pull_up(HW_PINNUM_SFP0_TXFLT);
    gpio_init(HW_PINNUM_SFP0_RXLOS);
    gpio_set_dir(HW_PINNUM_SFP0_RXLOS, GPIO_IN);
    gpio_pull_up(HW_PINNUM_SFP0_RXLOS);


    // SFP1
    gpio_init(HW_PINNUM_SFP1_RXD);
    gpio_set_dir(HW_PINNUM_SFP1_RXD, GPIO_IN);
    gpio_init(HW_PINNUM_SFP1_TXD);
    gpio_set_dir(HW_PINNUM_SFP1_TXD, GPIO_OUT);
    i2c_init(i2c1, 100000);
    gpio_set_function(HW_PINNUM_SFP1_SDA, GPIO_FUNC_I2C);
    gpio_set_function(HW_PINNUM_SFP1_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(HW_PINNUM_SFP1_SDA);
    gpio_pull_up(HW_PINNUM_SFP1_SCL);
    gpio_init(HW_PINNUM_SFP1_TXFLT);
    gpio_set_dir(HW_PINNUM_SFP1_TXFLT, GPIO_IN);
    gpio_pull_up(HW_PINNUM_SFP1_TXFLT);
    gpio_init(HW_PINNUM_SFP1_RXLOS);
    gpio_set_dir(HW_PINNUM_SFP1_RXLOS, GPIO_IN);
    gpio_pull_up(HW_PINNUM_SFP1_RXLOS);


    // SMA I/O
    gpio_init(HW_PINNUM_SMAIN0);
    gpio_set_dir(HW_PINNUM_SMAIN0, GPIO_IN);
    gpio_pull_down(HW_PINNUM_SMAIN0);

    gpio_init(HW_PINNUM_SMAIN1);
    gpio_set_dir(HW_PINNUM_SMAIN1, GPIO_IN);
    gpio_pull_down(HW_PINNUM_SMAIN1);

    gpio_init(HW_PINNUM_SMAOUT0);
    gpio_set_dir(HW_PINNUM_SMAOUT0, GPIO_OUT);

    gpio_init(HW_PINNUM_SMAOUT1);
    gpio_set_dir(HW_PINNUM_SMAOUT1, GPIO_OUT);


    // LEDs
    gpio_init(HW_PINNUM_LED0);
    gpio_set_dir(HW_PINNUM_LED0, GPIO_OUT);

    gpio_init(HW_PINNUM_LED1);
    gpio_set_dir(HW_PINNUM_LED1, GPIO_OUT);

    gpio_init(HW_PINNUM_LED4);
    gpio_set_dir(HW_PINNUM_LED4, GPIO_OUT);


    // ADC
    adc_init();
    adc_gpio_init(HW_PINNUM_ADC0);
    adc_gpio_init(HW_PINNUM_ADC1);


    // SW
    gpio_init(HW_PINNUM_SW0);
    gpio_set_dir(HW_PINNUM_SW0, GPIO_IN);
    gpio_pull_up(HW_PINNUM_SW0);

    gpio_init(HW_PINNUM_SW1);
    gpio_set_dir(HW_PINNUM_SW1, GPIO_IN);
    gpio_pull_up(HW_PINNUM_SW1);


    // GPIO
    gpio_init(HW_PINNUM_GPIO16);
    gpio_set_dir(HW_PINNUM_GPIO16, GPIO_IN);
    gpio_pull_down(HW_PINNUM_GPIO16);

    gpio_init(HW_PINNUM_GPIO17);
    gpio_set_dir(HW_PINNUM_GPIO17, GPIO_IN);
    gpio_pull_down(HW_PINNUM_GPIO17);

    gpio_init(HW_PINNUM_GPIO18);
    gpio_set_dir(HW_PINNUM_GPIO18, GPIO_IN);
    gpio_pull_down(HW_PINNUM_GPIO18);

    gpio_init(HW_PINNUM_GPIO19);
    gpio_set_dir(HW_PINNUM_GPIO19, GPIO_IN);
    gpio_pull_down(HW_PINNUM_GPIO19);

    // DCDC
    gpio_init(HW_PINNUM_DCDC_PS);
    gpio_set_dir(HW_PINNUM_DCDC_PS, GPIO_OUT);
    gpio_put(HW_PINNUM_DCDC_PS, true);              // true:PWM mode, false:PFM mode
                                                    // PWM is lower noise!
}
