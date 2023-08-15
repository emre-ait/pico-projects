import machine
import time

class R_Encoder:
    
    def __init__(self, A_Pin, B_Pin):
        self.A_Pin = A_Pin
        self.B_Pin = B_Pin
        self.Enc_Counter = 0
        self.Enc_A_State = 0
        self.Enc_B_State = 0
        self.Enc_A_State_old = 0
        self.Enc_B_State_old = 0
        self.error = 0
        
        self.Setup_Irqs()
        
    def Reset_Counter(self):
        self.Enc_Counter = 0
        
    def Enc_Handler(self, Source):
        self.Enc_A_State = self.Enc_Pin_A.value()
        self.Enc_B_State = self.Enc_Pin_B.value()
        
        if self.Enc_A_State == self.Enc_A_State_old and self.Enc_B_State == self.Enc_B_State_old:
            self.error += 1
            
        elif (self.Enc_A_State == 1 and self.Enc_B_State_old == 0) or (self.Enc_A_State == 0 and self.Enc_B_State_old == 1):
            self.Enc_Counter += 1
            
        elif (self.Enc_A_State == 1 and self.Enc_B_State_old == 1) or (self.Enc_A_State == 0 and self.Enc_B_State_old == 0):
            self.Enc_Counter -= 1
        
        else:
            self.error += 1
        self.Enc_A_State_old = self.Enc_A_State
        self.Enc_B_State_old = self.Enc_B_State
        
    def Setup_Irqs(self):
        self.Enc_Pin_A = machine.Pin(self.A_Pin, machine.Pin.IN, machine.Pin.PULL_DOWN)
        self.Enc_Pin_A.irq(trigger=machine.Pin.IRQ_RISING | machine.Pin.IRQ_FALLING, handler=self.Enc_Handler)
        self.Enc_Pin_B = machine.Pin(self.B_Pin, machine.Pin.IN, machine.Pin.PULL_DOWN)
        self.Enc_Pin_B.irq(trigger=machine.Pin.IRQ_RISING | machine.Pin.IRQ_FALLING, handler=self.Enc_Handler)
        
        self.Enc_A_State_old = self.Enc_Pin_A.value()
        self.Enc_B_State_old = sefl.Enc_Pin_B.value()
        
last_Enc_Counter = 0
Enc_Counter = 0
Last_Qtr_Cntr = 0
Qtr_Cntr = 0
error = 0

        