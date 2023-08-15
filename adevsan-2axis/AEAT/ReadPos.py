from machine import Pin, SPI
import machine
import utime


spi = SPI(0, baudrate=500000, sck=Pin(6), miso=Pin(4), polarity=0, phase=1, firstbit=SPI.MSB)

PWR_DN = machine.Pin(22, machine.Pin.OUT)
PWR_DN.value(0)

ALIGN = machine.Pin(26, machine.Pin.OUT)
ALIGN.value(0)

PROG = machine.Pin(27, machine.Pin.OUT)
PROG.value(0)

NCS = machine.Pin(5, machine.Pin.OUT)
NCS.value(1)

MAG_HI = machine.Pin(21, machine.Pin.IN)
MAG_LO = machine.Pin(20, machine.Pin.IN)
def SSI_Shift_In():
    data = 0
    sclk.value(0)
    utime.sleep_us(2)
    sclk.value(1)
    utime.sleep_us(2)
    for i in range(17):
        data <<= 1
        sclk.value(0)
        utime.sleep_us(2)
        sclk.value(1)
        utime.sleep_us(1)
        data |= miso.value() 
    return data

def readPosition():
    rxdata = bytearray(2)
    rxdata = 0
    NCS.value(0)
    utime.sleep_us(1)
    rxdata = spi.read(2)
    #print(rxdata)
    NCS.value(1)
    encoder_value = (rxdata[0] << 8) | rxdata[1]
    encoder_value = encoder_value >> 6
    return encoder_value
    """
    rawData = SSI_Shift_In()
    utime.sleep_us(25)
    angle = rawData * 360 / 65536
    return angle"""

while True:
    """
    if (MAG_LO.value() == 1):
        print("Magnetic field intensity is too low")
    if (MAG_HI.value() == 1):
        print("Magnetic field intensity is too high")
    if (MAG_LO.value() == 0) and (MAG_HI.value() == 0):
        print("Magnetic feild intensity is correct.")
    if (MAG_LO.value() == 1) and (MAG_HI.value() == 1):
        print("Fault, both pins reading high which should not be possible")
    """
    result = readPosition()
    print("Reading Position is: ", result)
    utime.sleep_ms(100)