#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include <stdlib.h>


#include "aeat6600.h"
#include "ili934x.h"
#include "hardware/gpio.h"
#include "gfxfont.h"
#include "FreeMono24pt7b.h"



/* SPI LCD PINS*/
#define PIN_MISO 4
#define PIN_CS   5
#define PIN_SCK  6
#define PIN_MOSI 7
#define PIN_DC   8
#define PIN_RST  9

#define SPI_PORT spi0


ILI934X *display = NULL;
uint64_t lastTime1hz;


/* --- Encoder Constants --- */
 
const uint8_t CLOCK_PIN = 15;
const uint8_t DATA_PIN = 16; 
const uint8_t CS_PIN = 14;
const uint8_t BIT_COUNT = 16;
const uint8_t MAGNETIC_FIELD_LOW_PIN = 8;
const uint8_t MAGNETIC_FIELD_HIGH_PIN = 9;
const uint8_t PROGRAMMING_PIN = 10;
const uint8_t ABS_ZERO_BUTTON = 11;


uint16_t ZeroPosition = 16;
uint16_t CurrentPosition = 0;



/* Flash Defines*/
#define FLASH_TARGET_OFFSET 0x100000 // 1MB
const uint8_t *FlashTargetContents = (uint8_t *)(FLASH_TARGET_OFFSET + XIP_BASE);




void writeZeroPositon()
{
   

}


void PrepareFlash()
{   
    uint32_t ints = save_and_disable_interrupts();
    //erase flash
    uint16_t data = (FlashTargetContents[0] << 8) | FlashTargetContents[1];
    if (data == 0xFFFF)
    {
        printf("Flash Not Programmed before setting up 0\n");
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
        uint8_t data[FLASH_PAGE_SIZE];
        data[0] = 0x00;
        data[1] = 0x00;
        flash_range_program(FLASH_TARGET_OFFSET, data, FLASH_PAGE_SIZE);
        ZeroPosition = 0;
    }
    else 
    {
        printf("Flash Programmed before value %d\n", data);
        ZeroPosition = data;
    }
    restore_interrupts (ints);
    return;
}


void ZeroPositionCallback(uint gpio, uint32_t events)
{
    if (gpio == ABS_ZERO_BUTTON && events == GPIO_IRQ_EDGE_RISE)
    {
        uint32_t ints = save_and_disable_interrupts(); 
        printf("Zero Position Callback \n");
        uint32_t seed = time_us_32();
        srand(seed);
        CurrentPosition = (uint16_t)rand(); 
        
        ZeroPosition = CurrentPosition;
        uint8_t data[FLASH_PAGE_SIZE];
        data[0] = (uint8_t)(ZeroPosition >> 8);
        data[1] = (uint8_t)(ZeroPosition & 0xFF);
        printf("writing %d to flash\n", data[0] << 8 | data[1]);
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET, data, FLASH_PAGE_SIZE);
        restore_interrupts (ints);
    }
   
    return;
}



int main()
{
    //set up serial
    stdio_init_all();

    spi_init(SPI_PORT, 500 * 1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);

    display = new ILI934X(SPI_PORT, PIN_CS, PIN_DC, PIN_RST);
    display->reset();
    display->init();
    sleep_ms(100);
    display->setRotation(R270DEG);
    display->clear(display->colour565(0,0,0));


   /*
    AEAT6600 *aet_encoder = new AEAT6600(
        CLOCK_PIN,
        DATA_PIN,
        CS_PIN,
        BIT_COUNT,
        MAGNETIC_FIELD_LOW_PIN,
        MAGNETIC_FIELD_HIGH_PIN,
        PROGRAMMING_PIN);

    aet_encoder->Init();

    */    
    //set ABS_ZERO_BUTTON as input interrupt
    gpio_init(ABS_ZERO_BUTTON);
    gpio_set_dir(ABS_ZERO_BUTTON, GPIO_IN);
    gpio_pull_up(ABS_ZERO_BUTTON);
    gpio_set_irq_enabled_with_callback(ABS_ZERO_BUTTON, GPIO_IRQ_EDGE_RISE, true, &ZeroPositionCallback);

   
 
    while (true)
    {

        /* code */
        //CurrentPosition = aet_encoder->Read();
        //printf("Raw Data : %d \t Zero Position : %d \n", CurrentPosition, ZeroPosition);
        //sleep_ms(100);

        //GFXfont *font = const_cast<GFXfont *>(&FreeMono24pt7b);

        //display->drawChar(0, 0, 'A', display->colour565(255,255,255), font);
        
        PrepareFlash();
        sleep_ms(1000);
    }

    return 0;
}
