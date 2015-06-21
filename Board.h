//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Board.h
//	Version:		0.01
//	Data:		2006.01.03
//
//	Author:		Liu, Zemin
//	Company:	zmLAB
//
//-----------------------------------------------------------------------------
//
// 	Target: 		ATmega48
// 	Tool chain: 	WinAvr (GCC-AVR)
//
//
//-----------------------------------------------------------------------------
//	Required files:
//
//-----------------------------------------------------------------------------
//	Notes:
//		1.	This file is associated with 113-03101-XXX.
//
//-----------------------------------------------------------------------------
//	Revision History:
//		V0.01		2006.01.03
//		1. First created.
//
///////////////////////////////////////////////////////////////////////////////
//
//

#ifndef	Board_h

#define	Board_h

#include	"Common.h"

// CPU frequency
//#define F_CPU      8000000L          	

// ===========================================
//	Uart parameters
//
#define	Uart0_Baud 	38400L
#define	Uart1_Baud 	38400L

#define	Uart0 	0
#define	Uart1	1

// ===========================================
// ADC channels
//

// -- Port bit definitions
#define	LCD_D_Port		PORTB
#define	LCD_D_Bit		0b00111100
#define	LCD_RS_Port		PORTB
#define	LCD_RS_Bit		PB1
#define	LCD_RW_Port	PORTB
#define	LCD_RW_Bit		PB1
#define	LCD_EN_Port	PORTB
#define	LCD_EN_Bit		PB0

#define	LCD_CMD		0
#define	LCD_DATA		1

#define	KeyIn_Port		PINC
#define	KeyIn_Bits		0x0F
#define	KeyDrive_Port	PORTB
#define	KeyDrive_Bits	0x3E

#define	KeyDrive_Port1	PORTD
#define	KeyDrive_Bits1	0x08

#define	EncIn_Port		PINC
#define	EncIn_Bits		0x30

#define	Beeper_Port		PORTC
#define	Beeper_Bit		PC5

// COMM definitions
#define	COMM_HS_Port		PORTD
#define	COMM_HS_Port_Dir	DDRD
#define	COMM_HS_Bit		PD2
#define	COMM_TXD_Port		PORTD
#define	COMM_TXD_Bit		PD3
#define	COMM_RXD_Port		PIND
#define	COMM_RXD_Bit		PIND2

#define	CommSync		'='
#define	DDS_Comm_Frame_Len		17

// Digital POT control
#define	DPOT_CS_Port	PORTD
#define	DPOT_CS_Bit		PD4
#define	DPOT_UD_Port	PORTD
#define	DPOT_UD_Bit	PD7
#define	DPOT_INC_Port	PORTD
#define	DPOT_INC_Bit	PD5


// ============== Key Analysis Definitions =====================
//
#define	KVoid		0

// -- Key Code Definitions 
enum	KeyCodes {
	KC_void 	= 0,				
	KC_1,				
	KC_2,				
	KC_3,				
	KC_4,				
	KC_5,				
	KC_6,				
	KC_7,				
	KC_8,				
	KC_9,				
	KC_0,				
	KC_Sign,
	KC_DP,				
	KC_ESC,				
	KC_WF,				
	KC_Hz,				
	KC_KHz,				
	KC_Mode,
	KC_Freq,
	KC_Amp,
	KC_OFS,
	KC_Aux,
	KC_CW,
	KC_CCW,
	KC_Aux_ESC,
	KC_Max			
	};

#define	NoKey			0xFFFF	

#define	KD_val			20		// 20 * 1ms
#define	KH_val			50		// 8 * 250ms
#define	KR_Time		1	

typedef	struct	{
	U16	ScanCode;
	U8	KeyCode;
	U8	KeyParam;
	}KeyScanCode;

// ====================================================
//	Macros
//

// ====================================================
// Declarations of public variables
extern	volatile	U8	DispBuf[];
extern	volatile	U16	GTimer;
extern	volatile	U8	Flags;	
#define	Flags_DispUpdate		7
#define	Flags_NoOverwrite		6	// Postpone keycode reception if the current keycode has not been processed.
extern	volatile	U8	PrevWiperPos;

extern	volatile	U8	Strbuf[];

extern	U8	KeyCode;
extern	U8	KeyCodeBuf;
extern	U8	KeyParam;
extern	volatile	U8	KFlags;
// KFlags bit allocation
#define	KReady		7
#define	KScanRdy	6
#define	Enc_CW		5	// Encoder clockwise
#define	Enc_CCW	4	// Encoder counter clockwise

// ====================================================
//	Function Prototype Declarations
//
void	Misc_Init(void);
void Port_Init(void);
void	ExtIntr_Init(void);
void ADC_Init (void);
U16	ADC_Poll(U8 channel);
void	VREF_Init(void);
void Timer0_Init(void);
void Timer1_Init (void);
void Timer2_Init (void);
void Comp_Init(void);
void Uart0_Init (void);
void Uart1_Init (void);

void	u0putc(U8 ch);
U8	u0getc(void);
U8	u0rcheck(void);
void	uputs(U8 *s);
void	uputs_ROM(U8 *s);

void	LCD_WriteByte(U8 RS, U8 byte);
void	LCD_Init(void);
U8	UpdateLCD(U8 symbol);
void UpdateOfs(U32 new_ofs);
void	ClrDispBuf(void);
void	SetDigitalPot(U8 WiperPos);

U8 EE_Read(U16 uiAddress);
void EE_Write(U16 uiAddress, U8 ucData);

void KeyScan(void);
void	KeyConvert(KeyScanCode *KSCTab, U16 KSCode);

void	SendBuf(U8 *buf);
void	SendByte(U8 ch);
U8	RecvByte(void);

#endif	// Board_h


