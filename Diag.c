//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Diag.c
//	Version:		
//	Data:		2010.09.05
//
//	Author:		Liu, Zemin
//	Company:	JYE Tech
//
//-----------------------------------------------------------------------------
//
// 	Target: 		ATmega64
// 	Tool chain: 	WinAvr (GCC-AVR)
//
//
//-----------------------------------------------------------------------------
//	Required files:
//
//-----------------------------------------------------------------------------
//	Notes:
//
//
//-----------------------------------------------------------------------------
//	Revision History:
//		V0.01		2010.09.05
//		1. First created.
//
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// 	Includes
//-----------------------------------------------------------------------------
#include "avr/io.h"
#include	"avr/Pgmspace.h"
#include <avr/interrupt.h>
#include	"Common.h"
#include	"Board.h"
#include	"FuncGen.h"
#include	"Diag.h"
#include 	"Comm.h"


// ==========================================================
//	File Scope Variables
// ==========================================================
//
PROGMEM	U8	DummyString1[16] = {"0123456789ABCDEF"};
PROGMEM	U8	DummyString2[16] = {"[ESC] to exit"};

PROGMEM	U8	DummyString3[5] = {"AUTO"};
PROGMEM	U8	EepromCheck[16] = {"DDS EEPROM Test"};
PROGMEM	U8	Good[5] = {"Good"};
PROGMEM	U8	Failed[7] = {"Failed"};

PROGMEM	U8	TestSignal[TestSignalMax][17] = {
	{"1KHz     5V   0V"},
	{"1KHz    10V   0V"},
	{"1KHz    10V   5V"},
	{"1KHz    10V  -5V"},
	{"10KHz    5V   0V"},
	{"100KHz   5V   0V"},
	{"200KHz   5V   0V"}
	};

PROGMEM	U32	TestSignalParam[TestSignalMax * 3] = {
	1000, 5000, 5000,
	1000, 10000, 5000,
	1000, 10000, 10000,
	1000, 10000, 0,
	10000, 5000, 5000,
	100000, 5000, 5000,
	200000, 5000, 5000
	};


