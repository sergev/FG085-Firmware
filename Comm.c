//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Comm.c
//	Version:		
//	Data:		2011.04.08
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
//
///////////////////////////////////////////////////////////////////////////////
#include	<avr/Pgmspace.h>

#include <avr/interrupt.h>
#include "Common.h"
#include "Board.h"
#include	"Diag.h"
#include	"FuncGen.h"
#include	"Comm.h"

// ===========================================================
// 	File Scope Global variables
// ===========================================================
//
volatile	U8	Uart0RxBuf[Uart0RxBufSize];
volatile	U8	*u0RxQueueHead, *u0RxQueueTail;

U8	Txbuf[Uart0TxBufSize];

U8	FrameBuf[FrameSizeMax];
U16	RxCnt;
U8	CommStatus;
// Bit definition for CommStatus
#define	CS_SynCh1Found	7
#define	CS_SynCh2Found	6
#define	CS_Rxing			5

PROGMEM	U8	MfgID_coded1[17] = {	//{"JYE Tech"};
	0x0F ^ 'J' ,
	0x0F ^ 'Y' ,
	0x0F ^ 'E' ,
	0x0F ^ ' ' ,
	0x0F ^ 'T' ,
	0x0F ^ 'e' ,
	0x0F ^ 'c' ,
	0x0F ^ 'h' ,
	0x0F ^ ' ' ,
	0x0F ^ '(' ,
	0x0F ^ 'c' ,
	0x0F ^ ')' ,
	0x0F ^ '2' ,
	0x0F ^ '0' ,
	0x0F ^ '1' ,
	0x0F ^ '2' ,
	0x00
	};

PROGMEM	U8	Web_coded1[16] = {	// {"www.jyetech.com "};
	0x0F ^ 'w' ,
	0x0F ^ 'w' ,
	0x0F ^ 'w' ,
	0x0F ^ '.' ,
	0x0F ^ 'j' ,
	0x0F ^ 'y' ,
	0x0F ^ 'e' ,
	0x0F ^ 't' ,
	0x0F ^ 'e' ,
	0x0F ^ 'c' ,
	0x0F ^ 'h' ,
	0x0F ^ '.' ,
	0x0F ^ 'c' ,
	0x0F ^ 'o' ,
	0x0F ^ 'm' ,
	0x00

	};

// ===========================================================
// 	Function definitions
// ===========================================================
//
// ========================================================
// Setup Uart 0 Rx buffer
//
void	u0RxBufInit(void)
{
 u0RxQueueHead = Uart0RxBuf;
 u0RxQueueTail = Uart0RxBuf;
}

// ========================================================
// Insert one character to Uart0Rxbuf queue. Returns non-zero if success,
//	otherwise returns 0.
// 
U8	Uart0RxBufAdd(U8 *ChPtr)
{
 // Store the char
 *u0RxQueueTail = *ChPtr;	

 // Advance pointer
 if(u0RxQueueTail + 1 >= Uart0RxBuf + Uart0RxBufSize) {
 	// Roll over
 	ChPtr = (U8 *)Uart0RxBuf;
 	}
 else {
 	ChPtr = (U8 *)(u0RxQueueTail + 1);
 	}

 if(ChPtr == u0RxQueueHead) {
 	// Queue full. Insertion failed
 	return 0;
 	}
 else {
 	// Successful
 	u0RxQueueTail = ChPtr;
	return 1;
 	}

}

// ========================================================
// Delet one character from Uart0RxBuf and save it to location pointed by "Ch". 
//	Returns non-zero if success, otherwise returns 0.
// 

U8	Uart0RxBufDelete(U8 *ChPtr)
{
 if(IsUart0RxBufEmpty()) {
 	// Queue empty
 	return 0;
 	}
 // Retrieve one character from queue
 *ChPtr = *u0RxQueueHead;
 // Advance pointer
 if(u0RxQueueHead + 1 >= Uart0RxBuf + Uart0RxBufSize) {
 	u0RxQueueHead = Uart0RxBuf;
 	}
 else {
 	u0RxQueueHead++;
 	}
 return 1;
 
}

U8 	IsUart0RxBufEmpty(void)
{
 return (u0RxQueueHead == u0RxQueueTail);
}

void	CommInit(void)
{
 u0RxBufInit();
// u0TxBufInit();
 
 CommStatus = 0x00;
//rameIndex = 0;

 // -- Enable receiver interrupt
 // -- Enable receiver
 // -- Enable transmitter
 UCSR0B = (1<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0)|(1<<RXEN0)
 			|(1<<TXEN0)|(0<<UCSZ02)|(0<<TXB80);

// Setup baudrate for 115200 bps
 UBRR0H = 0;
 UBRR0L = 21;
 UCSR0A = (0<<TXC0)|(1<<U2X0)|(0<<MPCM0);
 
}

