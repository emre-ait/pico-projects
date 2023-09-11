#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <math.h>


#include "aeat6600.h"
#include "ili934x.h"
#include "hardware/gpio.h"
#include "gfxfont.h"
#include "FreeSansBold12pt7b.h"

/* Flash Defines*/
#define FLASH_TARGET_OFFSET 0x100000 // 1MB
const uint8_t *FlashTargetContents = (uint8_t *)(FLASH_TARGET_OFFSET + XIP_BASE);


/* FONTS WITH POINT SIZE */
GFXfont *font_sansb12 = const_cast<GFXfont *>(&FreeSansBold12pt7b);


/* SPI LCD PINS*/
#define PIN_MISO 4
#define PIN_CS 5
#define PIN_SCK 6
#define PIN_MOSI 7
#define PIN_DC 8
#define PIN_RST 9

#define SPI_PORT spi0

ILI934X *display = NULL;
uint64_t lastTime1hz;

/* --- Constants --- */

const uint8_t BIT_COUNT = 16;

const uint8_t A_AXIS_CS_PIN = 14;
const uint8_t A_AXIS_CLOCK_PIN = 15;
const uint8_t A_AXIS_DATA_PIN = 16;

const uint8_t A_AXIS_MAGNETIC_FIELD_LOW_PIN = 1;
const uint8_t A_AXIS_MAGNETIC_FIELD_HIGH_PIN = 1;
const uint8_t A_AXIS_PROGRAMMING_PIN = 1;

const uint8_t C_AXIS_CLOCK_PIN = 1;
const uint8_t C_AXIS_DATA_PIN = 1;
const uint8_t C_AXIS_CS_PIN = 1;
const uint8_t C_AXIS_MAGNETIC_FIELD_LOW_PIN = 0;
const uint8_t C_AXIS_MAGNETIC_FIELD_HIGH_PIN = 1;
const uint8_t C_AXIS_PROGRAMMING_PIN = 2;

const uint8_t ABS_A_ZERO_BUTTON = 22;
const uint8_t ABS_C_ZERO_BUTTON = 26;
const uint8_t INC_A_ZERO_BUTTON = 27;
const uint8_t INC_C_ZERO_BUTTON = 28;
const uint8_t LCD_BACKLIGHT = 3;

uint16_t A_Axis_Abs_ZeroPos = 0;
uint16_t C_Axis_Abs_ZeroPos = 0; 
uint16_t C_Axis_Inc_ZeroPos = 0;
uint16_t A_Axis_Inc_ZeroPos = 0;
uint16_t CurrentPositionAxisA = 0;
uint16_t CurrentPositionAxisC = 0;
std::string Last_abs_degree_A_String = "1.23";
std::string Last_abs_degree_C_String = "1.23";
std::string Last_inc_degree_A_String = "1.23";
std::string Last_inc_degree_C_String = "1.23";



void IncAbsZeroPositionCallback(uint gpio, uint32_t events)
{
    if (gpio == ABS_A_ZERO_BUTTON)
    {
        A_Axis_Abs_ZeroPos = CurrentPositionAxisA;

        uint32_t ints = save_and_disable_interrupts(); 
        printf("Zero Position Callback \n");
        uint8_t data[FLASH_PAGE_SIZE];
        data[0] = (uint8_t)(A_Axis_Abs_ZeroPos >> 8);
        data[1] = (uint8_t)(A_Axis_Abs_ZeroPos & 0xFF);
        printf("writing %d to flash\n", data[0] << 8 | data[1]);
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET, data, FLASH_PAGE_SIZE);
        restore_interrupts (ints);
    }

    if (gpio == ABS_C_ZERO_BUTTON)
    {
        C_Axis_Abs_ZeroPos = CurrentPositionAxisC;

        uint32_t ints = save_and_disable_interrupts(); 
        printf("Zero Position Callback \n");
        uint8_t data[FLASH_PAGE_SIZE];
        data[2] = (uint8_t)(C_Axis_Abs_ZeroPos >> 8);
        data[3] = (uint8_t)(C_Axis_Abs_ZeroPos & 0xFF);
        printf("writing %d to flash\n", data[2] << 8 | data[3]);
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET, data, FLASH_PAGE_SIZE);
        restore_interrupts (ints);
    }

    if (gpio == INC_A_ZERO_BUTTON)
    {
        A_Axis_Inc_ZeroPos = CurrentPositionAxisA;
    }

    if (gpio == INC_C_ZERO_BUTTON)
    {
        C_Axis_Inc_ZeroPos = CurrentPositionAxisC;
    }
}

