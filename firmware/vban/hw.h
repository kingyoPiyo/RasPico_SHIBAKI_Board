#ifndef __HW_H__
#define __HW_H__

// SFP0
#define HW_PINNUM_SFP0_RXD      (10)
#define HW_PINNUM_SFP0_TXD      (11)
#define HW_PINNUM_SFP0_SCL      (13)
#define HW_PINNUM_SFP0_SDA      (12)
#define HW_PINNUM_SFP0_TXFLT    (14)
#define HW_PINNUM_SFP0_RXLOS    (15)

// SFP1
#define HW_PINNUM_SFP1_RXD      (4)
#define HW_PINNUM_SFP1_TXD      (5)
#define HW_PINNUM_SFP1_SCL      (7)
#define HW_PINNUM_SFP1_SDA      (6)
#define HW_PINNUM_SFP1_TXFLT    (9)
#define HW_PINNUM_SFP1_RXLOS    (8)

// SMA I/O
#define HW_PINNUM_SMAIN0        (3)
#define HW_PINNUM_SMAIN1        (2)
#define HW_PINNUM_SMAOUT0       (1)
#define HW_PINNUM_SMAOUT1       (0)

// LEDs
#define HW_PINNUM_LED0          (25)    // Pico onboard
#define HW_PINNUM_LED1          (22)
// #define HW_PINNUM_LED2          (26)
// #define HW_PINNUM_LED3          (27)
#define HW_PINNUM_ADC0          (26)
#define HW_PINNUM_ADC1          (27)
#define HW_PINNUM_LED4          (28)

// SW
#define HW_PINNUM_SW0           (20)
#define HW_PINNUM_SW1           (21)

// GPIO
#define HW_PINNUM_GPIO16        (16)
#define HW_PINNUM_GPIO17        (17)
#define HW_PINNUM_GPIO18        (18)
#define HW_PINNUM_GPIO19        (19)

// DCDC
#define HW_PINNUM_DCDC_PS       (23)

#endif //__HW_H__
