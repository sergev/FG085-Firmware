//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Diag.h
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

#ifndef Diag_h

#define	Diag_h

#include	"Common.h"

#define	TestSignalMax			7
#define	TestSignalDuration		50000

// ===========================================================
//	Function Prototype Declarations
// ===========================================================
//
void	DiagDisplay(void);
void	DiagKeypad(void);
void	DiagKeyCheck(void);
void	OutputTestSignal(void);
void	DiagEEPROM(void);
void	DiagDDS(void);
void PutStr_ROM_coded(void);

#endif

