// Mitutoyo SPC test

// Adapted from:
// https://github.com/hzeller/digi-spherometer/blob/master/firmware/dial-indicator-mitutoyo.h

#include <stdio.h>

#include <pico/stdlib.h>

#define SPC_NREQ 2
#define SPC_CLK  6
#define SPC_DATA 7

void spc_init()
{
    gpio_init(SPC_NREQ);
    gpio_init(SPC_CLK);
    gpio_init(SPC_DATA);

    gpio_set_dir(SPC_NREQ, GPIO_OUT);
    gpio_put(SPC_NREQ, 1);

    gpio_set_dir(SPC_CLK,  GPIO_IN);
    gpio_pull_up(SPC_CLK);
    gpio_set_dir(SPC_DATA, GPIO_IN);
    gpio_pull_up(SPC_DATA);
}

float
spc_read()
{
    uint8_t nibbles[13];

    gpio_put(SPC_NREQ, 0);

    uint32_t clock_inactive_count = 0;
    for (uint8_t nib = 0; nib < 13; nib++)
    {
        uint8_t nibble = 0;
        for (uint8_t bit = 1; bit != 0x10; bit <<= 1)
        {
            while (!gpio_get(SPC_CLK)) {
                // if (clock_inactive_count++ > 650000) {                
                //     return -1.0;
                // }
            } // Wait for clock high

            gpio_put(SPC_NREQ, 1);
             clock_inactive_count = 0;

            while (gpio_get(SPC_CLK)) {
                if (clock_inactive_count++ > 6500000) {
                    return -2.0;
                }
            } // Wait for falling edge

            if (gpio_get(SPC_DATA))
            {
                nibble |= bit;
            }
        }

        nibbles[nib] = nibble;
    }

    gpio_put(SPC_NREQ, 1);

    uint32_t val = 0;

    // Digits
    for (uint8_t i = 5; i < 10; i++)
    {
        val = val * 10 + nibbles[i];
    }

    // Negative
    if (nibbles[4] == 0b1000)
    {
        val = -val;
    }

    // Inch/Metric
    if (nibbles[12] == 0b0001)
    {
        return val * 0.00001f;
    } else {
        return val * .001f;
    }
}

int main(void)
{
    stdio_init_all();

    spc_init();

    while (true)
    {
        printf("%f\n", spc_read());
        sleep_ms(10);
    }

    return 0;
}

