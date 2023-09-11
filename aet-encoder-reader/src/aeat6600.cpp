#include "aeat6600.h"
#include "hardware/gpio.h"
#include "pico/time.h"


AEAT6600::AEAT6600(
    uint8_t clck_pin,  
    uint8_t data_pin, 
    uint8_t cs_pin, 
    uint8_t bit_count, 
    uint8_t magnetic_low_pin, 
    uint8_t magnetic_high_pin, 
    uint8_t programing_pin
    )
{
    _clck_pin = clck_pin;
    _data_pin = data_pin;
    _cs_pin = cs_pin;
    _bit_count = bit_count;
    _magnetic_low_pin = magnetic_low_pin;
    _magnetic_high_pin = magnetic_high_pin;
    _programing_pin = programing_pin;
}

void AEAT6600::Init()
{
    gpio_init(_clck_pin);
    gpio_set_dir(_clck_pin, GPIO_OUT);
    gpio_init(_data_pin);
    gpio_set_dir(_data_pin, GPIO_IN);
    gpio_init(_cs_pin);
    gpio_set_dir(_cs_pin, GPIO_OUT);
    gpio_init(_magnetic_low_pin);
    gpio_set_dir(_magnetic_low_pin, GPIO_IN);
    gpio_init(_magnetic_high_pin);
    gpio_set_dir(_magnetic_high_pin, GPIO_IN);
    gpio_init(_programing_pin);
    gpio_set_dir(_programing_pin, GPIO_OUT);
    gpio_put(_programing_pin, 1);
    gpio_put(_cs_pin, 1);
    gpio_put(_clck_pin, 1);
}

uint16_t AEAT6600::Read()
{
    uint16_t data = 0;

    //-----------------------------
    // CYCLE PREPARE
    //-----------------------------

    // Make sure clock is High
    gpio_put(_clck_pin, 1);
    sleep_us(2);

    // Maks sure the chip is high state in the begining 
    gpio_put(_cs_pin, 1);
    sleep_us(2);
    // Set the chip select to low
    gpio_put(_cs_pin, 0);
    sleep_us(2);

    // Set the clock to low
    gpio_put(_clck_pin, 0);
    sleep_us(2);


    /* -----------------------------
    START OF CYCLE READ
    -----------------------------
        create a loop to read the data
        data is reading falling edge of the clock
    -----------------------------*/
    for (uint8_t i = 0; i < _bit_count; i++)
    {
        gpio_put(_clck_pin, 1);
        sleep_us(2);
        gpio_put(_clck_pin, 0);
        data = data << 1 | gpio_get(_data_pin);
        sleep_us(2);
    }
    gpio_put(_clck_pin, 1);
    sleep_us(20);

    if(data > _old_position+1 || data < _old_position -1)
        _old_position = data;

    return _old_position;
}

void AEAT6600::Program()
{
  //not implemented yet
  return;
}

int AEAT6600::GetMagneticFieldStrength()
{   


    if (!gpio_get(_magnetic_high_pin)  && !gpio_get(_magnetic_low_pin))
    {
        // magnetic field is correct
        return 0;
    }
    else if (gpio_get(_magnetic_high_pin) && !gpio_get(_magnetic_low_pin))
    {
        // magnetic field is too high
        return 1;
    }
    else if (!gpio_get(_magnetic_high_pin) && gpio_get(_magnetic_low_pin))
    {
        // magnetic field is too low
        return -1;
    }
    else
    {
        // magnetic field is not correct
        return -2;
    }
}

uint16_t AEAT6600::GetZeroPosition()
{
    return _zero_position;
}

void AEAT6600::SetZeroPositon()
{
    _zero_position = Read();
}


