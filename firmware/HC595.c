#include "HC595.h"

void HC595_Init() 
{
    // Set SER, RCLK and SRCLK outputs
    HC595_DDR |= (1 << HC595_SER_PIN | 1 << HC595_RCLK_PIN | 1 << HC595_SRCLK_PIN);
}

void HC595_Pulse() 
{
    //Sends a clock pulse on the shift register clock pin
    HC595_PORT |=  (1 << HC595_SRCLK_PIN);
    HC595_PORT &= ~(1 << HC595_SRCLK_PIN);
}

void HC595_Latch() 
{
    //Sends a clock pulse on the storage register clock pin
    HC595_PORT |=  (1 << HC595_RCLK_PIN);
    HC595_PORT &= ~(1 << HC595_RCLK_PIN);
}

void HC595_Write(uint16_t data) 
{
    //Serially sends each data bit

    for(uint8_t i = 0; i < 16; i++) 
    {
        if(data & (1 << i))
            HC595_PORT |=  (1 << HC595_SER_PIN); 
        else
            HC595_PORT &= ~(1 << HC595_SER_PIN);

        //Pulse shift register clock
        HC595_Pulse();
    }

    //Move data to the output latch
    HC595_Latch();
}