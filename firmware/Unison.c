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
static uint8_t PrevPadHIDReportBuffer[sizeof(USB_Pad_Report_Data_t)];

/** Buffer to hold pad one's buttons status **/
static uint16_t PadOneButtonStatus = 0;

/** Buffer to hold pad two's buttons status **/
static uint16_t PadTwoButtonStatus = 0;

/** Buffer to hold pad one's lights status **/
static uint8_t PadOneLightStatus = 0;

/** Buffer to hold pad two's lights status **/
static uint8_t PadTwoLightStatus = 0;

/** Buffer to hold cabinet lights status **/
static uint16_t CabinetLightStatus = 0;

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
USB_ClassInfo_HID_Device_t Pad_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = INTERFACE_ID_Pad,
				.ReportINEndpoint             =
					{
						.Address              = PAD_EPADDR,
						.Size                 = PAD_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevPadHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevPadHIDReportBuffer),
			},
	};

/** Main program entry point. */
int main(void)
{
	SetupHardware();

	GlobalInterruptEnable();

	for (;;)
	{
		CheckInputsTask();
		HID_Device_USBTask(&Generic_HID_Interface);
		HID_Device_USBTask(&Pad_HID_Interface);
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable JTAG. Must be done twice to take effect! */
	MCUCR |= (1 << JTD);
	MCUCR |= (1 << JTD);

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

	/* Init milliseconds counter */
	Millis_Init();

	/* Init Shift register */
	HC595_Init();

	/* Configure Data Direction Registers */
	DDRB = 0xFF;
	DDRC = 0x40;
	DDRD = 0xE7;
	DDRE = 0x00;
	DDRF = 0xC0;

	/* Hardware Initialization */
	USB_Init();
}

void CheckInputsTask(void) 
{
	//Check sensors
		for(uint8_t i = 0; i < 5; i++) 
		{
			if(~PINF & 0x33)
			{
				//Set button
				PadOneButtonStatus |= (1 << i);
			} 
			else 
			{
				//Clear button
				PadOneButtonStatus &= ~(1 << i);
			}

			//Next mux input
			PORTD += 1;
		}

		//Reset mux selection to 0
		PORTD &= ~0x07;
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
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Pad_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Generic_HID_Interface);
	HID_Device_ProcessControlRequest(&Pad_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Generic_HID_Interface);
	HID_Device_MillisecondElapsed(&Pad_HID_Interface);
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
		return true;
	}
	else if (HIDInterfaceInfo == &Pad_HID_Interface)
	{
		USB_Pad_Report_Data_t* PadReport = (USB_Pad_Report_Data_t*)ReportData;

		PadReport->Button = PadOneButtonStatus;

		*ReportSize = sizeof(USB_Pad_Report_Data_t);
		return true;
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

	uint8_t* Data = (uint8_t*)ReportData;

	if(HIDInterfaceInfo == &Generic_HID_Interface) 
	{
		//Check packet type. First byte is always the packet id.
		if (Data[0] == 0x02) 
		{
			//Set Lights Command
			HandleSetLightsPacket(Data + 1);
		} 
		else if (Data[0] == 0xFF) 
		{
			//Enter Bootloader Command
			HandleJumpToBootloaderPacket(Data + 1);
		}
	}
}

