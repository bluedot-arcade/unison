/*
             LUFA Library
     Copyright (C) Dean Camera, 2017.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2017  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Header file for Unison.c.
 */

#ifndef _UNISON_H_
#define _UNISON_H_

	/* Includes: */
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <util/delay.h>
		#include <string.h>

		#include "Descriptors.h"
		#include "Config/AppConfig.h"
		#include "Millis.h"
		#include "Bootloader.h"
		#include "Lights.h"
		#include "HC595.h"

		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Platform/Platform.h>

	/* Macros: */
		#define TURN_ON_LIGHT_LEFT() 		 PORTC |=  (1 << PC6)
		#define TURN_ON_LIGHT_RIGHT()		 PORTD |=  (1 << PD7)
		#define TURN_ON_LIGHT_UP()		     PORTF |=  (1 << PF6)
		#define TURN_ON_LIGHT_DOWN()         PORTF |=  (1 << PF7)
		#define TURN_ON_LIGHT_UP_LEFT()      PORTF |=  (1 << PF6)
		#define TURN_ON_LIGHT_UP_RIGHT()     PORTF |=  (1 << PF7)
		#define TURN_ON_LIGHT_CENTER()       PORTD |=  (1 << PD6)
		#define TURN_ON_LIGHT_DOWN_LEFT()    PORTC |=  (1 << PC6)
		#define TURN_ON_LIGHT_DOWN_RIGHT()   PORTD |=  (1 << PD7)

		#define TURN_OFF_LIGHT_LEFT() 		 PORTC &= ~(1 << PC6)
		#define TURN_OFF_LIGHT_RIGHT()		 PORTD &= ~(1 << PD7)
		#define TURN_OFF_LIGHT_UP()		     PORTF &= ~(1 << PF6)
		#define TURN_OFF_LIGHT_DOWN()        PORTF &= ~(1 << PF7)
		#define TURN_OFF_LIGHT_UP_LEFT()     PORTF &= ~(1 << PF6)
		#define TURN_OFF_LIGHT_UP_RIGHT()    PORTF &= ~(1 << PF7)
		#define TURN_OFF_LIGHT_CENTER()      PORTD &= ~(1 << PD6)
		#define TURN_OFF_LIGHT_DOWN_LEFT()   PORTC &= ~(1 << PC6)
		#define TURN_OFF_LIGHT_DOWN_RIGHT()  PORTD &= ~(1 << PD7)

		#define MUX_PORT PORTD
		#define MUX_S0 	 PD0
		#define MUX_S1   PD1
		#define MUX_S2   PD2
		#define MUX_PIN  PINF
		#define MUX_A1	 PF0
		#define MUX_A2   PF1
		#define MUX_A3   PF4
		#define MUX_A4   PF5

	/* Type Defines: */
		
		/** Type define for the gamepad HID report structure, for creating and sending HID reports to the host PC.
		 *  This mirrors the layout described to the host in the HID report descriptor, in Descriptors.c.
		 */
		typedef struct
		{
			uint16_t Button; /**< Bit mask of the currently pressed gamepad buttons */
		} USB_Pad_Report_Data_t;

	/* Function Prototypes: */
		void Setup_Hardware(void);
		void Poll_Inputs(void);

		void EVENT_USB_Device_Connect(void);
		void EVENT_USB_Device_Disconnect(void);
		void EVENT_USB_Device_ConfigurationChanged(void);
		void EVENT_USB_Device_ControlRequest(void);
		void EVENT_USB_Device_StartOfFrame(void);

		bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                         uint8_t* const ReportID,
		                                         const uint8_t ReportType,
		                                         void* ReportData,
		                                         uint16_t* const ReportSize);
		void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
		                                          const uint8_t ReportID,
		                                          const uint8_t ReportType,
		                                          const void* ReportData,
		                                          const uint16_t ReportSize);

		void Handle_JumpToBootloader_Packet(uint8_t* Data);
		void Handle_SetLights_Packet(uint8_t* Data);
		void Handle_TurnOnLights_Packet(uint8_t* Data);
		void Handle_TurnOffLights_Packet(uint8_t* Data);

		/** Update current board pad lights */
		void Update_Pad_Lights(uint8_t Status);

		/** Update cabinet lights */
		void Update_Cabinet_Lights(uint16_t Status);
	
#endif /* _UNISON_H_ */

