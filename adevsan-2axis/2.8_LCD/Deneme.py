"""ILI9341 demo (shapes)."""
import machine
from machine import Timer
import utime
import math
from micropython import const
import time
from                                                                                                                                                                                                                                                                                                                                                                                                                                                                               time import sleep
from ili9341 import Display, color565
from machine import Pin, SPI
import os

def test():
    
    """Test code."""
    # Baud rate of 40000000 seems about the max
    signal = Pin(7,Pin.OUT)
    signal.value(1)
    
    def toggle(Source):
        signal.toggle()
        
    spi = SPI(1, baudrate=40000000, sck=Pin(10), mosi=Pin(11))
    display = Display(spi, dc=Pin(8), cs=Pin(9), rst=Pin(12))
    
    display.clear()
    display.draw_hline(0, 0, 100, color565(255, 0, 255))
    
    button_INC_A = Pin(27,Pin.IN,Pin.PULL_UP)
    button_INC_A.irq(trigger = Pin.IRQ_FALLING, handler = toggle)
    
    
        
    while True:
       sleep(5)

test()
