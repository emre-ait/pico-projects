import machine
import math
from micropython import const
import time
from PicoOLEDspi import OLED_1inch3
from machine import Pin
import os

REG_STATUS = const(0x0b)
i2c = machine.I2C(0,scl=machine.Pin(21),sda=machine.Pin(20),freq=400000)
i2c_1 = machine.I2C(1,scl=machine.Pin(19),sda=machine.Pin(18),freq=400000)
AS5600_id = const(0x36)  #Device ID

if ('A_ABS.txt' in os.listdir()):
    with open('A_ABS.txt') as a_f:
        a = a_f.read()
        a_f.close()
        abs_A_angle = int(a)
        inc_A_angle = int(a)
else:
    abs_A_angle = 0
    inc_A_angle = 0 

if ('B_ABS.txt' in os.listdir()):
    with open('B_ABS.txt') as b_f:
        b = b_f.read()
        b_f.close()
        abs_B_angle = int(b)
        inc_B_angle = int(b)
else:
    abs_B_angle = 0
    inc_B_angle = 0
   

Enc_A = i2c.scan() # Scan I2C bus for ENC_A
Enc_B = i2c_1.scan() # Scan I2C bus for ENC_B

if AS5600_id in Enc_A:
    print('Found AS5600 (id =',hex(AS5600_id),')')
else:
    print('AS5600 Not Found! (id =',hex(AS5600_id),')')
    
if AS5600_id in Enc_B:
    print('Found AS5600 (id =',hex(AS5600_id),')')
else:
    print('AS5600 Not Found! (id =',hex(AS5600_id),')')

OLED = OLED_1inch3()
OLED.fill(0x0000) # balck
OLED.show() # show balck
keyA = Pin(17,Pin.IN,Pin.PULL_UP) # ENC_A INC ZERO SET BUTTON
keyB = Pin(15,Pin.IN,Pin.PULL_UP) # ENC_B INC ZERO SET BUTTON

keyA_ABS = Pin(22,Pin.IN,Pin.PULL_UP) # ENC_A ABS ZERO SET BUTTON
keyB_ABS = Pin(16,Pin.IN,Pin.PULL_UP) # ENC_B ABS ZERO SET BUTTON

# OLED DISPLAY ONE CELL SIZE
ROW_WIDTH = 42
ROW_HEIGHT = const(20)

# TITLE
OLED.rect(0,0,ROW_WIDTH,ROW_HEIGHT,OLED.white)  
OLED.text("ENC",9,6,OLED.white) 
OLED.rect(ROW_WIDTH,0,ROW_WIDTH,ROW_HEIGHT,OLED.white) 
OLED.text("ABS",51,6,OLED.white)
OLED.rect(ROW_WIDTH*2,0,ROW_WIDTH,ROW_HEIGHT,OLED.white)
OLED.text("INC",93,6,OLED.white)

# SHOW ENC_A ABS ANGLE AND INC ANGLE
OLED.rect(0,ROW_HEIGHT,ROW_WIDTH,ROW_HEIGHT,OLED.white)
OLED.text("A",9,26,OLED.white)
OLED.rect(ROW_WIDTH,ROW_HEIGHT,ROW_WIDTH,ROW_HEIGHT,OLED.white)
OLED.rect(ROW_WIDTH*2,ROW_HEIGHT,ROW_WIDTH,ROW_HEIGHT,OLED.white)

# SHOW ENC_B ABS ANGLE AND INC ANGLE
OLED.rect(0,ROW_HEIGHT*2,ROW_WIDTH,ROW_HEIGHT,OLED.white)
OLED.text("B",9,46,OLED.white)
OLED.rect(ROW_WIDTH,ROW_HEIGHT*2,ROW_WIDTH,ROW_HEIGHT,OLED.white)
OLED.rect(ROW_WIDTH*2,ROW_HEIGHT*2,ROW_WIDTH,ROW_HEIGHT,OLED.white)

