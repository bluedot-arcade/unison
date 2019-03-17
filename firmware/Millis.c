#include "Millis.h"

static volatile uint32_t milliseconds;

ISR(TIMER0_COMPA_vect)
{
	++milliseconds;
}

void millis_init() 
{							
	TCCR0A |= (1 << WGM01); 				//Enable CTC Mode
	TCCR0B |= (1 << CS01) | (1 << CS00);	//Set Prescaler to 64
	TCNT0 = 0; 								//Init timer counter
	OCR0A = (F_CPU / PRESCALER) / 1000;		//Setup output compare register 
	TIMSK0 = (1 << OCIE0A);					//Enable output compare interrupt
	sei();									//Enable global interrupt
}

uint32_t millis() 
{
	uint32_t ms;
	uint8_t oldSREG = SREG;

	//Disable interrupts while reading milliseconds to avoid getting an inconsistent value
	cli();
	ms = milliseconds;
	SREG = oldSREG;
	return ms;
}

void millis_reset() {
	uint8_t oldSREG = SREG;

	//Disable interrupts whileresetting milliseconds
	cli();
	milliseconds = 0;
	SREG = oldSREG;
}


