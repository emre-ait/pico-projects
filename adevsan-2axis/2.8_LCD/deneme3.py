import machine
import utime
import machine
from machine import Timer
import math
from micropython import const
import time
from time import sleep
from ili9341 import Display, color565
from xglcd_font import XglcdFont
from machine import Pin, SPI
import os

last_abs_a_degree = 0
last_abs_c_degree = 0
last_inc_a_degree = 0
last_inc_c_degree = 0

last_status_A = [0]
last_status_B = [0]

# Initialize the GPIO pins
button_INC_A = machine.Pin(27, machine.Pin.IN, machine.Pin.PULL_UP)
button_INC_C = machine.Pin(28, machine.Pin.IN, machine.Pin.PULL_UP)

button_ABS_A = machine.Pin(22, machine.Pin.IN, machine.Pin.PULL_UP)
button_ABS_C = machine.Pin(26, machine.Pin.IN, machine.Pin.PULL_UP)

lcd_Close = Pin(7,Pin.OUT)
lcd_Close.value(1)

# Initialize the Encoders I2C interface
REG_STATUS = const(0x0b)
AGC_STATUS = const(0x1a)
MH = const(8)   #Magnet too strong (high)
ML = const(16)  #Magnet too weak (low)
MD = const (32) #Magnet detected
i2c_A = machine.I2C(0,scl=machine.Pin(21),sda=machine.Pin(20),freq=400000)
i2c_C = machine.I2C(1,scl=machine.Pin(19),sda=machine.Pin(18),freq=400000)
AS5600_id = const(0x36)  #Device ID

# Initialize LCD Display SPI interface
spi = SPI(1, baudrate=40000000, sck=Pin(10), mosi=Pin(11))
display = Display(spi, dc=Pin(8), cs=Pin(9), rst=Pin(12))

# Initialize LCD Display Font
print('Loading unispace')
font_width = 12
font_height = 24
unispace = XglcdFont('fonts/Unispace12x24.c', font_width, font_height)

# Initialize the Encoder Absolute Values
if ('ABS_A.txt' in os.listdir()):
    with open('ABS_A.txt') as ENC_A_file:
        a = ENC_A_file.read()
        ENC_A_file.close()
        abs_A_angle = int(a)
        inc_A_angle = int(a)
else:
    abs_A_angle = 0
    inc_A_angle = 0

if ('ABS_C.txt' in os.listdir()):
    with open('ABS_C.txt') as ENC_C_file:
        c = ENC_C_file.read()
        ENC_C_file.close()
        abs_C_angle = int(c)
        inc_C_angle = int(c)
else:
    abs_C_angle = 0
    inc_C_angle = 0

# Encoders scaning for comminications
Enc_A = i2c_A.scan() # Scan I2C bus for ENC_A
Enc_C = i2c_C.scan() # Scan I2C bus for ENC_C

if AS5600_id in Enc_A:
    print('Found Encoder_A (id =',hex(AS5600_id),')')
else:
    print('Encoder_A Not Found! (id =',hex(AS5600_id),')')
    
if AS5600_id in Enc_C:
    print('Found Encoder_C (id =',hex(AS5600_id),')')
else:
    print('Encoder_C Not Found! (id =',hex(AS5600_id),')')
    
# Function to read Encoder_A value
def read_Enc_A_Value():
    global Enc_A_Value
    Enc_A_Value = i2c_A.readfrom_mem(AS5600_id,0x0c,2)
    Enc_A_Value = ((Enc_A_Value[0] & 15) << 8) | Enc_A_Value[1]
    return Enc_A_Value

# Function to read Encoder_C value
def read_Enc_C_Value():
    global Enc_C_Value
    Enc_C_Value = i2c_C.readfrom_mem(AS5600_id,0x0c,2)
    Enc_C_Value = ((Enc_C_Value[0] & 15) << 8) | Enc_C_Value[1]
    return Enc_C_Value

# Initialize the counter
counter = 0

# Funtion to draw default table to LCD
def draw_chart():
    global last_abs_a_degree, last_abs_c_degree, last_inc_a_degree, last_inc_c_degree
    last_abs_a_degree = 0
    last_abs_c_degree = 0
    last_inc_a_degree = 0
    last_inc_c_degree = 0
    spi = SPI(1, baudrate=40000000, sck=Pin(10), mosi=Pin(11))
    display = Display(spi, dc=Pin(8), cs=Pin(9), rst=Pin(12))
    lcd_width = 320
    lcd_height = 240
    row_height = 60
# display.draw_rectangle(0, 0, 320, 240, color565(255, 255, 255))
    display.draw_rectangle(0, 0, 320, 60, color565(255, 255, 255))
    display.draw_rectangle(0, 60, 320, 60, color565(255, 255, 255))
    display.draw_rectangle(0, 120, 320, 60, color565(255, 255, 255))
    display.draw_rectangle(0, 180, 320, 60, color565(255, 255, 255))
    display.draw_rectangle(0, 0, 80, 240, color565(255, 255, 255))
    display.draw_rectangle(80, 0, 120, 240, color565(255, 255, 255))
    display.draw_rectangle(200, 0, 120, 240, color565(255, 255, 255))
    
    display.draw_text(3, 20, 'ENC', unispace, color565(255, 128, 0))
    display.draw_text(3, 80, 'STATUS', unispace, color565(255, 128, 0))
    display.draw_text(3, 140, 'INC', unispace, color565(255, 128, 0))
    display.draw_text(3, 200, 'ABS', unispace, color565(255, 128, 0))
    display.draw_text(100, 20, 'A AXIS', unispace, color565(255, 128, 0))
    display.draw_text(220, 20, 'C AXIS', unispace, color565(255, 128, 0))
    