void Draw_Chart()
{
    display->drawLine(0, 0, 319, 0, display->colour565(255, 255, 255));
    display->drawLine(0, 0, 0, 239, display->colour565(255, 255, 255));
    display->drawLine(0, 239, 319, 239, display->colour565(255, 255, 255));
    display->drawLine(319, 0, 319, 239, display->colour565(255, 255, 255));
    display->drawLine(0, 59, 319, 59, display->colour565(255, 255, 255));
    display->drawLine(0, 119, 319, 119, display->colour565(255, 255, 255));
    display->drawLine(0, 179, 319, 179, display->colour565(255, 255, 255));
    display->drawLine(79, 59, 79, 239, display->colour565(255, 255, 255));
    display->drawLine(199, 59, 199, 239, display->colour565(255, 255, 255));
    // İstenen başlangıç koordinatları:
    int16_t startX = 5;
    int16_t startY = 115;
    const char *axis = "AXIS";
    const char *inc = "INC";
    const char *abs = "ABS";
    const char *a_axis = "A AXIS";
    const char *c_axis = "C AXIS";

    display->DrawText(axis, startX, startY, display->colour565(0, 128, 255), font_sansb12);
    display->DrawText(inc, startX, startY + 60, display->colour565(0, 128, 255), font_sansb12);
    display->DrawText(abs, startX, startY + 120, display->colour565(0, 128, 255), font_sansb12);
    display->DrawText(a_axis, startX + 90, startY, display->colour565(0, 128, 255), font_sansb12);
    display->DrawText(c_axis, startX + 210, startY, display->colour565(0, 128, 255), font_sansb12);
}

// Display A Axis Function
void Display_Axis_A(u_int16_t raw_data_A)
{   
    printf("raw_data_A: %d\n", raw_data_A);
    // calculate absolute degree of A Axis
    uint16_t abs_A = (raw_data_A - A_Axis_Abs_ZeroPos);
    if (abs_A < 0)
    {
        abs_A = 65536 + abs_A;
    }
    float abs_degree_A = (abs_A * 360.0) / 65536;

    // calculate incremental degree of A Axis
    uint16_t inc_A = (raw_data_A - A_Axis_Inc_ZeroPos);
    if (inc_A < 0)
    {
        inc_A = 65536 + inc_A;
    }
    float inc_degree_A = (inc_A * 360.0) / 65536;


    // Convert float abs_degree_A to string
    std::string abs_degree_A_String = std::to_string(abs_degree_A);
    abs_degree_A_String = abs_degree_A_String.substr(0, abs_degree_A_String.find(".") + 3); // 3 decimal places

    // Convert float inc_degree_A to string
    std::string inc_degree_A_String = std::to_string(inc_degree_A);
    inc_degree_A_String = inc_degree_A_String.substr(0, inc_degree_A_String.find(".") + 3); // 3 decimal places

    if(Last_abs_degree_A_String == abs_degree_A_String){
        return;
    }

    // Clear Value Area of INC And ABS values
    display->fillRectBoundsChanged(Last_inc_degree_A_String.c_str(), inc_degree_A_String.c_str(), 95, 175, display->colour565(0, 0, 0), font_sansb12);
    display->fillRectBoundsChanged(Last_abs_degree_A_String.c_str(), abs_degree_A_String.c_str(), 95, 235, display->colour565(0, 0, 0), font_sansb12);
    
    // Update Last Values
    Last_abs_degree_A_String = abs_degree_A_String;
    Last_inc_degree_A_String = inc_degree_A_String;

    // Display inc_degre_A_String on display
    display->DrawText(inc_degree_A_String.c_str(), 95, 175, display->colour565(255, 255, 255), font_sansb12);

    // Display abs_degree_A_String on display
    display->DrawText(abs_degree_A_String.c_str(), 95, 235, display->colour565(255, 255, 255), font_sansb12);

}

