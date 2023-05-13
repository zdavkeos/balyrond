// Quad encoder and 0-10V analog input

#include <stdio.h>

#include <pico/stdlib.h>
#include <hardware/adc.h>
#include "tusb.h"

#include "quadrature_encoder.pio.h"

const float deg_per_count = 360.0 / 4096.0;

int main(void)
{
    stdio_init_all();

    adc_init();
    adc_select_input(0); // analog in (A0)

    PIO pio = pio0;
    const uint sm = 0;
    uint offset = pio_add_program(pio, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio, sm, offset, 16, 0); // (D9 & D10)

    int last_count = 0;

    while (true)
    {
        if (tud_cdc_connected())
        {
            float dist = adc_read(); 
            int counts = quadrature_encoder_get_count(pio, sm);
            
            // compute velocity
            // int delta = counts - last_count;

            printf("%3.3f,%0.3f,%0.2f\n", counts, dist, 0.0);
        }   
    }

    return 0;
}
