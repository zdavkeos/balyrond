# Balyrond Roundness tester
#
# Firmware for Seeed XAIO
# CircuitPython 7.3.3

import time
import board
import usb_cdc
import rotaryio
import supervisor
from analogio import AnalogIn

deg_per_count = 360.0 / 4096.0
volts_per_count = 3.3 / 65536
deg_per_ms_to_rpm = (60.0 * 1000.0) / 360.0

enc = rotaryio.IncrementalEncoder(board.D9, board.D8)
ain = AnalogIn(board.A3)

while True:

    velocity = 0
    enc.position = 0 # reset position each capture
    beg_t = time.monotonic()
    last_t = supervisor.ticks_ms()
    last_p = 0

    # wait for host program connect
    while usb_cdc.console.connected:
        angle = enc.position * deg_per_count % 360.0
        dist = ain.value / 1000.0 # * volts_per_count

        t = supervisor.ticks_ms()
        #if t > last_t and abs(angle - last_p) < 50:
        #    v = (angle - last_p) / (t - last_t)
        #    v = v * deg_per_ms_to_rpm
        #    velocity += (v - velocity) * 0.80
        print("{0:0.3f},{1:0.3f},{2:0.2f}".format(angle, dist, velocity))
        last_t = t
        last_p = angle
