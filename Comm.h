//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Comm.h
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
//
//
#ifndef	Comm_h
#define	Comm_h

#include	"Comm.h"


// ===========================================================
//
#define	FrameSizeMax		300		// in bytes
#define	Uart0RxBufSize		300
#define	Uart0TxBufSize		50

// SYNC characters
#define	SynCh				0xFE
#define	SynCh1				0x5A	// 'Z'
#define	SynCh2				0x5A
#define	FrameEnd			0xAA

// Frame IDs
#define	FID_SetVendorName			0xF8
#define	FID_RestoreMfgName		0xF9
#define	FID_NoOverwrite			0xFA
#define	FID_Keycode				0xFB
#define	FID_ReadRequest			0x10
#define	FID_WriteRequest			0x11
#define	FID_ReadReturn				0x12
#define	FID_WriteReturn				0x13
#define	FID_WriteWaveform			0x14
#define	FID_Connect					0xE1	//
#define	FID_Confirm					0xE2	//

extern	U8	Txbuf[];
extern	U8 	MfgID_coded1[];
extern	U8	Web_coded1[];

void	u0RxBufInit(void);
U8	Uart0RxBufAdd(U8 *ChPtr);
U8	Uart0RxBufDelete(U8 *ChPtr);
U8 	IsUart0RxBufEmpty(void);
void	CommInit(void);
U8	IsFrame(U8 FrameID);
void	CommRx(void);
void	CmdProc(void);
void CommTx(U8 *buf, U16 len);
void PutStr_ROM_coded1(U8 loc, U8 *str);

#endif

