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
 *  Main source file for the GenericHID demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "Unison.h"

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevGenericHIDReportBuffer[GENERIC_REPORT_SIZE];

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevPadOneHIDReportBuffer[sizeof(USB_Pad_Report_Data_t)];

/** Buffer to hold the previously generated HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevPadTwoHIDReportBuffer[sizeof(USB_Pad_Report_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Generic_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_GenericHID,
				.ReportINEndpoint             =
					{
						.Address              = GENERIC_IN_EPADDR,
						.Size                 = GENERIC_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevGenericHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevGenericHIDReportBuffer),
			},
	};

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t PadOne_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_PadOne,
				.ReportINEndpoint             =
					{
						.Address              = PAD_ONE_EPADDR,
						.Size                 = PAD_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevPadOneHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevPadOneHIDReportBuffer),
			},
	};

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t PadTwo_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_PadTwo,
				.ReportINEndpoint             =
					{
						.Address              = PAD_TWO_EPADDR,
						.Size                 = PAD_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevPadTwoHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevPadTwoHIDReportBuffer),
			},
	};

uint32_t old_millis = 0;

/** Main program entry point. */
int main(void)
{
	SetupHardware();

	GlobalInterruptEnable();

	for (;;)
	{
		if((millis() - old_millis) > 100)
		{
			PORTB ^= 1 << 0; //Switch RXLED
			old_millis = millis();
		}

		HID_Device_USBTask(&Generic_HID_Interface);
		HID_Device_USBTask(&PadOne_HID_Interface);
		HID_Device_USBTask(&PadTwo_HID_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Init milliseconds counter */
	millis_init();

	/* Configure Data Direction Registers */
	DDRB = 0xFF;
	DDRC = 0x40;
	DDRD = 0xE7;
	DDRE = 0x00;
	DDRF = 0xC0;

	/* Hardware Initialization */
	USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Generic_HID_Interface);
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&PadOne_HID_Interface);
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&PadTwo_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
	HID_Device_ProcessControlRequest(&PadOne_HID_Interface);
	HID_Device_ProcessControlRequest(&PadTwo_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
	HID_Device_MillisecondElapsed(&PadOne_HID_Interface);
	HID_Device_MillisecondElapsed(&PadTwo_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	if(HIDInterfaceInfo == &Generic_HID_Interface) 
	{
		return false;
	}
	else if (HIDInterfaceInfo == &PadOne_HID_Interface)
	{
		USB_Pad_Report_Data_t* PadReport = (USB_Pad_Report_Data_t*)ReportData;

		PadReport->Button |= (1 << 1);

		*ReportSize = sizeof(USB_Pad_Report_Data_t);
		return false;
	}
	else
	{
		USB_Pad_Report_Data_t* PadReport = (USB_Pad_Report_Data_t*)ReportData;

		PadReport->Button |= (1 << 0);

		*ReportSize = sizeof(USB_Pad_Report_Data_t);
		return false;
	}
	
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
	if(HIDInterfaceInfo == &Generic_HID_Interface) {
		//uint8_t* Data       = (uint8_t*)ReportData;

		/**
		if (Data[0])
		  	PORTB = 0x01; //Turn on  RXLED
		else
		  	PORTB = 0x00; //Turn off RXLED
		  	**/
	}
	
}