void HandleSetLightsPacket(uint8_t* Data) 
{
	/** Turn off all pad and cabinet lights **/
	PadOneLightStatus = 0x00;
	PadTwoLightStatus = 0x00;
	CabinetLightStatus = 0x00;

	//TODO Check if it is P1 or P2

	/* Update P1 pad lights (left side) */
	if(Data[LIGHT_P1_BTN_CUSTOM_01_BYTE] & LIGHT_P1_BTN_CUSTOM_01_MASK) PadOneLightStatus |= 0x01;
	if(Data[LIGHT_P1_BTN_CUSTOM_02_BYTE] & LIGHT_P1_BTN_CUSTOM_02_MASK) PadOneLightStatus |= 0x02;
	if(Data[LIGHT_P1_BTN_CUSTOM_03_BYTE] & LIGHT_P1_BTN_CUSTOM_03_MASK) PadOneLightStatus |= 0x04;
	if(Data[LIGHT_P1_BTN_CUSTOM_04_BYTE] & LIGHT_P1_BTN_CUSTOM_04_MASK) PadOneLightStatus |= 0x08;
	if(Data[LIGHT_P1_BTN_CUSTOM_05_BYTE] & LIGHT_P1_BTN_CUSTOM_05_MASK) PadOneLightStatus |= 0x10;

	/* Update P2 pad lights (right side) */
	if(Data[LIGHT_P2_BTN_CUSTOM_01_BYTE] & LIGHT_P2_BTN_CUSTOM_01_MASK) PadTwoLightStatus |= 0x01;
	if(Data[LIGHT_P2_BTN_CUSTOM_02_BYTE] & LIGHT_P2_BTN_CUSTOM_02_MASK) PadTwoLightStatus |= 0x02;
	if(Data[LIGHT_P2_BTN_CUSTOM_03_BYTE] & LIGHT_P2_BTN_CUSTOM_03_MASK) PadTwoLightStatus |= 0x04;
	if(Data[LIGHT_P2_BTN_CUSTOM_04_BYTE] & LIGHT_P2_BTN_CUSTOM_04_MASK) PadTwoLightStatus |= 0x08;
	if(Data[LIGHT_P2_BTN_CUSTOM_05_BYTE] & LIGHT_P2_BTN_CUSTOM_05_MASK) PadTwoLightStatus |= 0x10;

	/* Update P1 cab lights (left side) */
	if(Data[LIGHT_MARQUEE_UP_LEFT_BYTE]   & LIGHT_MARQUEE_UP_LEFT_MASK)  CabinetLightStatus |= 0x8000;
	if(Data[LIGHT_MARQUEE_LR_LEFT_BYTE]   & LIGHT_MARQUEE_LR_LEFT_MASK)  CabinetLightStatus |= 0x4000;
	if(Data[LIGHT_BASS_LEFT_BYTE] 		  & LIGHT_BASS_LEFT_MASK) 		 CabinetLightStatus |= 0x2000;
	if(Data[LIGHT_P1_BTN_MENULEFT_BYTE]   & LIGHT_P1_BTN_MENULEFT_MASK)  CabinetLightStatus |= 0x1000;
	if(Data[LIGHT_P1_BTN_MENURIGHT_BYTE]  & LIGHT_P1_BTN_MENURIGHT_MASK) CabinetLightStatus |= 0x0800;
	if(Data[LIGHT_P1_BTN_MENUUP_BYTE]     & LIGHT_P1_BTN_MENUUP_MASK)	 CabinetLightStatus |= 0x0400;
	if(Data[LIGHT_P1_BTN_MENUDOWN_BYTE]   & LIGHT_P1_BTN_MENUDOWN_MASK)	 CabinetLightStatus |= 0x0200;
	if(Data[LIGHT_P1_BTN_START_BYTE] 	  & LIGHT_P1_BTN_START_MASK)	 CabinetLightStatus |= 0x0100;

	/* Update P2 cab lights (right side) */
	if(Data[LIGHT_MARQUEE_UP_RIGHT_BYTE]  & LIGHT_MARQUEE_UP_RIGHT_MASK) CabinetLightStatus |= 0x0080;
	if(Data[LIGHT_MARQUEE_LR_RIGHT_BYTE]  & LIGHT_MARQUEE_LR_RIGHT_MASK) CabinetLightStatus |= 0x0040;
	if(Data[LIGHT_BASS_RIGHT_BYTE] 		  & LIGHT_BASS_RIGHT_MASK) 		 CabinetLightStatus |= 0x0020;
	if(Data[LIGHT_P2_BTN_MENULEFT_BYTE]   & LIGHT_P2_BTN_MENULEFT_MASK)  CabinetLightStatus |= 0x0010;
	if(Data[LIGHT_P2_BTN_MENURIGHT_BYTE]  & LIGHT_P2_BTN_MENURIGHT_MASK) CabinetLightStatus |= 0x0008;
	if(Data[LIGHT_P2_BTN_MENUUP_BYTE]     & LIGHT_P2_BTN_MENUUP_MASK)	 CabinetLightStatus |= 0x0004;
	if(Data[LIGHT_P2_BTN_MENUDOWN_BYTE]   & LIGHT_P2_BTN_MENUDOWN_MASK)	 CabinetLightStatus |= 0x0002;
	if(Data[LIGHT_P2_BTN_START_BYTE] 	  & LIGHT_P2_BTN_START_MASK)	 CabinetLightStatus |= 0x0001;

	/* Update pad lights */
	UpdatePadLights(PadOneLightStatus);

	/* Update cab lights */
	UpdateCabinetLights(CabinetLightStatus);
}

void HandleTurnOnLightsPacket(uint8_t* Data)
{
	
}

void HandleTurnOffLightsPacket(uint8_t* Data)
{
	
}

void HandleJumpToBootloaderPacket(uint8_t* Data)
{
	Jump_To_Bootloader();
}

void UpdatePadLights(uint8_t Status)
{
	//Update light dance LEFT / pump UP_LEFT
	if(Status & 0x01) 
		TURN_ON_LIGHT_LEFT();
	else
		TURN_OFF_LIGHT_LEFT();

	//Update light dance RIGHT / pump UP_RIGHT
	if(Status & 0x02)
		TURN_ON_LIGHT_RIGHT();
	else 
		TURN_OFF_LIGHT_RIGHT();

	//Update light dance UP / pump CENTER
	if(Status & 0x04)
		TURN_ON_LIGHT_UP();
	else
		TURN_OFF_LIGHT_UP();

	//Update light dance DOWN / pump DOWN_LEFT
	if(Status & 0x08)
		TURN_ON_LIGHT_DOWN();
	else
		TURN_OFF_LIGHT_DOWN();

	//Update light pump DOWN_RIGHT	
	if(Status & 0x10)
		TURN_ON_LIGHT_DOWN_RIGHT();
	else
		TURN_OFF_LIGHT_DOWN_RIGHT();
}

void UpdateCabinetLights(uint16_t Status) 
{
	/* Update cab lights shift registers */
	HC595_Write(Status);
}