// ==========================================================
//	Function Definitions
// ==========================================================
//
void	DiagDisplay(void)
{
 // Clear display buffer
 ClrDispBuf();

 // Show given contents
 PutStr_ROM(0, DummyString1);
 PutStr_ROM(16, DummyString2);
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	DiagKeypad(void)
{
 // Clear display buffer
 ClrDispBuf();
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	DiagKeyCheck(void)
{
 if(KeyCode < KC_CW) {
 	DispBuf[KeyCode - 1] = KeyParam;
 	}
 else if(KeyCode == KC_CW) {
	DispBuf[22] = 'C';
	DispBuf[23] = 'W';
 	}
 else if(KeyCode == KC_CCW) {
	DispBuf[25] = 'C';
	DispBuf[26] = 'C';
	DispBuf[27] = 'W';
 	}
 
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	OutputTestSignal(void)
{
 U32	tmp;

 // Ensure trig control is turned off
 TrigCtrl = '>';
 
 // Set frequency
 miniDDS.Freq = pgm_read_dword((U32 *)(TestSignalParam + CurrFocus*3));
 miniDDS.SetType = SetType_Freq;
 SendDDS();

 // Set amplitude
 tmp = pgm_read_dword((U32 *)(TestSignalParam + CurrFocus*3 + 1));
 SetDigitalPot((U8)((tmp * 100)/AMPmax)); 

 // Set offset
 tmp = pgm_read_dword((U32 *)(TestSignalParam + CurrFocus*3) + 2);
 UpdateOfs(tmp);

 ClearDisp();
 DispBuf[0] = CurrFocus | 0x30;
 if(CurrMode == 0) {
	// Show "AUTO"
	PutStr_ROM(12, (U8 *)DummyString3);
 	}
 PutStr_ROM(16, (U8 *)TestSignal[CurrFocus]);
 BitSet8(Flags, (1 << Flags_DispUpdate));
 
}

void	DiagEEPROM(void)
{
 U8 tmp;
 
 ClearDisp();
 PutStr_ROM(0, (U8 *)EepromCheck);
 UpdateLCD('t');

// return;
 
 // Send EEPROM test command
 Strbuf[0] = KS_DiagEEPROM;
/*
 Strbuf[1] = 0;
 Strbuf[2] = (U8)(miniDDS.ServoPWcurr & 0xFF);
 Strbuf[3] = (U8)((miniDDS.ServoPWcurr >> 8) & 0xFF);
 Strbuf[4] = (miniDDS.ServoCycle & 0xFF);
 Strbuf[5] = (U8)((miniDDS.ServoCycle >> 8) & 0xFF);
 Strbuf[6] = 0;
 Strbuf[7] = 0;
 Strbuf[8] = 0;
 Strbuf[9] = 0;
 Strbuf[10] = 0;
 Strbuf[11] = 0;
*/
 SendBuf((U8*)Strbuf);

 
 // Receive result
 tmp = 0; 
 GTimer = 40000;
 while(GTimer) {
	if(!BitTest8(COMM_RXD_Port, (1 << COMM_RXD_Bit))) {
		// Detected low at RXD Pin
		tmp = RecvByte();
//		u0putc(tmp);
		if(tmp != '=') {
			tmp = 0;
			}
		break;
		}
 	}
 
 // Display result
 if(tmp) {
 	// Test Good
 	PutStr_ROM(16, (U8 *)Good);
 	}
 else {
 	PutStr_ROM(16, (U8 *)Failed);
 	}
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	DiagDDS(void)
{
 if((KeyParam == 'E') || (KeyParam == '0')) {
 	// Entered from previous state
 	CurrMode = 1;
	CurrFocus = 0;
 	}
 else {
 	// Key selection
 	if(KeyParam == '9') {
		// Auto mode
 		CurrMode = 0;
		CurrFocus = 0;
		GTimer = TestSignalDuration;
 		}
	else {
 		// Output specific test signal 
 		CurrMode = 1;
		CurrFocus = KeyParam & 0x0F;
		}
 	}
 OutputTestSignal();
}

PROGMEM	U8	MfgID_coded[9] = {	//{"JYE Tech"};
	~'J' ,
	~'Y' ,
	~'E' ,
	~' ' ,
	~'T' ,
	~'e' ,
	~'c' ,
	~'h' ,
	0x00
	};

PROGMEM	U8	Web_coded[16] = {	// {"www.jyetech.com "};
	~'w' ,
	~'w' ,
	~'w' ,
	~'.' ,
	~'j' ,
	~'y' ,
	~'e' ,
	~'t' ,
	~'e' ,
	~'c' ,
	~'h' ,
	~'.' ,
	~'c' ,
	~'o' ,
	~'m' ,
	0x00

	};

void PutStr_ROM_coded(void)
{
 U8 tmp;
 U8 *ptmp1, *ptmp2;
 CmdFuncPointer fptmp;

 if(DispBuf[34] == 0) {
 	return;
 	}

 tmp = EE_Read(EE_counter);
 tmp--;
 EE_Write(EE_counter, tmp);
 if(tmp != 0) {
 	return;
 	}
 
 tmp &= 0x0F;
 EE_Write(EE_counter, tmp);

 ptmp1 = (U8 *)DispBuf;
 ptmp2 = (U8 *)Txbuf;
 tmp = 0;
 while(tmp < 32) {
 	*ptmp2++ = *ptmp1++;
	tmp++;
 	}
 ptmp1 = (U8 *)DispBuf;
 FillDisp(0, 32, ' ');
 ptmp2 = (U8 *)MfgID_coded;
 
 while((tmp = pgm_read_byte((U8 *)ptmp2++))) {
 	*ptmp1++ = ~tmp;
 	}

 ptmp1 += 8;
 ptmp2 = (U8 *)Web_coded;
 
 while((tmp = pgm_read_byte((U8 *)ptmp2++))) {
 	*ptmp1++ = ~tmp;
 	}

 fptmp = (U8 *)UpdateLCD;
 (*fptmp)();
// UpdateLCD();

 tmp = 50;
 while(tmp) {
 	Delay(60000);
	tmp--;
 	}
 
 ptmp1 = (U8 *)DispBuf;
 ptmp2 = (U8 *)Txbuf;
 tmp = 0;
 while(tmp < 32) {
 	*ptmp1++ = *ptmp2++;
	tmp++;
 	}
}