draw_chart()


# Function to toggle lcd on/off if both buttons are pressed simultaneously for 2 seconds
def toggle_lcd():
    global counter
    start_time = utime.ticks_ms()
    while not button_INC_A.value() and not button_INC_C.value():
        if utime.ticks_diff(utime.ticks_ms(), start_time) >= 2000:
            lcd_Close.toggle()
            if lcd_Close.value() == 1:
                print("lcd is opened draw chart")
                draw_chart()
            break

def show_Enc_A_Angle():
    global inc_A_angle, abs_A_angle, last_abs_a_degree, last_inc_a_degree
    raw_angle_a = read_Enc_A_Value()
    
    angle_a = raw_angle_a - abs_A_angle
    if (angle_a < 0):
        angle_a = 4096 + angle_a
        
    inc_A = raw_angle_a - inc_A_angle
    if (inc_A < 0):
        inc_A = 4096 + inc_A
    
    abs_degree_a = angle_a * 360 / 4096
    inc_degree_a = inc_A * 360 / 4096
    
    if ((last_abs_a_degree != abs_degree_a) or (last_inc_a_degree != inc_degree_a)):
        last_abs_a_degree = abs_degree_a
        last_inc_a_degree = inc_degree_a
        string_abs_A = "{:.2f}".format(last_abs_a_degree)
        string_inc_A = "{:.2f}".format(last_inc_a_degree)
        
        display.fill_rectangle(81, 181, 118, 58, color565(0, 0, 0))
        display.draw_text(100, 200, string_abs_A, unispace, color565(255, 128, 0))
        display.fill_rectangle(81, 121, 118, 58, color565(0, 0, 0))
        display.draw_text(100, 140, string_inc_A, unispace, color565(255, 128, 0))
        
def show_Enc_C_Angle():
    global inc_C_angle, abs_C_angle, last_abs_c_degree, last_inc_c_degree
    raw_angle_c = read_Enc_C_Value()
    
    angle_c = raw_angle_c - abs_C_angle
    if (angle_c < 0):
        angle_c = 4096 + angle_c
            
    inc_C = raw_angle_c - inc_C_angle
    if (inc_C < 0):
        inc_C = 4096 + inc_C
        
    abs_degree_c = angle_c * 360 / 4096
    inc_degree_c = inc_C * 360 / 4096
    
    if ((last_abs_c_degree != abs_degree_c) or (last_inc_c_degree != inc_degree_c)):
        last_abs_c_degree = abs_degree_c
        last_inc_c_degree = inc_degree_c
        string_abs_C = "{:.1f}".format(last_abs_c_degree)
        string_inc_C = "{:.1f}".format(last_inc_c_degree)
        
        display.fill_rectangle(201, 181, 118, 58, color565(0, 0, 0))
        display.draw_text(220, 200, string_abs_C, unispace, color565(255, 128, 0))
        display.fill_rectangle(201, 121, 118, 58, color565(0, 0, 0))
        display.draw_text(220, 140, string_inc_C, unispace, color565(255, 128, 0))
        
def RST_Inc_A():
    global inc_A_angle
    inc_A_angle = read_Enc_A_Value()

def RST_Inc_C():
    global inc_C_angle
    inc_C_angle = read_Enc_C_Value()
    
def write_ABS_A():
    global abs_A_angle
    print("ABS_A Set Zero")
    abs_A_angle = read_Enc_A_Value()
    file = open('ABS_A.txt', 'w')
    file.write(str(abs_A_angle))
    file.close()

def write_ABS_C():
    global abs_C_angle
    print("ABS_C Set Zero")
    abs_C_angle = read_Enc_C_Value()
    file = open('ABS_C.txt', 'w')
    file.write(str(abs_C_angle))
    file.close()
    
"""def check_AGC_Status_A():
    Ags_A_Value = i2c_A.readfrom_mem(AS5600_id,0x1a,1)
    #Ags_A_Value = ((Ags_A_Value[0] & 15) << 8) | Ags_A_Value[1]
    #decimal_num = int(Ags_A_Value)
    #return Ags_A_Value
    print(Ags_A_Value)"""

"""def check_Status_A():
    status_A = i2c_A.readfrom_mem(AS5600_id,REG_STATUS,1)
    print(bin(status_A[0]))
    if last_status_A[0] != status_A[0]:
        if status_A[0] & MH:
            display.fill_rectangle(81, 61, 118, 58, color565(255,0,0))
        if status_A[0] & ML:
            display.fill_rectangle(81, 61, 118, 58, color565(255,255,0))    
        if status_A[0] & (ML | MH) == 0:
            display.fill_rectangle(81, 61, 118, 58, color565(0,255,0))
        if status_A[0] & MD:
            display.draw_text(100, 20, 'ENC - A', unispace, color565(255, 128, 0), background=color565(0,255,0))
        else:
            display.draw_text(100, 20, 'ENC - A', unispace, color565(255, 128, 0), background=color565(255,0,0))"""
# Main loop
while True:
    show_Enc_A_Angle()
    show_Enc_C_Angle()
    if not button_INC_A.value():
        toggle_lcd()
        RST_Inc_A()
        utime.sleep_ms(80)
        
    elif not button_INC_C.value():
        toggle_lcd()
        RST_Inc_C()
        utime.sleep_ms(80)
        
    elif not button_ABS_A.value():
        write_ABS_A()
        
    elif not button_ABS_C.value():
        write_ABS_C()
        
    utime.sleep_ms(80)  # Small delay to debounce the buttons