U8		IsFrame(U8 FrameID)
{
 U8 tmp0 = 0;
 
 switch(FrameID) {
	case FID_RestoreMfgName:
	case FID_SetVendorName:
	case FID_NoOverwrite:
	case FID_Keycode:
	case FID_ReadRequest:
	case FID_WriteRequest:
	case FID_Connect:
	case FID_Confirm:
	case FID_WriteWaveform:
		tmp0 = 1;
	default:
		break;
	}

 return(tmp0);
}

void	CommRx(void)
{
 U8	tmp0;

 // Check and get a byte from Uart0RxBuf
// while(Uart0RxBufDelete(&tmp0)) {
 while(!IsUart0RxBufEmpty()) {
	 if((BitTest8(Flags, (1 << Flags_NoOverwrite))) && (KeyCode != 0)) {
		// Do not receive if current keycode has not been processed
		break;
	 	}

	// Get one byte
	Uart0RxBufDelete(&tmp0);
	// Check for SynCh
	if(tmp0 == SynCh) {
		// Mark it if SycCh found
		BitSet8(CommStatus, (1 << CS_SynCh1Found));
		continue;
		}

 	if(BitTest8(CommStatus, (1 << CS_SynCh1Found))) {
		// Process the byte following SynCh
		BitClr8(CommStatus, (1 << CS_SynCh1Found));	// Clear SynCh_Found flag first
		// The previous byte was SynCh. If Rxing is valid and the byte following SynCh is 0x00
		//	then the SynCh was a data byte. Otherwise check for frame ID. 
		if((BitTest8(CommStatus, (1 << CS_Rxing))) && (tmp0 == 0x00)) {
			// Frame receiving in process and the current byte is 0x00. Save SynCh as data
			FrameBuf[RxCnt++] = SynCh;
			// Go to check for frame end
			goto FrameEndCheck;
			}
		else {
			// Rxing not valid or the byte following SynCh is not 0x00.
			// Check for frame ID
			if(IsFrame(tmp0)) {
				// Initialize a freme
				FrameBuf[0] = tmp0;
				FrameBuf[2] = 0xFF;	// Make sure correct frame length check (byte1 & byte 2 are frame length)
				RxCnt = 1;
				BitSet8(CommStatus, (1 << CS_Rxing));
				}
			else {
				// Clear Rxing anyway. This is to prevent error at Rxing being valid but
				//	SynCh is not followed by frame ID or 0x00.
				BitClr8(CommStatus, (1 << CS_Rxing));
				}
			}
 		}
	else {
		// Normal byte processing. Save bytes if Rxing is valid
		if(BitTest8(CommStatus, (1 << CS_Rxing))) {
			FrameBuf[RxCnt++] = tmp0;
			FrameEndCheck:
			if((RxCnt >= (((U16)FrameBuf[2] << 8) | (U16)FrameBuf[1])) ||(RxCnt > FrameSizeMax)) {
				CmdProc();
				BitClr8(CommStatus, (1 << CS_Rxing));
				}
			}
 		}
 	}
 
}

// Send data in 'buf' of 'len' bytes
void CommTx(U8 *buf, U16 len)
{
 U8 tmp0;
 
 if(len == 0) {
 	return;
 	}

 u0putc(SynCh);
 
 while(len) {
	u0putc(tmp0 = *buf++);
	if(tmp0 == SynCh) {
		u0putc(0x00);
		}
	len--;
 	}
}


