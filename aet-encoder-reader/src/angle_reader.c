#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"


/* --- Constants --- */
 
const uint8_t CLOCK_PIN = 5;
const uint8_t DATA_PIN = 6; 
const uint8_t CS_PIN = 7;
const uint8_t BIT_COUNT = 16;
const uint8_t MAGNETIC_FIELD_LOW_PIN = 8;
const uint8_t MAGNETIC_FIELD_HIGH_PIN = 9;
const uint8_t PROGRAMMING_PIN = 10;
const uint8_t ABS_ZERO_BUTTON = 11;

/** Thresh values*/
const float MAX_ANGLE = 360.0;
const float MIN_ANGLE = 0.0;
const uint16_t THRESHOLD = 1;




uint16_t PREVIOUS_RAW_DATA = 0;

uint16_t ZERO_POSITION = 0;


/* --- Function Declarations --- */
/**
 * @brief Sets up the pins and serial
 * @returns void
 */
void Setup();


/**
 * @brief Reads the position of the magnet
 * @returns float angle in degrees
 */
float ReadPosition();

/**
 * @brief reads the raw data from the sensor
 * @returns uint16_t raw data
 */
uint16_t ShiftIn(const uint8_t dataPin, const uint8_t clockPin, const uint8_t bitCount);


/** 
 * @brief Sets the zero position of the sensor
 * @returns void
 */
void SetZeroPosition();



int main()
{
    Setup();

    while (true)
    {
        float angle = ReadPosition();

        printf("Angle: %f\n", angle);
        sleep_ms(10);
    
        // loop'ta ki gecikmeleri minimize eden bir işlev
        tight_loop_contents();
    }


    return 0;


}


void Setup()
{
    //set up pins
   
    gpio_init(CLOCK_PIN);
    gpio_set_dir(CLOCK_PIN, GPIO_OUT);
    
    gpio_init(DATA_PIN);
    gpio_set_dir(DATA_PIN, GPIO_IN);

    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);

    gpio_init(MAGNETIC_FIELD_LOW_PIN);
    gpio_set_dir(MAGNETIC_FIELD_LOW_PIN, GPIO_IN);
    
    gpio_init(MAGNETIC_FIELD_HIGH_PIN);
    gpio_set_dir(MAGNETIC_FIELD_HIGH_PIN, GPIO_IN);

    gpio_put(PROGRAMMING_PIN, false);
    gpio_put(CLOCK_PIN, true);
    gpio_put(CS_PIN, true);

    
    // set up interrupts
    gpio_init(ABS_ZERO_BUTTON);
    gpio_set_dir(ABS_ZERO_BUTTON, GPIO_IN);
    gpio_pull_down(ABS_ZERO_BUTTON);    
    gpio_set_irq_enabled_with_callback(ABS_ZERO_BUTTON, GPIO_IRQ_EDGE_FALL, true, &SetZeroPosition);
    
    //set up serial
    stdio_init_all();
    return;
}


uint16_t ShiftIn(const uint8_t dataPin, const uint8_t clockPin, const uint8_t bitCount)
{
    // data variable to hold the incoming data
    uint16_t raw_data = 0;

    // Make sure clock pin starts high
    gpio_put(CS_PIN, true);
    sleep_us(2);

    // Bring CS pin low to start communication
    gpio_put(CS_PIN, false);
    sleep_us(2);
    // Clock data in
    gpio_put(CLOCK_PIN, false);
    sleep_us(2);
    for (uint8_t i = 0; i < BIT_COUNT; i++)
    {
        gpio_put(CLOCK_PIN, true);
        sleep_us(2);
        gpio_put(CLOCK_PIN, false);
        sleep_us(2);

        raw_data = raw_data << 1 | gpio_get(DATA_PIN);
    }

    // Clock Pin high to end communication
    gpio_put(CS_PIN, true);
    sleep_us(20);

    if (raw_data < PREVIOUS_RAW_DATA + 1 && raw_data > PREVIOUS_RAW_DATA - 1)
    {
        raw_data = PREVIOUS_RAW_DATA;
    }

    if (raw_data < 3)
    {
        raw_data = 0;
    }
    else if (raw_data > 65532)
    {
        raw_data = 65532;
    }

    
    // shift raw data to actual zero position
    if (ZERO_POSITION > raw_data)
    {
        raw_data = 65532 - ZERO_POSITION + raw_data;
    }
    else
    {
    raw_data = raw_data - ZERO_POSITION;
    }

    PREVIOUS_RAW_DATA = raw_data;

    return raw_data;
}


float ReadPosition()
{
    uint16_t data = ShiftIn(DATA_PIN, CLOCK_PIN, BIT_COUNT);

    float angle = (float)data / 65532.0 * 360.0;
    if (angle > MAX_ANGLE)
    {
        angle = MAX_ANGLE;
    }
    else if (angle < MIN_ANGLE)
    {
        angle = MIN_ANGLE;
    }

    return angle;
}


void SetZeroPosition()
{

    int sum = 0;
    for(int i = 0; i < 100; i++)
    {
       sum += ShiftIn(DATA_PIN, CLOCK_PIN, BIT_COUNT);
    }
    
    ZERO_POSITION =  (uint16_t)(sum / 100);
    
    return;
}




