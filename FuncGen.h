//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Command.h
//	Version:		0.01
//	Data:		2006.01.16
//
//	Author:		Liu, Zemin
//	Company:	zmLAB
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
#ifndef	FuncGen_h
#define	FuncGen_h

#include	"FuncGen.h"

#define	EE_counter			127
#define	EE_VendorID		128		// Start address in EEPROM for vendor ID
#define	EE_UserWaveform	0x0100	// Start address of user waveform

// Constants
#define	FreqMax		999999L
#define	FreqMin			0
#define	AMPmax 		10000
#define	OFSmax			10000
#define	FG_SYSCLK		20000000.0	// DDS chip system clock
#define	ServoAmpMax	10000
#define	WaveMax		8			// Number of waveform types

// ======= Display parameters =================
//
#define	Char_Blank	'_'
#define	Sign_Blank					0
#define	Sign_Neg					1
#define	Sign_Pos					2

#define	Freq_InputBufSize			8
#define	Freq_DispStart				0
#define	Freq_SignOfs				2
#define	Freq_DigitOfs				3
#define	Freq_UnitOfs				8

#define	SwParam_InputBufSize		9
#define	SwParam_DispStart			16
#define	SwParam_SignOfs			6
#define	SwParam_DigitOfs			7
#define	SwParam_UnitOfs			14

#define	SwAmp_InputBufSize			6
#define	SwAmp_DispStart			21
#define	SwAmp_SignOfs				0
#define	SwAmp_DigitOfs				1
#define	SwAmp_UnitOfs				6

#define	SwOfs_InputBufSize			6
#define	SwOfs_DispStart				21
#define	SwOfs_SignOfs				0
#define	SwOfs_DigitOfs				1
#define	SwOfs_UnitOfs				6

#define	Amp_InputBufSize			6
#define	Amp_DispStart				16
#define	Amp_SignOfs				0
#define	Amp_DigitOfs				1
#define	Amp_UnitOfs				6

#define	AmpPos_CW					17
#define	AmpPos_Sweep				21

#define	Ofs_InputBufSize			6
#define	Ofs_DispStart				25
#define	Ofs_SignOfs					0
#define	Ofs_DigitOfs					1
#define	Ofs_UnitOfs					6

#define	OfsPos_CW					24
#define	OfsPos_Sweep				20

#define	ServoPos_DispStart			0
#define	ServoPosPW_InputBufSize	5
#define	ServoPosPW_DispStart		16
#define	ServoPosPW_SignOfs			3
#define	ServoPosPW_DigitOfs		4
#define	ServoPosPW_UnitOfs			10

#define	ServoRun_DispStart			0
#define	ServoRunState_DispStart		16

#define	ServoPosAmp_InputBufSize	5
#define	ServoPosAmp_DispStart		16
#define	ServoPosAmp_SignOfs		4
#define	ServoPosAmp_DigitOfs		5
#define	ServoPosAmp_UnitOfs		9

#define	ModePrompt_DispStart		0
#define	ModeName_DispStart			16
#define	ModeIndex_DispStart		29

#define	ServoParamName_DispStart	0
#define	ServoParamIndex_DispStart	13
#define	ServoParam_DispStart		16
#define	ServoParam_InputBufSize	5
#define	ServoParam_SignOfs			0
#define	ServoParam_DigitOfs			1
#define	ServoParam_UnitOfs			7

#define	SweepTswName_DispStart	0
#define	SweepTsw_DispStart			16
#define	SweepTsw_InputBufSize		5
#define	SweepTsw_SignOfs			0
#define	SweepTsw_DigitOfs			1
#define	SweepTsw_UnitOfs			7

#define	Inc_InputBufSize			8
#define	Inc_DispStart				16
#define	Inc_SignOfs					0
#define	Inc_DigitOfs					1
#define	Inc_UnitOfs					8

#define	SetStep_DispStart			0
#define	StepTitle_DispStart			0
#define	StepDigit_DispStart			16
#define	StepUnit_Ofs				11

// Function Generator structure
typedef struct {
	U8	Mode;
	U32	Freq;
	U8	SetType;	// 0 for Freq, 1 for Period
	U8	SetUnit;		// 
	U32	Amp;
	U32	Ofs;
	U8	CurrWave;
	U32	FreqIncStep;
	U32	CycleIncStep;
	U16	ServoPWcurr;
	U16	ServoAMPcurr;
	U16	ServoPWmin;
	U16	ServoPWmax;
	U16	ServoPWmid;
	U16	ServoPWinc;
	U16	ServoRunStep;
	U16	ServoRunRate;
	U16	ServoCycle;
	U16	ServoAMPmax;
	U32	SweepStartFreq;
	U32	SweepStopFreq;
//	U32	SweepCentertFreq;
//	U32	SweepSpanFreq;
	U32	SweepTime;
	U32 	SweepTswStep;		// In milisecond
	U8	SweepDir;	// ' ' (space) for mono-directional (low-to-high only). 'B' for bi-directional
	U8	Duty;
} FuncGen;

enum	SV_Param {
		SV_PWmin,
		SV_PWmax,
		SV_PWmid,
		SV_PWinc,
		SV_RunStep,
		SV_RunRate,
		SV_Cycle,
		SV_AMPmax,
		SV_DefaultRestore,
		ServoParamNumber
};

enum	ServoState {
		ServoState_Ready,
		ServoState_Running,
		ServoState_Hold
};

typedef struct {
	U8	CurrState;
	U8	PrevState;
	void	(*BackOutAction)(void);	// Function to execute at back out to previous state
} State;

