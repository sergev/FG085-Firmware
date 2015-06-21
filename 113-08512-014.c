//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	113-08512-014.c
//	Version:		0.14
//	Data:		2013.10.28
//
//	Author:		Liu, Zemin
//	Company:	JYE Tech Ltd.
//
//-----------------------------------------------------------------------------
//
// 	Target: 		ATmega168
// 	Tool chain: 	WinAvr (GCC-AVR)
//
//
//-----------------------------------------------------------------------------
//	Required files:
//
//-----------------------------------------------------------------------------
//	Notes:
//	ATmega48 fuses configuration:
//			  1)	Extended Byte = 0b-----001 = 0x01
//				Bit [2:1] 	: Select boot size (ATmega88/168 only)
//				
//				Bit 0		: (ATmega48) Self programming enable [SELFPRGEN(1)] 
//						: (ATmega88/168) Select reset vector [BOOTRST(1)] 
//							1 = Application reset, address = 0x0000
//							0 = Boot loader reset
//
//			  2) 	High Byte = 0b11010110 = 0xD6
//				Bit 7		: External Reset Disable [RSTDISBL(1)]
//							1 = External reset enabled
//				Bit 6		: DebugWIRE enable [DWEN(1)]
//				Bit 5		: Serial Programming enable [SPIEN(0)]
//				Bit 4		: WDT always on [WDTON(1)]
//							1 = WDT behaviour determined by WDE & WDIE in WDTSCR
//							0 = WDT on and works in System Reset Mode
//				Bit 3		: EEPROM Preserved [EESAVE(1)]
//							1 = EEPROM not reserved
//				Bit [2:0]	: BOD trig level [BODLEVEL[2:0] (111)]
//							000 = Reserved
//							001 = Reserved
//							010 = Reserved
//							011 = Reserved
//							100 = 4.3V (typical)
//							101 = 2.7V (typical)
//							110 = 1.8V (typical)
//							111 = Disabled
//
//			  2) 	Low Byte = 0b11100110 = 0XE0
//				Bit 7		: Divide Clock by 8 [CKDIV8 (0)]
//							0 = Clock divided by 8 (CLKPR[3:0] set to 0011)
//							1 = No division (CLKPR[3:0] set to 0000)
//				Bit 6		: Clock Output [CKOUT = 1]
//							0 = Enable clock output at CLKO pin
//							1 = Disable clock output
//				Bit [5:4]	: Select start-up time [SUT[1:0](10)]
//							See datasheet
//				Bit [3:0]	: Clock source select [CKSEL[3:0](0010)]
//							0000 = External clock
//							0001 = Reserved
//							0010 = Calibrated internal RC oscillator (8MHz)
//							0011 = Internal 128KHz RC oscillator
//							0100-0101 = Low Frequency Crystal Osc
//							0110-0111 = Full Swing Crystal Osc
//							1000-1111 = Low Power Crystal Osc
//-----------------------------------------------------------------------------
//	Revision History:
//
//		V0.10		2013.07.28
//		1. 	Added the feature of receiving button codes from serial port.
//
//		V0.11		2013.08.16
//		1. 	Removed "NoCodeLoss" command and substituted it with special button code 0xA1.
//
//		V0.12		2013.09.04
//		1. 	Fixed a bug which caused unit buttons (Hz and KHz) not function properly when
//			button codes are received from serial port. Each time a button code is received it
//			must be kept a copy in the variable 'KeyCodeBuf'. This copy is used to determine
//			how the entered digits be handled.
//
//		V0.13		2013.10.07
//		1. 	Added a special key code to place cursor to intended location.
//
//		V0.14		2013.10.28
//		1. 	Added a special key code to set waveform directly.
//		2.	Fix a bug that limits amplitude from being set to 0
//
///////////////////////////////////////////////////////////////////////////////

#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>

#include 	"Common.h"
#include 	"Board.h"
#include 	"FuncGen.h"
#include 	"Diag.h"
#include 	"Comm.h"


// ===========================================================
//	File Scope Variables
// ===========================================================
//

// ===========================================================
//	External Variables
// ===========================================================
//

PROGMEM	U8	Device[15] = {"FG085  miniDDS"};
PROGMEM	U8	FWver[16] = {"Main: 08512-014"};
PROGMEM	U8	DDSver[12] = {"DDS: 08502-"};
PROGMEM	U8	MfgID[18] = {"JYE Tech (c)2013"};
PROGMEM	U8	Web[17] = {"www.jyetech.com "};
PROGMEM	U8	CRLF[4] = {"\r\n"};
PROGMEM	U8	Decor[17] = {"Signal Generator"};	

