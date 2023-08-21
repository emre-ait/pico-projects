#include <stdio.h>

class AEAT6600
{
public:
    AEAT6600(
    uint8_t clck_pin,  
    uint8_t data_pin, 
    uint8_t cs_pin, 
    uint8_t bit_count, 
    uint8_t magnetic_low_pin, 
    uint8_t magnetic_high_pin, 
    uint8_t programing_pin
    );
    void Init();
    uint16_t Read();
    void SetZeroPositon();
    uint16_t GetZeroPosition();
    void Program();
    int GetMagneticFieldStrength();
private:
    uint16_t _zero_position = 0;
    uint16_t _old_position = 0;
    uint8_t _clck_pin;
    uint8_t _data_pin;
    uint8_t _cs_pin;
    uint8_t _bit_count;
    uint8_t _magnetic_low_pin;
    uint8_t _magnetic_high_pin;
    uint8_t _programing_pin;
};