typedef struct {
	U8	*InBuf;
	U8	DispStart;		// Start position in display buffer
	U8	DispLen;		// 
	U8	DigitCnt;		// Number of digits
	U8	Sign;
	U8	DPLoc;			// Decimal point location
	U32	DataValue;
	U8	EndKey;			// Keycode of ending Input
	U8	ExitTo;			// State to exit to
	void	(*ExitAction)(void);	// Function to execute at ExitAction
} DataEntry;

// -- Command Entry Structure
typedef	struct	{
	U8 	CmdCode;
	U8	NextState;
	CmdFuncPointer	CmdAction;
//	void	(*CmdAction)();
	}CmdEntry ;

enum	KeyStates {
	KS_CW,
	KS_Sweep,
	KS_ServoPos,
	KS_ServoRun,
	KS_ServoParam,
	KS_ModeSel,
	KS_DigitInput,
	KS_DiagLCD,
	KS_DiagKey,
	KS_DiagEEPROM,
	KS_DiagDDS,
	KS_ShowStep,
	KS_ShowDuty,
	KS_FetchDDSver,
	KS_WriteWavetable,
	KStateLast
	};
#define	ModeNumber 	KS_ServoRun + 1

extern	FuncGen	miniDDS;
extern	State	SysState;

extern	U8	CurrFocus;
extern	U8	CurrMode;
extern	U8	TrigCtrl;

enum	Units {
			Unit_Hz,
			Unit_KHz,
			Unit_Sec,
			Unit_mSec,
			Unit_V,
			Unit_mV,
			Unit_uSec,
			Unit_Degree,
			Unit_Last
		};

enum	Focus {
			FC_Freq,
			FC_Amp,
			FC_Ofs,
			FC_ServoPW,
			FC_ServoAmp,
			FC_ServoRun,
			FC_SW_StartFreq,
			FC_SW_StopFreq,
//			FC_SW_CenterFreq,
//			FC_SW_SpanFreq,
			FC_SW_Time,
			FC_SW_Tstep,
			FC_SW_Amp,
			FC_SW_Ofs,
			FC_max
		};

enum	Waveform {
		WF_Sine,
		WF_Square,
		WF_Triangle,
		WF_RampPos,
		WF_RampNeg,
		WF_StairPos,
		WF_StairNeg,
		WF_User,
		WF_Last
		};


#define	SetType_Freq	0
#define	SetType_Period	1

// Code for first entered parameter type
#define	Fstart1			0x00
#define	Fstop1			0x04
#define	Fcenter1		0x08
#define	Fspan1			0x0C

// Code for second entered parameter type
#define	Fstart2			0x00
#define	Fstop2			0x01
#define	Fcenter2		0x02
#define	Fspan2			0x03

#define	EE_signature1	0xFA
#define	EE_signature2	0x5F
#define	EepromVersion	0x04	// 0x04 for v1.30

// ====================================================
//	Function Prototype Declarations
// ====================================================
//
void	DoCmdProcessing(void);
void	*CmdAnalysis(U8 Ccode, CmdEntry *CmdTab, U8 *PState);
void AppInit(void);
void	CW_Init(void);
void	SweepInit(void);
void	ServoPosInit(void);
void	ServoRunInit(void);
void	ChangeSVParam(void);
void	SetSVParam(void);
void	SVParamDefault(void);
void	ExitSVParam(void);
void	ChangeTswParam(void);
void	SetTswParam(void);
void	ExitTswParam(void);
void Dummy(void);
void Dummy1(U8 Dir);
void	ChangeMode(void);
void	SelectMode(void);
void	EscapeMode(void);
void	FreqKey(void);
void	SetFreqCW(void);
void	SetSweepParam(void);
void	QuickSweepParam(void);
void	SetSweepAmp(void);
void	SetSweepOfs(void);
void	SetPeriod(void);
void	SetAmp(void);
void	SetOfs(void);
void	SetPW(void);
void	SetSVAmp(void);
void	SetIncrement(void);
void	SetDuty(void);
void	NumInputSetup(U8 dispstart, U8 displen, U8 exittostate, void (*exitaction)(void));
void	StartStop(void);
void	CAN(void);
void	UnitButton(void);
void	Hz(void);
void	KHz(void);
void	TYPE(void);
void ClearDisp(void);
void PutStr_ROM(U8 loc, U8 *str);
void	ToggleSign(void);
void	Digit(void);
U8	CalWholePart(void);
U8	CalValue(void);
void	Inc(void);
void	Dec(void);
void	ClearInputBuf(void);
void	DisplayInputBuf(void);
void	FreqInputHandle(void);
void	SweepParamInputHandle(void);
void	SweepAmpInputHandle(void);
void	SweepOfsInputHandle(void);
void	AmpInputHandle(void);
void	OfsInputHandle(void);
void	PWInputHandle(void);
void	TswInputHandle(void);
void	IncInputHandle(void);
void	DutyInputHandle(void);
void	ServoAmpInputHandle(void);
void	ServoParamInputHandle(void);
void	ShowMode(void);
void	ShowFreqPeroid(void);
void	ShowSweep(void);
void	ShowAmp(U8 startpos);
void	ShowOfs(U8 startpos);
void ShowType(void);
void	ShowFocus(void);
void	ShowPW(void);
void	ShowServoAmp(void);
void	ShowSVParam(void);
void	ShowServoRun(void);
void	ShowTsw(void);
void	ShowStep(void);
void	ShowDuty(void);
void	SendDDS(void);
void	SendSweep(void);
void	SendServoPos(void);
void	SendServoRun(void);
void	LoadUserWaveform(void);
void	SaveSetting(void);
void	FillDisp(U8 startpos, U8 len, U8 symbol);
void	DispWholeNum(U8 startpos, U8 maxlen, U32 *num);
void	PutFreq(U32 *item, U32 freq);
void	ToggleTrigCtrl(void);
void	ToggleSweepDir(void);

#endif

