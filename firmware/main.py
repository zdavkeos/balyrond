# Balyrond Roundness tester
#
# Firmware for Seeed XAIO
# CircuitPython 7.3.3

import time
import board
import rotaryio
import supervisor
from analogio import AnalogIn

deg_per_count = 360.0 / 1024.0
volts_per_count = 3.3 / 65536
deg_per_ms_to_rpm = (60.0 * 1000.0) / 360.0

enc = rotaryio.IncrementalEncoder(board.D9, board.D8)
ain = AnalogIn(board.A2)

while True:
    # print("angle,distance, velocity")

    velocity = 0
    enc.position = 0
    beg_t = time.monotonic()
    last_t = supervisor.ticks_ms()
    last_p = 0
    while time.monotonic() < (beg_t + 10.0):
        angle = enc.position * deg_per_count
        dist = ain.value * volts_per_count

        t = supervisor.ticks_ms()
        if t > last_t:
            v = (angle - last_p) / (t - last_t)
            v = v * deg_per_ms_to_rpm
            velocity += (v - velocity) * 0.80
        print("{0:3.3f},{1:0.3f},{2:0.2f}".format(angle, dist, velocity))
        last_t = t
        last_p = angle
