from machine import Pin
import machine
import time
import utime

delay = 0.00000005
bitcount = 16

PWR_DN = machine.Pin(22, machine.Pin.OUT)
PWR_DN.value(0)

ALIGN = machine.Pin(26, machine.Pin.OUT)
ALIGN.value(0)

PROG = machine.Pin(27, machine.Pin.OUT)
PROG.value(0)

CS = machine.Pin(14, machine.Pin.OUT)
CS.value(1)

CLK = machine.Pin(15, machine.Pin.OUT)
CLK.value(1)

MISO = machine.Pin(16, machine.Pin.IN)

MAG_HI = machine.Pin(21, machine.Pin.IN)
MAG_LO = machine.Pin(20, machine.Pin.IN)

def readPosition():
    data = 0
    CS.value(0)
    utime.sleep_us(1)
    for _ in range (16):
        data <<= 1
        CLK.value(0)
        utime.sleep_us(2)
        CLK.value(1)
        utime.sleep_us(1)
        data |= MISO.value()
    CS.value(1)
    return data

while True:
    """if (MAG_LO.value() == 1):
        print("Magnetic field intensity is too low")
    if (MAG_HI.value() == 1):
        print("Magnetic field intensity is too high")
    if (MAG_LO.value() == 0) and (MAG_HI.value() == 0):
        print("Magnetic feild intensity is correct.")
    if (MAG_LO.value() == 1) and (MAG_HI.value() == 1):
        print("Fault, both pins reading high which should not be possible")"""
    result = readPosition()
    print("Reading Position is: ", result)
    utime.sleep_ms(100)
