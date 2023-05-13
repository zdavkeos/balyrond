// Quad encoder and 0-10V analog input

#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/time.h>
#include <hardware/adc.h>
#include "tusb.h"

#include "quadrature_encoder.pio.h"

// Unit conversions
const float counts_per_rev = 4096.0;
const float deg_per_count = 360.0 / counts_per_rev;
const float counts_per_us_to_rpm = (1000000.0 * 60.0) / (counts_per_rev);
const float vel_filter_coef = 0.8; // Between 0 and 1 for exponential averaging

int main(void)
{
    stdio_init_all();

    adc_init();
    adc_select_input(0); // analog in (A0)

    PIO pio = pio0;
    const uint sm = 0;
    uint offset = pio_add_program(pio, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio, sm, offset, 3, 0); // (GPIO3 & GPIO4)

    int last_count = 0;
    absolute_time_t last_time = get_absolute_time();
    float vel_avg = 0.0;

    while (true)
    {
        if (tud_cdc_connected())
        {
            float dist = adc_read(); 
            int counts = quadrature_encoder_get_count(pio, sm);
            absolute_time_t time = get_absolute_time();
            
            int64_t diff = absolute_time_diff_us(last_time, time);
            last_time = time;

            float angle = counts * deg_per_count;
            
            // compute velocity
            int delta = counts - last_count;
            last_count = counts;

            float vel = (float)delta / (float)diff * counts_per_us_to_rpm;
            vel_avg = vel_avg * vel_filter_coef + (1-vel_filter_coef) * vel;

            printf("%3.3f,%0.3f,%0.2f\n", angle, dist, vel_avg);

            sleep_ms(1);
        }   
    }

    return 0;
}

