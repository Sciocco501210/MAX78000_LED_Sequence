#include <stdio.h>
#include <mxc_device.h>
#include <gpio.h>
#include <board.h>
#include <mxc_delay.h>

// Define the GPIO pin masks for each LED
#define LED1_PIN  (MXC_GPIO_PIN_17)  // P0_17
#define LED2_PIN  (MXC_GPIO_PIN_16)  // P0_16
#define LED3_PIN  (MXC_GPIO_PIN_1)   // P3_1
#define LED4_PIN  (MXC_GPIO_PIN_19)  // P0_19
#define LED5_PIN  (MXC_GPIO_PIN_11)  // P0_11
#define LED6_PIN  (MXC_GPIO_PIN_8)   // P0_8
#define LED7_PIN  (MXC_GPIO_PIN_9)   // P0_9
#define LED8_PIN  (MXC_GPIO_PIN_6)   // P1_6

#define BTN_PIN  (MXC_GPIO_PIN_6)

bool isWave = true;
uint8_t pattern = 0b1;


void gpio_isr(void *cbdata)
{
    // mxc_gpio_cfg_t *cfg = cbdata;
    // MXC_GPIO_OutToggle(cfg->port, cfg->mask);
    isWave = !isWave;
    pattern = 1;
}

void setup_gpio(void) {
    // GPIO configuration structure
    mxc_gpio_cfg_t gpio_cfg = {0};

    // Configure GPIO0 pins (P0_17, P0_16, P0_19, P0_11, P0_8, P0_9)
    gpio_cfg.port = MXC_GPIO0;
    gpio_cfg.mask = (LED1_PIN | LED2_PIN | LED4_PIN | LED5_PIN | LED6_PIN | LED7_PIN);
    gpio_cfg.pad = MXC_GPIO_PAD_NONE;
    gpio_cfg.func = MXC_GPIO_FUNC_OUT;
    MXC_GPIO_Config(&gpio_cfg);

    // Configure GPIO3 pin (P3_1)
    gpio_cfg.port = MXC_GPIO3;
    gpio_cfg.mask = LED3_PIN;
    MXC_GPIO_Config(&gpio_cfg);

    // Configure GPIO1 pin (P1_6)
    gpio_cfg.port = MXC_GPIO1;
    gpio_cfg.mask = LED8_PIN;
    MXC_GPIO_Config(&gpio_cfg);

    // configue BTN (P0_2)
    gpio_cfg.port = MXC_GPIO0;
    gpio_cfg.mask = BTN_PIN;
    gpio_cfg.pad = MXC_GPIO_PAD_PULL_UP;
    gpio_cfg.func = MXC_GPIO_FUNC_IN;
    gpio_cfg.vssel = MXC_GPIO_VSSEL_VDDIOH;
    gpio_cfg.drvstr = MXC_GPIO_DRVSTR_0;
    MXC_GPIO_Config(&gpio_cfg);
    MXC_GPIO_RegisterCallback(&gpio_cfg, gpio_isr, &gpio_cfg);
    MXC_GPIO_IntConfig(&gpio_cfg, MXC_GPIO_INT_FALLING);
    MXC_GPIO_EnableInt(gpio_cfg.port, gpio_cfg.mask);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(MXC_GPIO_GET_IDX(MXC_GPIO0)));
}

void clear_leds(void) {
    // Turn off all LEDs using MXC_GPIO_OutClr
    MXC_GPIO_OutClr(MXC_GPIO0, LED1_PIN | LED2_PIN | LED4_PIN | LED5_PIN | LED6_PIN | LED7_PIN);
    MXC_GPIO_OutClr(MXC_GPIO3, LED3_PIN);
    MXC_GPIO_OutClr(MXC_GPIO1, LED8_PIN);
}

void setPin(uint8_t bit, mxc_gpio_regs_t **port, int *pin) {
    switch (bit)
    {
    case 0:
        *port = MXC_GPIO0;
        *pin = LED1_PIN;
        break;
    case 1:
        *port = MXC_GPIO0;
        *pin = LED2_PIN;
        break;
    case 2:
        *port = MXC_GPIO3;
        *pin = LED3_PIN;
        break;
    case 3:
        *port = MXC_GPIO0;
        *pin = LED4_PIN;
        break;
    case 4:
        *port = MXC_GPIO0;
        *pin = LED5_PIN;
        break;
    case 5:
        *port = MXC_GPIO0;
        *pin = LED6_PIN;
        break;
    case 6:
        *port = MXC_GPIO0;
        *pin = LED7_PIN;
        break;
    case 7:
        *port = MXC_GPIO1;
        *pin = LED8_PIN;
        break;
    
    default:
        break;
    }
}


uint8_t reverseBits(uint8_t bits) {
    uint8_t reverse = 0;
    for (int i = 0; i < 8; i++) {
        reverse <<= 1;
        reverse |= (bits & 1);
        bits >>= 1;
    }
    return reverse;
}

void wave_pattern(void) {
    if (pattern == 128) pattern = 1;
    else pattern = pattern << 1;
}

void curtain_pattern() {
    uint8_t current = pattern & 0b00001111;

    if (current == 0b1000) {
        current = 1;
    } else {
        current <<= 1;
    }

    pattern = reverseBits(current) | current;
}

void printPattern() {
    mxc_gpio_regs_t* port;
    int pin;
    for (uint8_t bit = 0; bit < 8; bit++) {
        setPin(bit, &port, &pin);
        if (pattern & (1 << bit)) {
            MXC_GPIO_OutSet(port, pin);
        } else {
            MXC_GPIO_OutClr(port, pin);
        }
    }
}

int main(void) {
    // Initialize the board and configure GPIO
    printf("Initializing LED wave pattern...\n");
    setup_gpio();

    while (1) {
        // MXC_GPIO_OutSet(MXC_GPIO0, LED1_PIN);  // Turn on LED 1 (P0_17)

        if (isWave) {
            wave_pattern();
        } else {
            curtain_pattern();
        }
        printPattern();
        MXC_Delay(MSEC(500));
    }

    return 0;
}