void	CmdProc(void) 
{
 U8	tmp;
 U16 tmp0, tmp1, tmp2;
 
 switch(FrameBuf[0]) {
	case FID_SetVendorName:
		tmp = 0;
		while(tmp < 34) {
 			EE_Write((U16)(EE_VendorID + tmp), ~FrameBuf[3 + tmp]);	// Write inverted data for encryption
			tmp++;
			}
		Ackownledge:
		u0putc('G');
		break;
		
	case FID_RestoreMfgName:
		if((FrameBuf[3] == 'J') && (FrameBuf[4] == 'Y')) {
			EE_Write((U16)(EE_VendorID), 0xFF);
			goto Ackownledge;
//			u0putc('G');
			}
		break;

	case FID_NoOverwrite:
		BitSet8(Flags, (1 << Flags_NoOverwrite));
		goto Ackownledge;
//		u0putc('G');
//		break;

	case FID_Keycode:
		switch(FrameBuf[3]) {
			case KC_Aux_ESC:
				goto Ackownledge;

			case 0xA0:
				// Turn No-Overwrite off
				BitClr8(Flags, (1 << Flags_NoOverwrite));
				goto Ackownledge;

			case 0xA1:
				// Turn No-Overwrite on
				BitSet8(Flags, (1 << Flags_NoOverwrite));
				goto Ackownledge;

			case 0xA2:
				// Set cursor position
				if(SysState.CurrState == KS_CW) {
					// Only process this code under CW mode
					if(FrameBuf[4] < 3) {
						// Place cursor to location indicated by FrameBuf[4]
						CurrFocus = FrameBuf[4];
						ShowFocus();
						}
					}
				goto Ackownledge;
				
			case 0xA3:
				// Set waveform
				if((SysState.CurrState == KS_CW) ||(SysState.CurrState == KS_Sweep)) {
					// Only process this code under CW mode or Sweep mode

					// Set waveform indicated by FrameBuf[4]
					//	Decrease waveform number by 1 first because it will be added
					//	back in TYPE();
					tmp = FrameBuf[4] - 1;
					if(tmp == 0xFF) {
						tmp = WaveMax - 1;
						}
					miniDDS.CurrWave = tmp;
					TYPE();			// For any number greater than WaveMax Sine will be used.
					}
				goto Ackownledge;
				
			default:
				KeyCode = FrameBuf[3];
				KeyCodeBuf = KeyCode;	// Keep a copy to determine handling in
										//	UnitButton()
				KeyParam = FrameBuf[4];
				goto Ackownledge;
			}
//		break;
/*		
		if(FrameBuf[3] == KC_Aux_ESC) {
			// Do not enter Diagnostic for request from serial
			break;
			}
		if(FrameBuf[3] == 0xA0) {
			// Turn No-Overwrite off
			BitClr8(Flags, (1 << Flags_NoOverwrite));
			break;
			}
		KeyCode = FrameBuf[3];
		KeyParam = FrameBuf[4];
		goto Ackownledge;
//		break;
*/		
	case FID_ReadRequest:
		break;

	case FID_WriteRequest:
		if((FrameBuf[3] & 0xC0) == 0x80) {
			// Write internal EEPROM
			tmp1 = *((U16 *)&FrameBuf[6]);	// start address
			tmp2 = *((U16 *)&FrameBuf[10]);	// Write size
			tmp0 = 0;
			while(tmp0 < tmp2) {
 				EE_Write(tmp1, ~(FrameBuf[14 + tmp0]));	// Data are reversed before writing
				tmp1++;
				tmp0++;
				}
			// Send acknowledgement
			Txbuf[0] = FID_WriteReturn;
			Txbuf[1] = 0x06;
			Txbuf[2] = 0;
			Txbuf[3] = 0x80;	// Echo write type, i.e. Internal EEPROM write
			CommTx(Txbuf, *((U16 *)&Txbuf[1]));
			}
		break;
		
	case FID_WriteWaveform:
		// Write waveform to EEPROM address 0x0100
		tmp1 = 0x0100;	// start address
		tmp2 = *((U16 *)&FrameBuf[4]);	// Write size
		if(tmp2 > 256) {
			tmp2 = 256;
			}
		tmp0 = 0;
		while(tmp0 < tmp2) {
 			EE_Write(tmp1, FrameBuf[14 + tmp0]);
			tmp1++;
			tmp0++;
			}
		// Send acknowledgement
		Txbuf[0] = FID_WriteReturn;
		Txbuf[1] = 0x06;
		Txbuf[2] = 0;
		Txbuf[3] = 0x80;	// Echo write type, i.e. Internal EEPROM write
		CommTx(Txbuf, *((U16 *)&Txbuf[1]));
		break;

	case FID_Connect:
		// Send confirm
		Txbuf[0] = FID_Confirm;
		Txbuf[1] = 0x09;
		Txbuf[2] = 0;
		Txbuf[3] = 'G';	// Device type. 'G' for signal generator
		Txbuf[4] = 1;	// Model. 1 for 085
		CommTx(Txbuf, *((U16 *)&Txbuf[1]));
		break;		
	default:
		u0putc('?');
		break;
 	}
}

ISR(SIG_USART_RECV)
{
 U8 tmp0;

 tmp0 = UDR0;
 Uart0RxBufAdd(&tmp0);
 
 }

// ==============================================
// Store a ROM string "str" to the "loc" location of DispBuf
//	
void PutStr_ROM_coded1(U8 loc, U8 *str)
{
 U8 tmp;

 DispBuf[32] = 'l';
 while((tmp = pgm_read_byte(str++))) {
 	DispBuf[loc++] = 0x0F ^ tmp;
 	}
}