def show_Enc_A_Angle():
    global inc_A_angle, abs_A_angle
    raw_angle_a = i2c.readfrom_mem(AS5600_id,0x0c,2)
    raw_angle_a = ((raw_angle_a[0] & 15) << 8) | raw_angle_a[1]
    
    angle_a = raw_angle_a - abs_A_angle
    if (angle_a < 0):
        angle_a = 4096 + angle_a
        
    inc_A = raw_angle_a - inc_A_angle
    if (inc_A < 0):
        inc_A = 4096 + inc_A
    
    abs_degree_a = angle_a * 360 / 4096
    inc_degree_a = inc_A * 360 / 4096
    
    OLED.fill_rect(ROW_WIDTH+1,ROW_HEIGHT+1,ROW_WIDTH-2,ROW_HEIGHT-2, OLED.balck)
    OLED.text('{:0.1f}'.format(abs_degree_a),43,26,OLED.white )
    
    OLED.fill_rect(ROW_WIDTH*2+1,ROW_HEIGHT+1,ROW_WIDTH-2,ROW_HEIGHT-2, OLED.balck)
    OLED.text('{:0.1f}'.format(inc_degree_a), 85,26,OLED.white)

def show_Enc_B_Angle():
    global inc_B_angle, abs_B_angle
    raw_angle_b = i2c_1.readfrom_mem(AS5600_id,0x0c,2)
    raw_angle_b = ((raw_angle_b[0] & 15) << 8) | raw_angle_b[1]
    
    angle_b = raw_angle_b - abs_B_angle
    if (angle_b < 0):
        angle_b = 4096 + angle_b
            
    inc_B = raw_angle_b - inc_B_angle
    if (inc_B < 0):
        inc_B = 4096 + inc_B
        
    abs_degree_b = angle_b * 360 / 4096
    inc_degree_b = inc_B * 360 / 4096
    
    OLED.fill_rect(ROW_WIDTH+1,ROW_HEIGHT*2+1,ROW_WIDTH-2,ROW_HEIGHT-2, OLED.balck)
    OLED.text('{:0.1f}'.format(abs_degree_b),43,46,OLED.white )
    
    OLED.fill_rect(ROW_WIDTH*2+1,ROW_HEIGHT*2+1,ROW_WIDTH-2,ROW_HEIGHT-2, OLED.balck)
    OLED.text('{:0.1f}'.format(inc_degree_b), 85,46,OLED.white)

def RST_Inc_A():
    global inc_A_angle
    angle = i2c.readfrom_mem(AS5600_id,0x0c,2)
    angle = ((angle[0] & 15) << 8) | angle[1]
    inc_A_angle = angle

def RST_Inc_B():
    global inc_B_angle
    inc_b = i2c_1.readfrom_mem(AS5600_id,0x0c,2)
    inc_b = ((inc_b[0] & 15) << 8) | inc_b[1]
    inc_B_angle = inc_b

def write_A_ABS():
    global abs_A_angle
    abs_A_angle = i2c.readfrom_mem(AS5600_id,0x0c,2)
    abs_A_angle = ((abs_A_angle[0] & 15) << 8) | abs_A_angle[1]
    file = open('A_ABS.txt', 'w')
    file.write(str(abs_A_angle))
    file.close()

def write_B_ABS():
    global abs_B_angle
    abs_B_angle = i2c_1.readfrom_mem(AS5600_id,0x0c,2)
    abs_B_angle = ((abs_B_angle[0] & 15) << 8) | abs_B_angle[1]
    file = open('B_ABS.txt', 'w')
    file.write(str(abs_B_angle))
    file.close()
    
while(1):
    show_Enc_A_Angle()
    show_Enc_B_Angle()
    
    if keyA.value() == 0:
        RST_Inc_A()
        
    if keyB.value() == 0:
        RST_Inc_B()
        
    if keyA_ABS.value() == 0:
        write_A_ABS()
        
    if keyB_ABS.value() == 0:
        write_B_ABS()       
            
    OLED.show()