int	main (void)
{
 U8	tmp;
 U8 	*ptmp;
 
 Port_Init();

 // -- Initialize misc registers
 Misc_Init();

 LCD_Init();
 
 // Display device
 FillDisp(0, 32, ' ');
 PutStr_ROM(0, (U8 *)Device);
 PutStr_ROM(16, (U8 *)Decor);
 UpdateLCD('0');
 DelayDisp('u');
 
 // -- Initialize Uart 0
// Uart0_Init();
 CommInit();
 uputs_ROM((U8*)CRLF);
 uputs_ROM((U8*)Device);
 uputs_ROM((U8*)CRLF);
 uputs_ROM((U8*)FWver);
 uputs_ROM((U8*)CRLF);
 uputs_ROM((U8*)MfgID);
 uputs_ROM((U8*)CRLF);
 uputs_ROM((U8*)Web);
 
 Timer0_Init();
// Timer1_Init();
 Timer2_Init();
// ADC_Init();
// Comp_Init();

// ExtIntr_Init();

 // Display Logo
 ClearDisp();
 ptmp = (U8 *)DispBuf;
 *(ptmp + 32) = 0;
 *(ptmp + 33) = 0;
 *(ptmp + 34) = 0;
 tmp = EE_Read(EE_VendorID);
 if(tmp == 0xC2) {						// 0xC2 == ~0x3D ('=')
 	// Read vendor ID from EEPROM
	tmp = 0;
 	while(tmp < 32) {
		*ptmp++ = ~(EE_Read(EE_VendorID + 1 + tmp));
		tmp++;
 		}
	*ptmp = 'l';
 	}
 else {
	// Display manufacturer ID
	PutStr_ROM_coded1(0, (U8 *)MfgID_coded1);
//	PutStr_ROM(0, (U8 *)MfgID);
	PutStr_ROM_coded1(16, (U8 *)Web_coded1);
//	PutStr_ROM(16, (U8 *)Web);
 	}
 DispBuf[33] = UpdateLCD(DispBuf[32]);	// This set DispBuf[33] to 'z' if DispBuf[32] == 'l'
 DispBuf[34] = DelayDisp(DispBuf[32]);		// This should set DispBuf[34] to 0

/*
 // Save sample user waveform
 tmp = 0;
 while(tmp < 255) {
 	EE_Write(EE_UserWaveform + tmp, tmp);
	tmp++;
 	}
 EE_Write(EE_UserWaveform + 255, 0x80);
*/

 // Display firmware versions
 ClearDisp();
 PutStr_ROM(0, (U8 *)FWver);
 PutStr_ROM(16, (U8 *)DDSver);
 // Send command to fetch DDS core version
 Strbuf[0] = KS_FetchDDSver;
 SendBuf((U8*)Strbuf);
 
 // Receive result (3 bytes)
 tmp = 0; 
 GTimer = 40000;
 while(GTimer) {
	if(!BitTest8(COMM_RXD_Port, (1 << COMM_RXD_Bit))) {
		// Detected low at RXD Pin
		DispBuf[27 + tmp] = RecvByte();
//		u0putc(DispBuf[27 + tmp]);
		tmp++;
		GTimer = 40000;
		if(tmp > 2) {
			break;
			}
		}
 	}
 
 // Display result
 UpdateLCD('A');
 DelayDisp('w');

 if((DispBuf[32] == 'l') && (DispBuf[33] == 'z') && (DispBuf[34] == 'm')) {
 	// Disable EE_counter
 	DispBuf[34] = 0;
// 	PutStr_ROM_coded();
 	}

/*
 while(1) {
 	BitSet8(PORTD, (1 << PD3));
 	Delay(200);
 	BitClr8(PORTD, (1 << PD3));
 	Delay(200);
 	}
 */
 
 // Start application
 AppInit();
 
 sei();

 while(1) {	
 	if(BitTest8(KFlags, (1<<KScanRdy))) {
//		BitSet8(PIND, (1<<PIND2));
		KeyScan();
		BitClr8(KFlags, (1<<KScanRdy));
 		}

	// -- Key command processing
	if(KeyCode) {
		DoCmdProcessing();
		}

	// Update test SIGNAL
	if((SysState.CurrState == KS_DiagDDS) && (CurrMode == 0)) {
		if(GTimer == 0) {
			CurrFocus++;
			if(CurrFocus >= TestSignalMax) {
				CurrFocus = 0;
				}
			OutputTestSignal();
			GTimer = TestSignalDuration;
			}
		}

	CommRx();
	
	// Update LCD
	if(BitTest8(Flags, (1 << Flags_DispUpdate))) {
		BitClr8(Flags, (1 << Flags_DispUpdate));
		UpdateLCD(',');
		}

//	SendByte(tmp++);
 	}

 return (0);
}

