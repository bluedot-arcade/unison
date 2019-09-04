#ifndef _HC595_H_
#define _HC595_H_

    /* Includes: */
        #include <avr/io.h>

    /* Macros: */
        #define HC595_PORT      PORTB
        #define HC595_DDR       DDRB
        #define HC595_SER_PIN   PB7
        #define HC595_RCLK_PIN  PB5
        #define HC595_SRCLK_PIN PB6

    /* Function Prototypes: */
        void HC595_Init(void);

        void HC595_Pulse(void);

        void HC595_Latch(void);

        void HC595_Write(uint16_t data);

#endif /* _HC595_H_ */