void PrepareFlash()
{   
    uint32_t ints = save_and_disable_interrupts();
    //erase flash
    uint16_t data_a = (FlashTargetContents[0] << 8) | FlashTargetContents[1];
    uint16_t data_b = (FlashTargetContents[2] << 8) | FlashTargetContents[3];
    if (data_a == 0xFFFF)
    {
        printf("Flash Not Programmed before setting up 0\n");
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
        uint8_t data[FLASH_PAGE_SIZE];
        data[0] = 0x00;
        data[1] = 0x00;
        flash_range_program(FLASH_TARGET_OFFSET, data, FLASH_PAGE_SIZE);
        A_Axis_Abs_ZeroPos = 0;
    }
    else 
    {
        printf("Flash Programmed before value %d\n", data_a);
        A_Axis_Abs_ZeroPos = data_a;
    }
    if (data_b == 0xFFFF)
    {
        printf("Flash Not Programmed before setting up 0\n");
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_PAGE_SIZE);
        uint8_t data[FLASH_PAGE_SIZE];
        data[2] = 0x00;
        data[3] = 0x00;
        flash_range_program(FLASH_TARGET_OFFSET, data, FLASH_PAGE_SIZE);
        C_Axis_Abs_ZeroPos = 0;
    }
    else 
    {
        printf("Flash Programmed before value %d\n", data_b);
        C_Axis_Abs_ZeroPos = data_b;
    }
    restore_interrupts (ints);
    return;
}

int main()
{
    
    // set LCD_BACKLIGHT as output
    gpio_init(LCD_BACKLIGHT);
    gpio_set_dir(LCD_BACKLIGHT, GPIO_OUT);
    gpio_put(LCD_BACKLIGHT, 1);

    // set up serial
    stdio_init_all();

    spi_init(SPI_PORT,900*1000);
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
    display->clear(display->colour565(0, 0, 0));
    Draw_Chart();
    PrepareFlash();

    AEAT6600 *encoder_a_axis = new AEAT6600(
        A_AXIS_CLOCK_PIN,
        A_AXIS_DATA_PIN,
        A_AXIS_CS_PIN,
        BIT_COUNT,
        A_AXIS_MAGNETIC_FIELD_LOW_PIN,
        A_AXIS_MAGNETIC_FIELD_HIGH_PIN,
        A_AXIS_PROGRAMMING_PIN);

    encoder_a_axis->Init();

    AEAT6600 *encoder_c_axis = new AEAT6600(
        C_AXIS_CLOCK_PIN,
        C_AXIS_DATA_PIN,
        C_AXIS_CS_PIN,
        BIT_COUNT,
        C_AXIS_MAGNETIC_FIELD_LOW_PIN,
        C_AXIS_MAGNETIC_FIELD_HIGH_PIN,
        C_AXIS_PROGRAMMING_PIN);

    encoder_c_axis->Init();

    // set ABS_A_ZERO_BUTTON as input interrupt
    gpio_init(ABS_A_ZERO_BUTTON);
    gpio_set_dir(ABS_A_ZERO_BUTTON, GPIO_IN);
    gpio_pull_up(ABS_A_ZERO_BUTTON);
    gpio_set_irq_enabled_with_callback(ABS_A_ZERO_BUTTON, GPIO_IRQ_EDGE_FALL, true, &IncAbsZeroPositionCallback);

    // set INC_A_ZERO_BUTTON as input interrupt
    gpio_init(INC_A_ZERO_BUTTON);
    gpio_set_dir(INC_A_ZERO_BUTTON, GPIO_IN);
    gpio_pull_up(INC_A_ZERO_BUTTON);
    gpio_set_irq_enabled_with_callback(INC_A_ZERO_BUTTON, GPIO_IRQ_EDGE_RISE, true, &IncAbsZeroPositionCallback);

    // set ABS_C_ZERO_BUTTON as input interrupt
    gpio_init(ABS_C_ZERO_BUTTON);
    gpio_set_dir(ABS_C_ZERO_BUTTON, GPIO_IN);
    gpio_pull_up(ABS_C_ZERO_BUTTON);
    gpio_set_irq_enabled_with_callback(ABS_C_ZERO_BUTTON, GPIO_IRQ_EDGE_RISE, true, &IncAbsZeroPositionCallback);

    // set INC_C_ZERO_BUTTON as input interrupt
    gpio_init(INC_C_ZERO_BUTTON);
    gpio_set_dir(INC_C_ZERO_BUTTON, GPIO_IN);
    gpio_pull_up(INC_C_ZERO_BUTTON);
    gpio_set_irq_enabled_with_callback(INC_C_ZERO_BUTTON, GPIO_IRQ_EDGE_RISE, true, &IncAbsZeroPositionCallback);

  
    while (true)
    {
        // Read A Axis Encoder
        CurrentPositionAxisA = encoder_a_axis->Read();

        
        // Display A Axis
        Display_Axis_A(CurrentPositionAxisA);
        sleep_ms(100);
    }

    return 0;
}
