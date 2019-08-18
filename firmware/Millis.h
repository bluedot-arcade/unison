#ifndef _MILLIS_H_
#define _MILLIS_H_

	/* Includes: */
		#include <avr/io.h>
	    #include <avr/interrupt.h>
		#include <util/atomic.h>

	/* Macros: */
		/* Use timer 0 */
		#define PRESCALER 64

	/* Function Prototypes: */
		/** 
		 * Initialise millis timer
		 */
		void Millis_Init(void);

		/**
		 * Get current milliseconds.
		 *
         * @return milliseconds as uint32_t.
		 */
		uint32_t Millis(void);

		/**
		 * Reset milliseconds counter to 0.
		 */
		void Millis_Reset(void);

#endif /* _MILLIS_H_ */