//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	ServoTester.h
//	Version:		0.01
//	Data:		2006.02.28
//
//	Author:		Liu, Zemin
//	Company:	zmLAB
//
//-----------------------------------------------------------------------------
//
// 	Target: 		ATmega88
// 	Tool chain: 	WinAvr (GCC-AVR)
//
//
//-----------------------------------------------------------------------------
//	Required files:
//
//-----------------------------------------------------------------------------
//	Notes:
//		1.	This file is associated with 113-03601-XXX.
//
//-----------------------------------------------------------------------------
//	Revision History:
//		V0.01		2006.02.28
//		1. First created.
//
///////////////////////////////////////////////////////////////////////////////
//
//

#ifndef	ServoTester_h

#define	ServoTester_h

#include	"Common.h"

// -- Command Entry Structure
typedef	struct	{
	U8 	CmdCode;
//	U8	NextState;
	void	(*CmdAction)();
	}CmdEntry ;

// -- Command Index Structure
typedef	struct {
	U8			Cmd;
	CmdEntry	*TabIndex;
}  CmdIndex;

// -- Key Entry Structure
typedef	struct	{
	U8 	KeyCode;
	U8	NextState;
	void	(*KeyFunc)();
	}KeyEntry ;

// -- Key Code Constants
#define	KC_Sel		0b00000001		// SW2
#define	KC_Exit		0b00000010		// SW3
#define	KC_Aux		0b00000100		// SW1 pushbutton
#define	KC_Enter	0b00001000		// SW4
#define	KC_Inc		0b00010000		// SW1 CW
#define	KC_Dec		0b00100000		// SW1 CCW

enum	PState {
	PS_Standby
	};

// ====================================================
//	Function Prototype Declarations
//
void App_Init(void);
void	SelectFunc(void);
void	ExitFunc(void);
void	EnterFunc(void);
void	AuxFunc(void);
void	NextFunc(void);
void	PrevFunc(void);

void	DoKeyProcessing(void);
void	*KeyAnalysis(U8 Kcode, KeyEntry **PSindex);
void	Dummy(void);

void	GetVolt(void);
void	GetCurr(void);
void	UpdatePWM(void);

CmdEntry *GetIndex(U8 CmdChar);
void	*CmdAnalysis(U8 Ccode, CmdEntry *CmdTab);
U8 	GetHex(U8 FirstDigit, U8 nDigit, U16 *Buf);
void	GoPosition(void);
void	GoInc(void);
void	GoDec(void);
void	GoHalt(void);
void	GoRun(void);
void	SetPara(void);
void	ReadPara(void);
void	ReadCmd(void);
void	Logger(void);
void	uputHex4(U16 hdata);
void	ServoCmd(U8 *cmd);
U8 	*ServoRly(U8 *buf, U8 len);
U16	ServoPos(void);

#endif	// ServoTester_h



