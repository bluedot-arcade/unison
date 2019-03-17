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
		void millis_init(void);

		/**
		 * Get current milliseconds.
		 *
         * @return milliseconds as uint32_t.
		 */
		uint32_t millis(void);

		/**
		 * Reset milliseconds counter to 0.
		 */
		void millis_reset(void);

#endif /* _MILLIS_H_ */