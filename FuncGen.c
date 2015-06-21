//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Command.c
//	Version:		0.10
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
#include	<avr/Pgmspace.h>

#include "Common.h"
#include "Board.h"
#include "FuncGen.h"
#include "Diag.h"

// ===========================================================
// 	File Scope Global variables
// ===========================================================
//
// ---------------------------------------------------------------------------
// -- Key States
//enum	KeyStates  KeyState;
//enum	KeyStates  KeyStateRightBefore;

// -- Input Key Code
		
// -- Key State Tables

PROGMEM 	CmdEntry KS_CW_Tab[23] = {
		{KC_Freq,		KS_CW,			SetFreqCW },
		{KC_OFS,		KS_CW,			SetOfs },
		{KC_Amp,		KS_CW,			SetAmp  },
		{KC_WF,			KS_CW,			TYPE },
		{KC_CW,			KS_CW,			Inc},
		{KC_CCW,		KS_CW,			Dec},
		{KC_Aux,		KS_CW,			ToggleTrigCtrl},
		{KC_Aux_ESC,	KS_DiagLCD,		DiagDisplay},
		{KC_Mode,		KS_ModeSel,		ChangeMode},
		{KC_0,			KS_DigitInput,		SetIncrement },
		{KC_1,			KS_DigitInput,		SetIncrement },
		{KC_2,			KS_DigitInput,		SetIncrement },
		{KC_3,			KS_DigitInput,		SetIncrement },
		{KC_4,			KS_DigitInput,		SetIncrement },
		{KC_5,			KS_DigitInput,		SetIncrement },
		{KC_6,			KS_DigitInput,		SetIncrement },
		{KC_7,			KS_DigitInput,		SetIncrement },
		{KC_8,			KS_DigitInput,		SetIncrement },
		{KC_9,			KS_DigitInput,		SetIncrement },
		{KC_Hz,			KS_ShowStep,	ShowStep},
		{KC_KHz,		KS_ShowStep,	ShowStep},
		{KC_DP,			KS_ShowDuty,	ShowDuty},
		{KVoid,	 		KS_CW,			Dummy	}	
	};

PROGMEM 	CmdEntry KS_Sweep_Tab[14] = {
		{KC_Freq,		KS_Sweep,		SetSweepParam},
		{KC_Amp,		KS_Sweep,		SetSweepAmp  },
		{KC_OFS,		KS_Sweep,		SetSweepOfs },
		{KC_1,			KS_Sweep,		QuickSweepParam },
		{KC_2,			KS_Sweep,		QuickSweepParam },
		{KC_3,			KS_Sweep,		QuickSweepParam },
		{KC_4,			KS_Sweep,		QuickSweepParam },
		{KC_WF,			KS_Sweep,		TYPE },
		{KC_CW,			KS_Sweep,		Inc},
		{KC_CCW,		KS_Sweep,		Dec},
		{KC_Aux,		KS_Sweep,		ToggleTrigCtrl},
		{KC_Mode,		KS_ModeSel,		ChangeMode},
		{KC_Sign,		KS_Sweep,		ToggleSweepDir},
		{KVoid,	 		KS_Sweep,		Dummy	}	
	};

PROGMEM 	CmdEntry KS_ServoPos_Tab[10] = {
		{KC_Freq,		KS_ServoPos,		SetPW },
		{KC_Amp,		KS_ServoPos,		SetSVAmp  },
		{KC_CW,			KS_ServoPos,		Inc},
		{KC_CCW,		KS_ServoPos,		Dec},
		{KC_Aux,		KS_ServoParam,	ChangeSVParam},
		{KC_Mode,		KS_ModeSel,		ChangeMode},
		{KVoid,	 		KS_ServoPos,		Dummy	}	
	};

PROGMEM 	CmdEntry KS_ServoRun_Tab[4] = {
		{KC_WF,			KS_ServoRun,		StartStop},
		{KC_Aux,		KS_ServoParam,	ChangeSVParam},
		{KC_Mode,		KS_ModeSel,		ChangeMode},
		{KVoid,	 		KS_ServoRun,			Dummy	}	
	};

PROGMEM 	CmdEntry KS_ServoParam_Tab[7] = {
		{KC_Freq,		KS_DigitInput,		SetSVParam },
		{KC_CW,			KS_ServoParam,	Inc},
		{KC_CCW,		KS_ServoParam,	Dec},
		{KC_WF,			KS_ServoParam,	SVParamDefault},
		{KC_ESC,		KS_ServoPos,		ExitSVParam},
		{KC_Aux,		KS_ServoPos,		ExitSVParam},
		{KVoid,	 		KS_ServoParam,	Dummy	}	
	};

PROGMEM 	CmdEntry KS_ModeSel_Tab[5] = {
		{KC_Mode,		KS_ModeSel,		SelectMode },
		{KC_CW,			KS_ModeSel,		Inc},
		{KC_CCW,		KS_ModeSel,		Dec},
		{KC_ESC,		KS_ModeSel,		EscapeMode},
		{KVoid,	 		KS_ModeSel, 		Dummy	}	
	};

PROGMEM 	CmdEntry KS_DigitInput_Tab[17] = {
		{KC_Freq,	KS_DigitInput,			FreqKey},
		{KC_0,		KS_DigitInput,			Digit },
		{KC_1,		KS_DigitInput,			Digit },
		{KC_2,		KS_DigitInput,			Digit },
		{KC_3,		KS_DigitInput,			Digit },
		{KC_4,		KS_DigitInput,			Digit },
		{KC_5,		KS_DigitInput,			Digit },
		{KC_6,		KS_DigitInput,			Digit },
		{KC_7,		KS_DigitInput,			Digit },
		{KC_8,		KS_DigitInput,			Digit },
		{KC_9,		KS_DigitInput,			Digit },
		{KC_DP,		KS_DigitInput,			Digit},
		{KC_Sign,	KS_DigitInput,			ToggleSign},
		{KC_ESC,	KS_DigitInput,			CAN },
		{KC_Hz,		KS_DigitInput,			UnitButton },
		{KC_KHz,	KS_DigitInput,			UnitButton },
		{KVoid,	 	KS_DigitInput,			Dummy	}	
	};

PROGMEM 	CmdEntry KS_DiagLCD_Tab[2] = {
		{KC_ESC,		KS_DiagKey,		DiagKeypad},
		{KVoid,	 		KS_DiagLCD,		Dummy	}	
	};

PROGMEM 	CmdEntry KS_DiagKey_Tab[25] = {
		{KC_0,			KS_DiagKey,		DiagKeyCheck},
		{KC_1,			KS_DiagKey,		DiagKeyCheck},
		{KC_2,			KS_DiagKey,		DiagKeyCheck},
		{KC_3,			KS_DiagKey,		DiagKeyCheck},
		{KC_4,			KS_DiagKey,		DiagKeyCheck},
		{KC_5,			KS_DiagKey,		DiagKeyCheck},
		{KC_6,			KS_DiagKey,		DiagKeyCheck},
		{KC_7,			KS_DiagKey,		DiagKeyCheck},
		{KC_8,			KS_DiagKey,		DiagKeyCheck},
		{KC_9,			KS_DiagKey,		DiagKeyCheck},
		{KC_DP,			KS_DiagKey,		DiagKeyCheck},
		{KC_Sign,		KS_DiagKey,		DiagKeyCheck},
		{KC_ESC,		KS_DiagKey,		DiagKeyCheck},
		{KC_WF,			KS_DiagKey,		DiagKeyCheck},
		{KC_Hz,			KS_DiagKey,		DiagKeyCheck},
		{KC_KHz,		KS_DiagKey,		DiagKeyCheck},
		{KC_Mode,		KS_DiagKey,		DiagKeyCheck},
		{KC_Freq,		KS_DiagKey,		DiagKeyCheck},
		{KC_OFS,		KS_DiagKey,		DiagKeyCheck},
		{KC_Amp,		KS_DiagKey,		DiagKeyCheck},
		{KC_Aux,		KS_DiagKey,		DiagKeyCheck},
		{KC_CW,			KS_DiagKey,		DiagKeyCheck},
		{KC_CCW,		KS_DiagKey,		DiagKeyCheck},
		{KC_Aux_ESC,	KS_DiagEEPROM,	DiagEEPROM},
		{KVoid,	 		KS_DiagKey,		Dummy	}	
	};

PROGMEM 	CmdEntry KS_DiagEEPROM_Tab[2] = {
		{KC_ESC,		KS_DiagDDS,		DiagDDS},
		{KVoid,	 		KS_DiagEEPROM,	Dummy	}	
	};

PROGMEM 	CmdEntry KS_DiagDDS_Tab[10] = {
		{KC_ESC,		KS_DiagLCD,		DiagDisplay},
		{KC_0,			KS_DiagDDS,		DiagDDS},
		{KC_1,			KS_DiagDDS,		DiagDDS},
		{KC_2,			KS_DiagDDS,		DiagDDS},
		{KC_3,			KS_DiagDDS,		DiagDDS},
		{KC_4,			KS_DiagDDS,		DiagDDS},
		{KC_5,			KS_DiagDDS,		DiagDDS},
		{KC_6,			KS_DiagDDS,		DiagDDS},
		{KC_9,			KS_DiagDDS,		DiagDDS},
		{KVoid,	 		KS_DiagDDS,		Dummy	}	
	};

PROGMEM 	CmdEntry KS_ShowStep_Tab[4] = {
		{KC_ESC,		KS_CW,			CW_Init},
		{KC_Hz,			KS_ShowStep,	ShowStep},
		{KC_KHz,		KS_ShowStep,	ShowStep},
		{KVoid,	 		KS_ShowStep,	Dummy	}	
	};

PROGMEM 	CmdEntry KS_ShowDuty_Tab[3] = {
		{KC_ESC,		KS_CW,			CW_Init},
		{KC_DP,			KS_ShowDuty,	SetDuty},
		{KVoid,	 		KS_ShowDuty,	Dummy	}	
	};


// -- Key State Index
PROGMEM	CmdEntry	*CmdIndex[KStateLast - 2] = {
	(CmdEntry *)KS_CW_Tab,
	(CmdEntry *)KS_Sweep_Tab,
	(CmdEntry *)KS_ServoPos_Tab,
	(CmdEntry *)KS_ServoRun_Tab,
	(CmdEntry *)KS_ServoParam_Tab,
	(CmdEntry *)KS_ModeSel_Tab,
	(CmdEntry *)KS_DigitInput_Tab,
	(CmdEntry *)KS_DiagLCD_Tab,
	(CmdEntry *)KS_DiagKey_Tab,
	(CmdEntry *)KS_DiagEEPROM_Tab,
	(CmdEntry *)KS_DiagDDS_Tab,
	(CmdEntry *)KS_ShowStep_Tab,
	(CmdEntry *)KS_ShowDuty_Tab,
	};

// Initialization function table
//PROGMEM	CmdFuncPointer InitFuncTab[M_Last] = {
PROGMEM	void (*InitFuncTab[KS_ServoRun + 1])(void) = {
	CW_Init,
	SweepInit,
	ServoPosInit,
	ServoRunInit,
	};

PROGMEM	U8	WaveName[8][6] = {
	{"SINE"},
	{" SQR"},
	{" TRI"},
	{"RMP+"},
	{"RMP-"},
	{"STR+"},
	{"STR-"},
	{"USER"}
	};

PROGMEM	U8	Unit_Symbols[Unit_Last][5] = {
	{"Hz"},
	{"KHz"},
	{"Sec"},
	{"mS"},
	{"V  "},
	{"mV  "},
	{"uSec"},
	{"Deg"}
	};

PROGMEM	U8	ModePrompt[12] = {"Select Mode"};
PROGMEM	U8	ModeNames[ModeNumber][10] = {
	{"CW"},
	{"Sweep"},
	{"Servo Pos"},
	{"Servo Run"}
	};

PROGMEM	U8	ModeIndex[ModeNumber][5] = {
	{"1/4"},
	{"2/4"},
	{"3/4"},
	{"4/4"}
	};

PROGMEM	U8	SweepParams[6][7] = {
	{"Start:"},
	{"Stop:"},
//	{"Centr:"},
//	{"Span:"},
	{"Time:"},
	{"Tstep:"},
	{"Amp:"},
	{"Ofs:"}
	};

PROGMEM	U8	SweepParamUnit[4][3] = {
	{"Hz"},
	{"Hz"},
//	{"Hz"},
//	{"Hz"},
	{"mS"},
	{"mS"}
	};

PROGMEM	U8	TswTitle[10] = {"Tsw Step:"};

PROGMEM	U8	ServoPos[17] = {"Servo        Pos"};
PROGMEM	U8	ServoPW[4] = {"PW:"};
PROGMEM	U8	ServoAmp[5] = {"AMP:"};

PROGMEM	U8	ServoRun[17] = {"Servo        Run"};
PROGMEM	U8	ServoRunState[3][8] = {
	{"Ready"},
	{"Running"},
	{"Hold"}
	};
PROGMEM	U8	SetFreqPeriodStep[15] = {"Set F/T step"};
PROGMEM	U8	FreqStep[15] = {"F step size:"};
PROGMEM	U8	TimeStep[15] = {"T step size:"};

PROGMEM	U8	Duty[10] = {"Duty (%)"};


PROGMEM	U8	ServoParamDefaultRestore[17] = {"[WF] to restore"};

PROGMEM	U8	ServoParamNames[ServoParamNumber][12] = {
	{"SV.PWmin"},
	{"SV.PWmax"},
	{"SV.PWmid"},
	{"SV.PWinc"},
	{"SV.RunStep"},
	{"SV.RunRate"},
	{"SV.Cycle"},
	{"SV.AMPmax"},
	{"SV.Default"}
	};

PROGMEM	U8	ServoParamIndex[ServoParamNumber][5] = {
	{"1/9"},
	{"2/9"},
	{"3/9"},
	{"4/9"},
	{"5/9"},
	{"6/9"},
	{"7/9"},
	{"8/9"},
	{"9/9"}
	};

PROGMEM	U8	ServoParamUnits[ServoParamNumber][7] = {
	{"uSec"},
	{"uSec"},
	{"uSec"},
	{"uSec"},
	{"uSec"},
	{"S/Step"},
	{"uSec"},
	{"V"},
	{""}
	};

// Focus location in LCD buffer
PROGMEM 	U8	FocusLocTab[3] = {
	0, 16, 25
	};

PROGMEM	FuncGen	miniDDS_ROM = {
		KS_CW,			// Mode
		1000,			// Freq
		SetType_Freq,		// Frequency setting type
		Unit_Hz,			// Frequency unit
		5000,			// Amp
		7500,			// Ofs
		1,				// FreqIncStep
		1,				// CycleIncStep
		WF_Sine,			// Waveform
		1500,			// ServoPWcurr;
		5000,			// ServoAMPcurr;
		1000,			// ServoPWmin;
		2000,			// ServoPWmax;
		1500,			// ServoPWmid;
		100,				// ServoPWinc;
		100,				// ServoRunStep;
		100,				// ServoRunRate;
		20000,			// ServoCycle;
		5000,			// ServoAMPmax;
		1000,			//	SweepStartFreq;
		100000L,			//	SweepStopFreq;
//		49500L,			//	SweepCentertFreq;
//		99000L,			//	SweepSpanFreq;
		5000,			//	SweepTime;
		1,				//   	SweepTswStep;
		' ',				// SweepDir  
		50				// Duty cycle
		};


FuncGen	miniDDS;
State	SysState;
DataEntry	NumInput;
DigitString	NumString;

#define	InputBufSize		16
U8	InputBuf[InputBufSize];
U8	CurrFocus;
U8	CurrMode;
U8	CurrParam;
U8	TrigCtrl;

U8	Signature_EE1 __attribute__ ((section (".eeprom"))) = {0xFF};
U8	Signature_EE2 __attribute__ ((section (".eeprom"))) = {0xFF};
U8	EEversion __attribute__ ((section (".eeprom"))) = {0xFF}; // Please set EEversion in FuncGen.h
FuncGen	miniDDS_EE __attribute__ ((section (".eeprom"))) = {}; 

// ==========================================================
//	Function Definitions
// ==========================================================
void	DoCmdProcessing(void)
{
 if(KeyCode != 0) {
	// -- Analyses key and execute associated key function
	(*(void (*)(void) )CmdAnalysis( 
		KeyCode, 
		(CmdEntry *)pgm_read_word(&CmdIndex[SysState.CurrState]),
		(U8 *)&SysState.CurrState))();
	KeyCode = 0;
	}
}

// ----------------------------------------------------------------------------
// Resolve a command function with given 'CmdCode' and 'CmdTab'. Update 'PState' 
//	with NextState and return the pointer to the resolved command function
// ----------------------------------------------------------------------------
//
void	*CmdAnalysis(U8 Ccode, CmdEntry *CmdTab, U8 *PState)
{
 U8	tmp0;
 
 // -- Looking for match command code
 while((tmp0 = pgm_read_byte((U8 *)CmdTab)) != 0) {
	if(tmp0 == Ccode) {
		// -- Match found
		SysState.PrevState = *PState;
		*PState = pgm_read_byte((U8 *)CmdTab + 1);
		return ((void *)pgm_read_word((U8 *)CmdTab + 2));
		}
	else {
		// -- Proceed to next entry
		CmdTab = (CmdEntry *)((U8 *)CmdTab + 4);
		}
 	}

 return Dummy;
}


void AppInit(void)
{
 U8	tmp,  tmp1, tmp2;
 
 KeyCode = 0;
 KFlags = 0x00;
 Flags = 0x00;

 // Set X9C102 to 0 position
 SetDigitalPot(0);
 PrevWiperPos = 0;

 // Looking for EEPROM signature
 tmp = EE_Read((U16)&Signature_EE1);
 tmp1 = EE_Read((U16)&Signature_EE2);
 tmp2 = EE_Read((U16)&EEversion);
 
 if((tmp == EE_signature1) && (tmp1 == EE_signature2) && (tmp2 == EepromVersion)) {
 	// Load setting from EEPROM
 	tmp = 0;
	while(tmp < sizeof(FuncGen)) {
		*((U8*)&miniDDS + tmp) = EE_Read((U16)&miniDDS_EE + tmp);
		tmp++;
		}
 	}
 else {
 	// Load setting from Flash
 	tmp = 0;
	while(tmp < sizeof(FuncGen)) {
		*((U8*)&miniDDS + tmp) 
			= pgm_read_byte((U8 *)&miniDDS_ROM + tmp);
		tmp++;
		}

	// Setup EEPROM
	SaveSetting();

	// Write signature & version
 	EE_Write((U16)&Signature_EE1, EE_signature1);
 	EE_Write((U16)&Signature_EE2, EE_signature2);
 	EE_Write((U16)&EEversion, EepromVersion);
 	}

 Delay(10000);

 if(miniDDS.CurrWave == WaveMax - 1) {
 	// Load user waveform
//	LoadUserWaveform();
	}
 
 // Execute initialization based on Mode
 if(miniDDS.Mode >= ModeNumber) {
 	miniDDS.Mode = KS_CW;
 	}
 (*(void (*)(void))(pgm_read_word(&InitFuncTab[miniDDS.Mode])))();
 
}

void	CW_Init(void)
{
 SysState.CurrState = KS_CW;
 CurrMode = KS_CW;
 CurrFocus = FC_Freq;
 TrigCtrl = '>';	

 ClearDisp();
 ShowType();
 ShowFreqPeroid();
 ShowAmp(AmpPos_CW);
 ShowOfs(OfsPos_CW);
 ShowFocus();
}

void	SweepInit(void)
{
 SysState.CurrState = KS_Sweep;
 CurrMode = KS_Sweep;
 CurrFocus = FC_SW_StartFreq;
 TrigCtrl = '>';	

 ClearDisp();
 ShowSweep();
}

void	ServoPosInit(void)
{
 SysState.CurrState = KS_ServoPos;
 CurrMode = KS_ServoPos;
 CurrFocus = FC_ServoPW;

 SetDigitalPot((U8)(((U32)miniDDS.ServoAMPcurr * 100)/ServoAmpMax)); 
 
 // Set offset to half of Servo Amp
 UpdateOfs((U32)miniDDS.ServoAMPcurr/2 + OFSmax/2);

// ClearDisp();
// PutStr_ROM(ServoPos_DispStart, (U8 *)ServoPos);
 ShowPW();
}

void	ServoRunInit(void)
{
 SysState.CurrState = KS_ServoRun;
 CurrMode = KS_ServoRun;
 CurrFocus = ServoState_Ready;

 SetDigitalPot((U8)(((U32)miniDDS.ServoAMPcurr * 100)/ServoAmpMax)); 
 
 // Set offset to half of Servo Amp
 UpdateOfs((U32)miniDDS.ServoAMPcurr/2 + OFSmax/2);

 ShowServoRun();
}

void	ChangeSVParam(void)
{
 PutStr_ROM_coded();

 CurrParam = 0;
 ShowSVParam();
}

void	SetSVParam(void)
{
 // Accept input
 NumInputSetup(ServoParam_DispStart + ServoParam_DigitOfs,  ServoParam_InputBufSize, KS_ServoParam, ServoParamInputHandle);
/* 
 NumInput.DispStart = ServoParam_DispStart + ServoParam_DigitOfs;
 NumInput.DispLen = ServoParam_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_ServoParam;
 NumInput.ExitAction = ServoParamInputHandle;
 
 ClearInputBuf();
*/
 PutStr_ROM_coded();

 DisplayInputBuf();
}

void	SVParamDefault(void)
{
 U8 tmp, tmp1;

 if(CurrParam != ServoParamNumber - 1) {
 	return;
 	}
 
 // Servo parameter size
 tmp1 = (U16)&miniDDS.ServoAMPmax - (U16)&miniDDS.ServoPWcurr + 2;
 u0putc(tmp1);
 
 // Load setting from Flash
 tmp = 0;
 while(tmp < tmp1) {
		*((U8*)&miniDDS.ServoPWcurr + tmp) 
			= pgm_read_byte((U8 *)&miniDDS_ROM.ServoPWcurr + tmp);
		tmp++;
		}

 // Setup EEPROM
 SaveSetting();
 ServoPosInit();
}

void	ExitSVParam(void)
{
 // Execute initialization based on Mode
 (*(void (*)(void))(pgm_read_word(&InitFuncTab[miniDDS.Mode])))();
}

void	ChangeMode(void)
{
 CurrMode = KS_CW;
 ShowMode();
}

void	SelectMode(void)
{
 miniDDS.Mode = CurrMode;
 SaveSetting();

 PutStr_ROM_coded();
 
 // Execute initialization based on Mode
 (*(void (*)(void))(pgm_read_word(&InitFuncTab[miniDDS.Mode])))();

}

void	EscapeMode(void)
{
 // Go back to current mode
 (*(void (*)(void))(pgm_read_word(&InitFuncTab[miniDDS.Mode])))();
}

void Dummy(void)
{

}

void	FreqKey(void)
{
 if((miniDDS.Mode == KS_CW) && (NumInput.ExitAction == FreqInputHandle)) {
 	SetFreqCW();
 	}
 if(miniDDS.Mode == KS_Sweep) {
 	// Scroll focus to next parameter
 	CurrFocus++;
 	if(CurrFocus > FC_SW_Time) {
 		CurrFocus = FC_SW_StartFreq;
 		}
 	SetSweepParam();
 	}
}

void	SetFreqCW(void)
{
 // Only execute the below at CW Mode.
// if(miniDDS.Mode != KS_CW) {
// 	return;
//	}

 // Set focus to FREQ if it was not 
 if(CurrFocus != FC_Freq) {
 	CurrFocus = FC_Freq;
	ShowFocus();
//	SysState.CurrState = SysState.PrevState;
	return;
 	}
 
 switch(DispBuf[Freq_DispStart]) {
 	case 'F': 
		DispBuf[Freq_DispStart] = 'T';
		break;
	case 'T':
	default:
		DispBuf[Freq_DispStart] = 'F';
		break;
 	}
 DispBuf[1] = ':';

 // Accept input
 NumInputSetup(Freq_DispStart + Freq_DigitOfs, Freq_InputBufSize, KS_CW, FreqInputHandle);
/*
 NumInput.DispStart = Freq_DispStart + Freq_DigitOfs;
 NumInput.DispLen = Freq_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_CW;
 NumInput.ExitAction = FreqInputHandle;
 
 ClearInputBuf();
*/
 DisplayInputBuf();
}

void	SetSweepParam(void)
{
 // Set focus if not previously at sweep param
 if((CurrFocus < FC_SW_StartFreq) || (CurrFocus > FC_SW_Tstep)) {
	CurrFocus = FC_SW_StartFreq;
	ShowSweep();
	return;
 	}
 
// SysState.CurrState = KS_DigitInput;
 PutStr_ROM( SwParam_DispStart, (U8*)SweepParams[CurrFocus - FC_SW_StartFreq]);
 
 // Accept input
 NumInputSetup(SwParam_DispStart + SwParam_DigitOfs, SwParam_InputBufSize, KS_Sweep, SweepParamInputHandle);
 DisplayInputBuf();
}

void	QuickSweepParam(void)
{
 if((CurrFocus - FC_SW_StartFreq + 1) != (KeyParam & 0x0F)) {
	CurrFocus = FC_SW_StartFreq - 1 + (KeyParam & 0x0F);
	ShowSweep();
	return;
 	}
 
// SysState.CurrState = KS_DigitInput;
 PutStr_ROM( SwParam_DispStart, (U8*)SweepParams[CurrFocus - FC_SW_StartFreq]);
 
 // Accept input
 NumInputSetup(SwParam_DispStart + SwParam_DigitOfs, SwParam_InputBufSize, KS_Sweep, SweepParamInputHandle);
 DisplayInputBuf();
}

void	SetSweepAmp(void)
{
 // Set focus if not previously at sweep param
 if((CurrFocus != FC_SW_Amp)) {
	CurrFocus = FC_SW_Amp;
	ShowSweep();
	return;
 	}
// SysState.CurrState = KS_DigitInput;

 // Accept input
 NumInputSetup(SwAmp_DispStart + SwAmp_DigitOfs, SwAmp_InputBufSize, KS_Sweep, AmpInputHandle);
/* 
 NumInput.DispStart = SwAmp_DispStart + SwAmp_DigitOfs;
 NumInput.DispLen = SwAmp_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_Sweep;
 NumInput.ExitAction = AmpInputHandle;
 
 ClearInputBuf();
*/
 DisplayInputBuf();
}

void	SetSweepOfs(void)
{
 // Set focus if not previously at sweep param
 if((CurrFocus != FC_SW_Ofs)) {
	CurrFocus = FC_SW_Ofs;
	ShowSweep();
	return;
 	}
// SysState.CurrState = KS_DigitInput;

 // Accept input
 NumInputSetup(SwOfs_DispStart + SwOfs_DigitOfs, SwOfs_InputBufSize, KS_Sweep, OfsInputHandle);
/* 
 NumInput.DispStart = SwOfs_DispStart + SwOfs_DigitOfs;
 NumInput.DispLen = SwOfs_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_Sweep;
 NumInput.ExitAction = OfsInputHandle;
 
 ClearInputBuf();
*/
 PutStr_ROM_coded();

 DisplayInputBuf();


}

void	SetAmp(void)
{
 // Set focus 
 if(CurrFocus != FC_Amp) {
 	CurrFocus = FC_Amp;
	ShowFocus();
//	SysState.CurrState = SysState.PrevState;
	return;
 	}

 // Accept input
 NumInputSetup(Amp_DispStart + Amp_DigitOfs, Amp_InputBufSize, KS_CW, AmpInputHandle);
/* 
 NumInput.DispStart = Amp_DispStart + Amp_DigitOfs;
 NumInput.DispLen = Amp_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_CW;
 NumInput.ExitAction = AmpInputHandle;
 
 ClearInputBuf();
*/
 DisplayInputBuf();
}

void	SetOfs(void)
{
 // Set focus 
 if(CurrFocus != FC_Ofs) {
 	CurrFocus = FC_Ofs;
	ShowFocus();
//	SysState.CurrState = SysState.PrevState;
	return;
 	}

 // Accept input
 NumInputSetup(Ofs_DispStart + Ofs_DigitOfs, Ofs_InputBufSize, KS_CW, OfsInputHandle);
/* 
 NumInput.DispStart = Ofs_DispStart + Ofs_DigitOfs;
 NumInput.DispLen = Ofs_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_CW;
 NumInput.ExitAction = OfsInputHandle;
 
 ClearInputBuf();
*/
 DisplayInputBuf();
}

void	SetPW(void)
{
 // Set focus 
 if(CurrFocus != FC_ServoPW) {
 	CurrFocus = FC_ServoPW;
	ShowPW();
//	SysState.CurrState = SysState.PrevState;
	return;
 	}

 // Accept input
 NumInputSetup(ServoPosPW_DispStart + ServoPosPW_DigitOfs, ServoPosPW_InputBufSize, KS_ServoPos, PWInputHandle);
/* 
 NumInput.DispStart = ServoPosPW_DispStart + ServoPosPW_DigitOfs;
 NumInput.DispLen = ServoPosPW_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_ServoPos;
 NumInput.ExitAction = PWInputHandle;
 
 ClearInputBuf();
*/
 DisplayInputBuf();
}

void	SetSVAmp(void)
{
 // Set focus 
 if(CurrFocus != FC_ServoAmp) {
 	CurrFocus = FC_ServoAmp;
	ShowServoAmp();
//	SysState.CurrState = SysState.PrevState;
	return;
 	}

 // Accept input
 NumInputSetup(ServoPosAmp_DispStart + ServoPosAmp_DigitOfs,  ServoPosAmp_InputBufSize, KS_ServoPos, ServoAmpInputHandle);
/* 
 NumInput.DispStart = ServoPosAmp_DispStart + ServoPosAmp_DigitOfs;
 NumInput.DispLen = ServoPosAmp_InputBufSize;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = KS_ServoPos;
 NumInput.ExitAction = ServoAmpInputHandle;
 
 ClearInputBuf();
*/
 PutStr_ROM_coded();

 DisplayInputBuf();
}

void	SetIncrement(void)
{
 // Only execute the below at CW Mode
 if(miniDDS.Mode != KS_CW) {
 	return;
 	}

 // Display title
 ClearDisp();
 PutStr_ROM(SetStep_DispStart, (U8 *)SetFreqPeriodStep);

 // Accept input
 NumInputSetup(Inc_DispStart + Inc_DigitOfs, Inc_InputBufSize, KS_CW, IncInputHandle);

 Digit();
 DisplayInputBuf();
}

void	SetDuty(void)
{
 // Display title
 ShowDuty();
// ClearDisp();
// PutStr_ROM(0, (U8 *)Duty);

 // Accept input
 NumInputSetup(17, 5, KS_ShowDuty, DutyInputHandle);

 DisplayInputBuf();
}

void	NumInputSetup(U8 dispstart, U8 displen, U8 exittostate, void (*exitaction)(void))
{
 SysState.CurrState = KS_DigitInput;

 // Accept input
 NumInput.DispStart = dispstart;
 NumInput.DispLen = displen;
 NumInput.InBuf = InputBuf;
 NumInput.DigitCnt = 0;
 NumInput.Sign = 0;
 NumInput.DPLoc = 0;
 NumInput.ExitTo = exittostate;
 NumInput.ExitAction = exitaction;
 
 ClearInputBuf();
}

void	StartStop(void)
{
 if(CurrFocus == 0x01) {
 	// Already in running state. Change to hold state`
 	CurrFocus = 0x02;
 	}
 else {
 	// Set to running state
 	CurrFocus = 0x01;
 	}
// CurrFocus ^= 0x01;

 PutStr_ROM_coded();

 ShowServoRun();
}

void	CAN(void)
{
 U8	tmp;
 
 // Backspace if digit entered
 if(NumInput.DigitCnt > 0) {
 	NumInput.DigitCnt--;
 	// Shift right1 digit
 	tmp = NumInput.DispLen - 1;
 	while(tmp) {
 		*(NumInput.InBuf + tmp + 1) = *(NumInput.InBuf + tmp);
		tmp--;
 		}

 	// Make first digit blank
 	*(NumInput.InBuf) = Char_Blank;
	DisplayInputBuf();
	return;
 	}

 PutStr_ROM_coded();
 
 NumInput.EndKey = KC_ESC;

 SysState.CurrState = NumInput.ExitTo;

 // Execute handling function
 (*(void (*)(void))(NumInput.ExitAction))();
}

void	UnitButton(void)
{
 if(NumInput.DigitCnt == 0) {
 	NumInput.EndKey = KC_ESC;
 	}
 else {
 	NumInput.EndKey = KeyCodeBuf;
 	}

 SysState.CurrState = NumInput.ExitTo;

 // Execute handling function
 (*(void (*)(void))(NumInput.ExitAction))();
}


void	TYPE(void)
{
 miniDDS.CurrWave++;
 if(miniDDS.CurrWave >= WaveMax) {
 	miniDDS.CurrWave = 0;
 	}

 EE_Write((U16)&miniDDS_EE.CurrWave, miniDDS.CurrWave);
 
 ShowType();

 // Update display first because load user waveform may take time
 UpdateLCD(0);
// BitSet8(Flags, (1 << Flags_DispUpdate));

 if(miniDDS.CurrWave == WaveMax - 1) {
 	// Load user waveform
	LoadUserWaveform();
	}

 if(miniDDS.Mode == KS_CW) {
 	SendDDS();
 	}
 if(miniDDS.Mode == KS_Sweep) {
 	SendSweep();
 	}

}

void ClearDisp(void)
{
 FillDisp(0, 32, ' ');
}

void	Digit(void)
{
 U8	tmp;

 // Shift left 1 digit
 tmp = 0;
 while(tmp < NumInput.DispLen - 1) {
 	*(NumInput.InBuf + tmp) = *(NumInput.InBuf + tmp + 1);
	tmp++;
 	}

 // Enter new digit
 *(NumInput.InBuf + tmp) = KeyParam;

 // Count up
 NumInput.DigitCnt++;
 if(NumInput.DigitCnt > NumInput.DispLen) {
 	NumInput.DigitCnt = NumInput.DispLen;
 	}

 // Display input digits
 DisplayInputBuf();
}

U8	CalWholePart(void)
{
 U8	tmp1, tmp2;
 U32	tmp3;

 if(NumInput.DigitCnt == 0) {
 	return 0;
 	}

 tmp3 = 0;
 tmp1 = 0;
 while(tmp1 < NumInput.DispLen) {
 	tmp2 = *(NumInput.InBuf + tmp1);
	switch(tmp2) {
		case '_':
			break;

		case '.':
			// Only take the whole number, drop fraction
 			NumInput.DataValue = tmp3;
 			return 1;

		default:
			// Digit 0 - 9
			tmp3 *= 10;
			tmp2 = tmp2 & 0x0F;
			tmp3 += tmp2;
			break;
		}
	tmp1++;
	}
	
 NumInput.DataValue = tmp3;
 return 1;
}

U8	CalValue(void)
{
 U8	tmp1, tmp2, tmp4;
 U32	tmp3;

 if(NumInput.DigitCnt == 0) {
 	return 0;
 	}
 
 tmp3 = 0;	// result
 tmp4 = 0; 	// Decimal point location 
 tmp1 = 0;
 // Scan input buffer
 while(tmp1 < NumInput.DispLen) {
 	if(tmp4) {
		// Decimal point has been encountered. Limit to 3 digits after DP
		if((tmp1 - tmp4) > 3) {
			// Stop scaning
			break;
			}
 		}
	
 	tmp2 = *(NumInput.InBuf + tmp1);
	switch(tmp2) {
		case '_':
			break;

		case '.':
			if(tmp4 == 0) {
				// Keep the first decimal point location
				tmp4 = tmp1;
				}
			else {
				// Error. Two decimal point. Exit. 
				return 0;
				}
			break;

		default:
			// Digit 0 - 9
			tmp3 *= 10;
			tmp2 = tmp2 & 0x0F;
			tmp3 += tmp2;
			break;
		}
	tmp1++;
	}

 if(tmp4 != 0) {
 	NumInput.DPLoc = (tmp1 - 1) - tmp4;	// DPloc is the digits after decimal point
 	}

 if((NumInput.DPLoc == 0) && (tmp3 > ((4294967296L/1000) + 1) )) {
 	// Possible overflow
	return 0;
 	}
 
 tmp1 = 3 - NumInput.DPLoc;
 while(tmp1) {
	tmp3 *= 10;
	tmp1--;
	}
 NumInput.DataValue = tmp3;
 return 1;
}

void	ToggleSign(void)
{
 if(NumInput.Sign != Sign_Neg) {
 	// Set to '-'
 	NumInput.Sign = Sign_Neg;
 	}
 else {
 	// Set to '+'
 	NumInput.Sign = Sign_Pos;
 	}
 DisplayInputBuf();
}

void	Inc(void)
{
 if(SysState.CurrState == KS_CW) {
 	switch(CurrFocus) {
		case FC_Freq:
			if(miniDDS.SetType == SetType_Freq) {
 				miniDDS.Freq += miniDDS.FreqIncStep;
				}
			else {
 				miniDDS.Freq += miniDDS.CycleIncStep;
				}
 			ShowFreqPeroid();
			break;

		case FC_Amp:
 			miniDDS.Amp += 100;
 			if(miniDDS.Amp > AMPmax) {
				miniDDS.Amp = AMPmax;
				}
 			ShowAmp(AmpPos_CW);
			break;

		case FC_Ofs:
 			miniDDS.Ofs += 100;
 			if(miniDDS.Ofs > OFSmax) {
				miniDDS.Ofs = OFSmax;
				}
 			ShowOfs(OfsPos_CW);
		default:
			break;
 		}
	SaveSetting();
 	}

 if((SysState.CurrState == KS_Sweep) && (CurrFocus >= FC_SW_StartFreq) && (CurrFocus <= FC_SW_Tstep)) {
 	CurrFocus++;
	if(CurrFocus >= FC_SW_Tstep + 1) {
		CurrFocus = FC_SW_Tstep;
		}
	ShowSweep();
 	}

 if(SysState.CurrState == KS_ServoPos) {
 	switch(CurrFocus) {
		case FC_ServoPW:
			miniDDS.ServoPWcurr += miniDDS.ServoPWinc;
			if(miniDDS.ServoPWcurr > miniDDS.ServoPWmax) {
				miniDDS.ServoPWcurr = miniDDS.ServoPWmax;
				}
			ShowPW();
			break;

		case FC_ServoAmp:
			if(miniDDS.ServoAMPcurr < miniDDS.ServoAMPmax - 100) {
				miniDDS.ServoAMPcurr += 100;
				}
			else {
				miniDDS.ServoAMPcurr = miniDDS.ServoAMPmax;
				}
			ShowServoAmp();
		default:
			break;
 		}
	SaveSetting();
 	}

 if(SysState.CurrState == KS_ServoParam) {
	CurrParam++;
	if(CurrParam >= ServoParamNumber) {
		CurrParam = ServoParamNumber - 1;
		}
	ShowSVParam();
 	}
 
 if(SysState.CurrState == KS_ModeSel) {
	CurrMode++;
	if(CurrMode >= ModeNumber) {
		CurrMode = ModeNumber - 1;
		}
	ShowMode();
 	}
}

void	Dec(void)
{
 if(SysState.CurrState == KS_CW) {
 	switch(CurrFocus) {
		case FC_Freq:
			if(miniDDS.SetType == SetType_Freq) {
				if(miniDDS.Freq >= miniDDS.FreqIncStep) {
					miniDDS.Freq -= miniDDS.FreqIncStep;
					}
				}
			else {
				if(miniDDS.Freq > miniDDS.CycleIncStep) {
					miniDDS.Freq -= miniDDS.CycleIncStep;
					}
				}
 			ShowFreqPeroid();
			break;

		case FC_Amp:
 			if(miniDDS.Amp >= 100) {
				miniDDS.Amp -= 100;
				}
 			ShowAmp(AmpPos_CW);
			break;

		case FC_Ofs:
 			if(miniDDS.Ofs >= 100) {
				miniDDS.Ofs -= 100;
				}
 			ShowOfs(OfsPos_CW);
		default:
			break;
 		}
	SaveSetting();
 	}

 if((SysState.CurrState == KS_Sweep) && (CurrFocus >= FC_SW_StartFreq) && (CurrFocus <= FC_SW_Tstep)) {
 	CurrFocus--;
	if(CurrFocus < FC_SW_StartFreq) {
		CurrFocus = FC_SW_StartFreq;
		}
	ShowSweep();
 	}

 if(SysState.CurrState == KS_ServoPos) {
 	switch(CurrFocus) {
		case FC_ServoPW:
			if(miniDDS.ServoPWcurr > miniDDS.ServoPWmin + miniDDS.ServoPWinc) {
				miniDDS.ServoPWcurr -= miniDDS.ServoPWinc;
				}
			else {
				miniDDS.ServoPWcurr = miniDDS.ServoPWmin;
				}
			ShowPW();
			break;

		case FC_ServoAmp:
			if(miniDDS.ServoAMPcurr > 100) {
				miniDDS.ServoAMPcurr -= 100;
				}
			else {
				miniDDS.ServoAMPcurr = 0;
				}
			ShowServoAmp();
		default:
			break;
 		}
	SaveSetting();
 	}

 if(SysState.CurrState == KS_ServoParam) {
	if(CurrParam > 0) {
		CurrParam--;
		}
	ShowSVParam();
 	}

 if(SysState.CurrState == KS_ModeSel) {
	if(CurrMode > 0) {
		CurrMode--;
		ShowMode();
		}
 	}
}

void	ClearInputBuf(void)
{
 U8	tmp;

 tmp = 0;
 while(tmp < NumInput.DispLen) {
 	*(NumInput.InBuf + tmp) = Char_Blank;
	tmp++;
	}
}

void	DisplayInputBuf(void)
{
 U8	tmp;
 
 tmp = 0;
 while(tmp < NumInput.DispLen) {
 	DispBuf[NumInput.DispStart + tmp ] = *(NumInput.InBuf + tmp );
	tmp++;
 	}

 switch(NumInput.Sign) {
 	case Sign_Blank:
		DispBuf[NumInput.DispStart - 1] = Char_Blank;
		break;
	case Sign_Neg:
		DispBuf[NumInput.DispStart - 1] = '-';
		break;
 	case Sign_Pos:
		DispBuf[NumInput.DispStart - 1] = '+';
		break;
 	}
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	FreqInputHandle(void)
{
 U32 tmp;
 
 tmp = miniDDS.Freq;
 switch(NumInput.EndKey) {
	case KC_Hz:
		if(DispBuf[Freq_DispStart] == 'F') {
			miniDDS.SetType = SetType_Freq;
			miniDDS.SetUnit = Unit_Hz;
			}
		else {
			miniDDS.SetType = SetType_Period;
			miniDDS.SetUnit = Unit_Sec;
			}
		break;
		
	case KC_KHz:
		if(DispBuf[Freq_DispStart] == 'F') {
			miniDDS.SetType = SetType_Freq;
			miniDDS.SetUnit = Unit_KHz;
			}
		else {
			miniDDS.SetType = SetType_Period;
			miniDDS.SetUnit = Unit_mSec;
			}
		break;
		
	case KC_ESC:
	default:
		 ShowFreqPeroid();			
		return;
		break;
	}

 if((miniDDS.SetUnit == Unit_Hz) || (miniDDS.SetUnit == Unit_mSec)) {
	// Take whole part only
	if(CalWholePart()) {
		tmp = NumInput.DataValue;
		}
	}
 else if((miniDDS.SetUnit == Unit_KHz) || (miniDDS.SetUnit == Unit_Sec)){
	// Take whole part and decimal
	if(CalValue()) {
		tmp = NumInput.DataValue;
		}
	}

 // Do not change if 0 was entered for period
 if((miniDDS.SetType != SetType_Period) || (tmp != 0)) {
 	PutFreq((U32 *)&miniDDS.Freq, tmp);
// 	if(tmp > FreqMax) {
//		tmp = FreqMax;
// 		}
//	miniDDS.Freq = tmp;
 	}
 
 SaveSetting();
 ShowFreqPeroid();			
}

void	SweepParamInputHandle(void)
{
 U32 tmp;
 U8	SetType, SetUnit;

 tmp = *(U32 *)((U8 *)&miniDDS.SweepStartFreq + (CurrFocus - FC_SW_StartFreq) * 4);
 switch(NumInput.EndKey) {
	case KC_Hz:
		if((CurrFocus >= FC_SW_StartFreq) && (CurrFocus <= FC_SW_StopFreq)) {
			// Set frequencies
			SetType = SetType_Freq;
			SetUnit = Unit_Hz;
			}
		else {
			// Set time
			SetType = SetType_Period;
			SetUnit = Unit_Sec;
			}
		break;
		
	case KC_KHz:
		if((CurrFocus >= FC_SW_StartFreq) && (CurrFocus <= FC_SW_StopFreq)) {
			// Set frequencies
			SetType = SetType_Freq;
			SetUnit = Unit_KHz;
			}
		else {
			// Set time
			SetType = SetType_Period;
			SetUnit = Unit_mSec;
			}
		break;
		
	case KC_ESC:
	default:
 		ShowSweep();	
		return;
		break;
	}

 if((SetUnit == Unit_Hz) || (SetUnit == Unit_mSec)) {
	// Take whole part only
	if(CalWholePart()) {
		tmp = NumInput.DataValue;
		}
	}
 else if((SetUnit == Unit_KHz) || (SetUnit == Unit_Sec)){
	// Take whole part and decimal
	if(CalValue()) {
		tmp = NumInput.DataValue;
		}
	}

 // Do not change if 0 was entered for time
 // Values (including Sweep Time) are limited by FreqMax and FreqMin in PutFreq()
 if((SetType != SetType_Period) || (tmp != 0)) {
 	PutFreq((U32 *)((U8 *)&miniDDS.SweepStartFreq + (CurrFocus - FC_SW_StartFreq) * 4), tmp);
// 	*(U32 *)((U8 *)&miniDDS.SweepStartFreq + (CurrFocus - FC_SW_StartFreq) * 4) = tmp;
 	}

 if(miniDDS.SweepTswStep > 0x0000FFFF) {
 	miniDDS.SweepTswStep = 0x0000FFFF;
 	}
 
 SaveSetting();
 ShowSweep();			
}

void	AmpInputHandle(void)
{
 switch(NumInput.EndKey) {
	case KC_Hz:
		if(CalValue()) {
			miniDDS.Amp = NumInput.DataValue;
			}
		break;
		
	case KC_KHz:
		// Take whole part only
		if(CalWholePart()) {
			miniDDS.Amp = NumInput.DataValue;
			}
		break;
		
	case KC_ESC:
	default:
		break;
	}
 SaveSetting();
 if(miniDDS.Mode == KS_CW) {
 	ShowAmp(AmpPos_CW);
 	}
 if(miniDDS.Mode == KS_Sweep) {
 	ShowAmp(AmpPos_Sweep);
 	}
}

void	OfsInputHandle(void)
{
 U32	tmp2;

 switch(NumInput.EndKey) {
	case KC_Hz:
		if(CalValue()) {
			tmp2 = NumInput.DataValue;
			}
		break;
		
	case KC_KHz:
		// Take whole part only
		if(CalWholePart()) {
			tmp2 = NumInput.DataValue;
			}
		break;
		
	case KC_ESC:
	default:
//		tmp2 = miniDDS.Ofs;
		goto ShowOffset;
		break;
	}

 if(tmp2 > OFSmax/2) {
	tmp2 = OFSmax/2;
	}

 // Sign handling
 if(NumInput.Sign == Sign_Neg) {
	// Negative
	miniDDS.Ofs = OFSmax/2 - tmp2;
	}
 else {
	// Positive
 	miniDDS.Ofs = OFSmax/2 + tmp2;
	}

 ShowOffset:		
 if(miniDDS.Mode == KS_CW) {
 	ShowOfs(OfsPos_CW);
 	}
 if(miniDDS.Mode == KS_Sweep) {
 	ShowOfs(OfsPos_Sweep);
 	}
 SaveSetting();
}

void	PWInputHandle(void)
{
 switch(NumInput.EndKey) {
	case KC_Hz:
		// Take whole part only
		if(CalWholePart()) {
			miniDDS.ServoPWcurr  = NumInput.DataValue;
			}
		break;
		
	case KC_KHz:
		if(CalValue()) {
 			miniDDS.ServoPWcurr = NumInput.DataValue;
			}
		break;
		
	case KC_ESC:
	default:
		break;
	}

 ShowPW();
 SaveSetting();
}

void	ServoAmpInputHandle(void)
{
 switch(NumInput.EndKey) {
	case KC_KHz:
		// Take whole part only
		if(CalWholePart()) {
			miniDDS.ServoAMPcurr  = NumInput.DataValue;
			}
		break;
		
	case KC_Hz:
		if(CalValue()) {
 			miniDDS.ServoAMPcurr = NumInput.DataValue;
			}
		break;
		
	case KC_ESC:
	default:
		break;
	}

 ShowServoAmp();
 SaveSetting();
}

void	ServoParamInputHandle(void)
{
 U16 *ptmp;

 ptmp = &miniDDS.ServoPWmin;
 switch(NumInput.EndKey) {
	case KC_Hz:
		switch(CurrParam) {
			case SV_PWmin:
			case SV_PWmax:
			case SV_PWmid:
			case SV_PWinc:
			case SV_Cycle:
			case SV_RunStep:
				// Take whole part only
				if(CalWholePart()) {
					*(ptmp + CurrParam) = NumInput.DataValue;
					}
				break;

			case SV_AMPmax:
			case SV_RunRate:
				if(CalValue()) {
					*(ptmp + CurrParam) = NumInput.DataValue;
					}
			default:
				break;
			}
		break;
		
	case KC_KHz:
		switch(CurrParam) {
			case SV_PWmin:
			case SV_PWmax:
			case SV_PWmid:
			case SV_PWinc:
			case SV_Cycle:
			case SV_RunStep:
				// Take whole part only
				if(CalValue()) {
					*(ptmp + CurrParam) = NumInput.DataValue;
					}
				break;

			case SV_AMPmax:
			case SV_RunRate:
				if(CalWholePart()) {
					*(ptmp + CurrParam) = NumInput.DataValue;
					}
			default:
				break;
			}
		break;
	case KC_ESC:
	default:
		break;
	}

 SaveSetting();
 ShowSVParam();
}

/*
void	TswInputHandle(void)
{

 switch(NumInput.EndKey) {
	case KC_Hz:
		break;
		
	case KC_KHz:
		// Take whole part only
		if(CalWholePart()) {
			if((NumInput.DataValue != 0) && (NumInput.DataValue < 65536)) {
				miniDDS.SweepTswStep = NumInput.DataValue;
				}
			}
		break;
	case KC_ESC:
	default:
		break;
	}

 SaveSetting();
 ShowTsw();
}
*/

void	IncInputHandle(void)
{
 switch(NumInput.EndKey) {
	case KC_Hz:
	case KC_KHz:
		// Frequency or cycle increment input
		// Take whole part only
		if(CalWholePart()) {
			if(NumInput.DataValue != 0) {
				switch(NumInput.EndKey) {
					case KC_Hz:
						miniDDS.FreqIncStep = NumInput.DataValue;
						break;
					case KC_KHz:
						miniDDS.CycleIncStep = NumInput.DataValue;
					default:
						break;
					}
				}
			}
		break;
/*		
	case KC_KHz:
		// Cycle increment input
		// Take whole part only
		if(CalWholePart()) {
			if(NumInput.DataValue != 0) {
				miniDDS.CycleIncStep = NumInput.DataValue;
				}
			}
		break;
*/		
	case KC_ESC:
	default:
		break;
	}

 SaveSetting();
 ClearDisp();
 ShowType();
 ShowFreqPeroid();
 ShowAmp(AmpPos_CW);
 ShowOfs(OfsPos_CW);
 ShowFocus();
}

void	DutyInputHandle(void)
{
 switch(NumInput.EndKey) {
	case KC_Hz:
	case KC_KHz:
		if(CalWholePart()) {
			if(NumInput.DataValue > 100) {
				NumInput.DataValue = 100;
				}
			miniDDS.Duty = NumInput.DataValue;
			EE_Write((U16)&miniDDS_EE.Duty, miniDDS.Duty);
			}
	case KC_ESC:
	default:
		break;
	}
 ShowDuty();
}

void	ShowMode(void)
{
 ClearDisp();
 PutStr_ROM(ModePrompt_DispStart, (U8 *)ModePrompt);
 PutStr_ROM(ModeName_DispStart, (U8 *)ModeNames[CurrMode]);
 PutStr_ROM(ModeIndex_DispStart, (U8 *)ModeIndex[CurrMode]);
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	ShowFreqPeroid(void)
{
 U8 tmp, tmp1;
 
 BinToDec32(miniDDS.Freq, (U8*)Strbuf);

 FillDisp(2, 10, ' ');
/*
 tmp = 0;
 while(tmp < 10) {
	DispBuf[2 + tmp] = ' ';
	tmp++;
 	}
*/

// DispBuf[0] = (SetType) ? 'T' : 'F';
// DispBuf[1] = '=';

 tmp1 = 9;
 if((miniDDS.SetUnit == Unit_KHz) ||(miniDDS.SetUnit == Unit_Sec)) {
 	tmp1 = 6;
 	}
 // Remove leading zeros
 tmp = 4;
 while(tmp < tmp1)  {
	// Do not display leading 0's
	if(Strbuf[tmp] == '0') {
		Strbuf[tmp] = ' ';
		}
	else {
		break;
		}
	tmp++;
	}

// DispBuf[0] = ' ';
 DispBuf[1] = Strbuf[4];
 DispBuf[2] = Strbuf[5];
 DispBuf[3] = Strbuf[6];
 if((miniDDS.SetUnit == Unit_Hz) ||(miniDDS.SetUnit == Unit_mSec)) {
 	DispBuf[4] = (tmp < 7) ?  ',' : ' ';
 	}
 else {
	DispBuf[4] = '.';
 	}
 DispBuf[5] = Strbuf[7];
 DispBuf[6] = Strbuf[8];
 DispBuf[7] = Strbuf[9];

 PutStr_ROM(8, (U8*)Unit_Symbols[miniDDS.SetUnit]);

 ShowFocus();

 BitSet8(Flags, (1 << Flags_DispUpdate));

 SendDDS();
}

void	ShowSweep(void)
{
// U8 tmp, tmp1;

 ClearDisp();

 // Display title
 PutStr_ROM(0, (U8*)ModeNames[KS_Sweep]);

 // Show Trig Control state
 if(TrigCtrl == '*') {
	DispBuf[6] = '*';
 	}
 // Show Sweep direction mode
 DispBuf[7] = miniDDS.SweepDir;

 // Display wavefore type
 ShowType();
 
 // Display parameter name
 PutStr_ROM(16, (U8*)SweepParams[CurrFocus - FC_SW_StartFreq]);
 
 // Display parameter unit
 if((CurrFocus >= FC_SW_StartFreq) && (CurrFocus <= FC_SW_Tstep)) {
 	PutStr_ROM(30, (U8*)SweepParamUnit[CurrFocus - FC_SW_StartFreq]);
 	DispWholeNum(22, 7, (U32 *)((U8 *)&miniDDS.SweepStartFreq + (CurrFocus - FC_SW_StartFreq) * 4));
 	}

 if(CurrFocus == FC_SW_Amp) {
 	ShowAmp(AmpPos_Sweep);
 	}
 else {
	// Set amplitude
 	SetDigitalPot((U8)((miniDDS.Amp * 100)/AMPmax)); 
 	}
 
 if(CurrFocus == FC_SW_Ofs) {
 	ShowOfs(OfsPos_Sweep);
 	}
 else {
	// Set offset
	UpdateOfs(miniDDS.Ofs);
 	}

 BitSet8(Flags, (1 << Flags_DispUpdate));

 SendSweep();
}


void	ShowAmp(U8 startpos)
{
 if(miniDDS.Amp > AMPmax) {
// 	Beep(ErrBeepLen);
 	miniDDS.Amp = AMPmax;
 	}
 
 BinToDec32(miniDDS.Amp, (U8*)Strbuf);
 
// DispBuf[16] = 'A';
// DispBuf[17] = '=';
 
// DispBuf[16] = ' ';
 DispBuf[startpos + 0] = (Strbuf[5] == '0') ? ' ' : Strbuf[5];
 DispBuf[startpos + 1] = Strbuf[6];
 DispBuf[startpos + 2] = '.';
 DispBuf[startpos + 3] = Strbuf[7];
 DispBuf[startpos + 4] = ' ';
 DispBuf[startpos + 5] = 'V';
 DispBuf[startpos + 6] = ' ';

 if(miniDDS.Mode == KS_CW) {
 	ShowFocus();
 	}
 BitSet8(Flags, (1 << Flags_DispUpdate));

 SetDigitalPot((U8)((miniDDS.Amp * 100)/AMPmax)); 
}

void	ShowOfs(U8 startpos)
{
 U32 tmp;
 U8	tmp0;

 if(miniDDS.Ofs >= OFSmax/2) {
 	tmp = miniDDS.Ofs - OFSmax/2;
	tmp0 = 0;
 	}
 else {
 	tmp = OFSmax/2 - miniDDS.Ofs;
	tmp0 = 1;
 	}
 
 BinToDec32(tmp, (U8*)Strbuf);

 DispBuf[startpos + 0] = ' ';
 DispBuf[startpos + 1] = ' ';
 DispBuf[startpos + 2] = (tmp0 == 1) ?  '-' : '+';
 DispBuf[startpos + 3] = Strbuf[6];
 DispBuf[startpos + 4] = '.';
 DispBuf[startpos + 5] = Strbuf[7];
 DispBuf[startpos + 6] = ' ';
 DispBuf[startpos + 7] = 'V';

 if(miniDDS.Mode == KS_CW) {
 	ShowFocus();
 	}
 BitSet8(Flags, (1 << Flags_DispUpdate));

 UpdateOfs(miniDDS.Ofs);
}

void ShowType(void)
{
 PutStr_ROM(12, (U8 *)WaveName[miniDDS.CurrWave]);
}

void	ShowFocus(void) 
{
 
 DispBuf[0] = ' ';
 DispBuf[16] = ' ';
 DispBuf[25] = ' ';

 if(CurrFocus < FC_max) {
 	DispBuf[pgm_read_byte((U8 *)FocusLocTab + CurrFocus)] = TrigCtrl;
 	}

 BitSet8(Flags, (1 << Flags_DispUpdate));
}

void	ShowPW(void)
{
 U8	tmp1;

 if(miniDDS.ServoPWcurr < miniDDS.ServoPWmin) {
 	miniDDS.ServoPWcurr = miniDDS.ServoPWmin;
 	}
 if(miniDDS.ServoPWcurr > miniDDS.ServoPWmax) {
 	miniDDS.ServoPWcurr = miniDDS.ServoPWmax;
 	}

 NumString.binary = miniDDS.ServoPWcurr;
 NumString.decimal = 0;
 NumString.stringbuf = (U8*)Strbuf;
 BinToStr(&NumString);

 ClearDisp();
 PutStr_ROM(ServoPos_DispStart, (U8 *)ServoPos);

 tmp1 = 0;
 while(tmp1 < 5) {
 	DispBuf[ServoPosPW_DispStart + ServoPosPW_DigitOfs + tmp1] = Strbuf[tmp1 + 5];
	tmp1++;
 	}

 DispBuf[ServoPosPW_DispStart + ServoPosPW_SignOfs] = ' ';
 
 PutStr_ROM(ServoPosPW_DispStart, (U8 *)ServoPW);
 PutStr_ROM(ServoPosPW_DispStart + ServoPosPW_UnitOfs, (U8 *)Unit_Symbols[Unit_uSec]);

 BitSet8(Flags, (1 << Flags_DispUpdate));

 SendServoPos();
}

void	ShowServoRun(void)
{
 ClearDisp();
 PutStr_ROM(ServoRun_DispStart, (U8 *)ServoRun);
 PutStr_ROM(ServoRunState_DispStart, (U8 *)ServoRunState[CurrFocus]);

 BitSet8(Flags, (1 << Flags_DispUpdate));

 SendServoRun();
}

void	ShowServoAmp(void)
{
 U8 tmp;

 if(miniDDS.ServoAMPcurr > miniDDS.ServoAMPmax) {
 	miniDDS.ServoAMPcurr = miniDDS.ServoAMPmax;
 	}
 
 NumString.binary = miniDDS.ServoAMPcurr;
 NumString.decimal = 3;
 NumString.stringbuf = (U8*)Strbuf;
 BinToStr(&NumString);

/* 
 tmp = miniDDS.ServoAMPcurr;

 BinToDec32(tmp, (U8*)Strbuf);
*/

 ClearDisp();
 PutStr_ROM(ServoPos_DispStart, (U8 *)ServoPos);
/*
 DispBuf[ServoPosAmp_DispStart + ServoPosAmp_DigitOfs] = (Strbuf[5] == '0') ? ' ' : Strbuf[5];
 DispBuf[ServoPosAmp_DispStart + ServoPosAmp_DigitOfs + 1] = Strbuf[6];
 DispBuf[ServoPosAmp_DispStart + ServoPosAmp_DigitOfs + 2] = '.';
 DispBuf[ServoPosAmp_DispStart + ServoPosAmp_DigitOfs + 3] = Strbuf[7];
*/

 tmp = 0;
 while(tmp < 4) {
	DispBuf[ServoPosAmp_DispStart + ServoPosAmp_DigitOfs + tmp]	= Strbuf[4 + tmp];
	tmp++;
 	}
 
 DispBuf[ServoPosAmp_DispStart + ServoPosAmp_SignOfs] = ' ';
 
 PutStr_ROM(ServoPosAmp_DispStart, (U8 *)ServoAmp);
 PutStr_ROM(ServoPosAmp_DispStart + ServoPosAmp_UnitOfs, (U8 *)Unit_Symbols[Unit_V]);

 BitSet8(Flags, (1 << Flags_DispUpdate));

 SetDigitalPot((U8)(((U32)miniDDS.ServoAMPcurr * 100)/ServoAmpMax)); 
 // Set offset to half of Servo Amp
 UpdateOfs((U32)miniDDS.ServoAMPcurr/2 + OFSmax/2);
 
}

void	ShowSVParam(void)
{
 U8	tmp1;
 U16 *ptmp;

 ClearDisp();
 PutStr_ROM(ServoParamName_DispStart, (U8 *)ServoParamNames[CurrParam]);
 PutStr_ROM(ServoParamIndex_DispStart, (U8 *)ServoParamIndex[CurrParam]);
 PutStr_ROM(ServoParam_DispStart + ServoParam_UnitOfs, (U8 *)ServoParamUnits[CurrParam]);

 ptmp = &miniDDS.ServoPWmin;

 // Display selected parameter
 switch(CurrParam) {
	case SV_PWmin:
	case SV_PWmax:
	case SV_PWmid:
	case SV_PWinc:
	case SV_Cycle:
	case SV_RunStep:
 		NumString.binary = *(ptmp + CurrParam);
 		NumString.decimal = 0;
 		NumString.stringbuf = (U8*)Strbuf;
 		BinToStr(&NumString);
		
 		tmp1 = 0;
 		while(tmp1 < 5) {
 			DispBuf[ServoParam_DispStart + tmp1] = Strbuf[tmp1 + 5];
			tmp1++;
 			}
		break;

	case SV_AMPmax:
	case SV_RunRate:
 		NumString.binary = *(ptmp + CurrParam);
 		NumString.decimal = 3;
 		NumString.stringbuf = (U8*)Strbuf;
 		BinToStr(&NumString);
		
 		tmp1 = 0;
 		while(tmp1 < 5) {
 			DispBuf[ServoParam_DispStart + tmp1] = Strbuf[tmp1 + 3];
			tmp1++;
 			}
		break;
		
	case SV_DefaultRestore:
 		PutStr_ROM(ServoParam_DispStart, (U8 *)ServoParamDefaultRestore);
	default:
		break;
			}

/*
 if(CurrParam < ServoParamNumber - 1) {
	tmp = *(ptmp + CurrParam);
 	BinToDec32(tmp, (U8*)Strbuf);

 	// Remove leading zeros
 	tmp1 = 5;
 	while(tmp1 < 10)  {
		// Do not display leading 0's
		if(Strbuf[tmp1] == '0') {
			Strbuf[tmp1] = ' ';
			}
		else {
			break;
			}
		tmp1++;
		}

 	tmp1 = 0;
 	while(tmp1 < 5) {
 		DispBuf[ServoParam_DispStart + tmp1] = Strbuf[tmp1 + 5];
		tmp1++;
 		}
 	}
 else {
 	PutStr_ROM(ServoParam_DispStart, (U8 *)ServoParamDefaultRestore);
 	}

 if(CurrParam == ServoParamNumber - 2) {
 	// Special handling for ServoAmpMax
 	DispBuf[ServoParam_DispStart + 0] = ' ';
 	DispBuf[ServoParam_DispStart + 1] = (Strbuf[5] == '0') ? ' ' : Strbuf[5];
 	DispBuf[ServoParam_DispStart + 2] = Strbuf[6];
 	DispBuf[ServoParam_DispStart + 3] = '.';
 	DispBuf[ServoParam_DispStart + 4] = Strbuf[7];
 	}
*/ 
 BitSet8(Flags, (1 << Flags_DispUpdate));
}

/*
void	ShowTsw(void)
{
 U8 tmp;

 // Convert binary to string
 NumString.binary = miniDDS.SweepTswStep;
 NumString.decimal = 0;
 NumString.stringbuf = (U8*)Strbuf;
 BinToStr(&NumString);

 ClearDisp();
 // Show title
 PutStr_ROM(SweepTswName_DispStart, (U8 *)TswTitle);

 // Show Tsw value
 tmp = 0;
 while(tmp < 5) {
	DispBuf[SweepTsw_DispStart + SweepTsw_DigitOfs + tmp]	= Strbuf[5 + tmp];
	tmp++;
 	}
 
 DispBuf[SweepTsw_DispStart + SweepTsw_SignOfs] = ' ';

 // Show unit
 PutStr_ROM(SweepTsw_DispStart + SweepTsw_UnitOfs, (U8 *)Unit_Symbols[Unit_mSec]);

 BitSet8(Flags, (1 << Flags_DispUpdate)); 
 SendSweep();
}
*/

void	ShowStep(void)
{
 U8	tmp;
 
 // Display title
 ClearDisp();

 if(KeyParam == 'H') {
 	PutStr_ROM(StepTitle_DispStart, (U8 *)FreqStep);
	DispBuf[StepDigit_DispStart + StepUnit_Ofs] = 'H';
	DispBuf[StepDigit_DispStart + StepUnit_Ofs + 1] = 'z';
 	BinToDec32(miniDDS.FreqIncStep, (U8*)Strbuf);
 	}
 else {
 	PutStr_ROM(StepTitle_DispStart, (U8 *)TimeStep);
	DispBuf[StepDigit_DispStart + StepUnit_Ofs] = 'm';
	DispBuf[StepDigit_DispStart + StepUnit_Ofs + 1] = 's';
 	BinToDec32(miniDDS.CycleIncStep, (U8*)Strbuf);
 	}

 // Remove leading zeros
 tmp = 0;
 while(tmp < 9)  {
	// Do not display leading 0's
	if(Strbuf[tmp] == '0') {
		Strbuf[tmp] = ' ';
		}
	else {
		break;
		}
	tmp++;
	}

 tmp = 0;
 while(tmp < 10) {
	DispBuf[StepDigit_DispStart + tmp] = Strbuf[tmp];
	tmp++;
 	}
 
 BitSet8(Flags, (1 << Flags_DispUpdate));

}

void	ShowDuty(void)
{
 U8	tmp;

 // Display title
 ClearDisp();
 PutStr_ROM(0, (U8 *)Duty);

 BinToDec32((U32)miniDDS.Duty, (U8*)Strbuf);
 
 // Remove leading zeros
 tmp = 0;
 while(tmp < 9)  {
	// Do not display leading 0's
	if(Strbuf[tmp] == '0') {
		Strbuf[tmp] = ' ';
		}
	else {
		break;
		}
	tmp++;
	}

 tmp = 0;
 while(tmp < 3) {
	DispBuf[17 + tmp] = Strbuf[7 +tmp];
	tmp++;
 	}
 
 BitSet8(Flags, (1 << Flags_DispUpdate));

 SendDDS();
}

void	CalDeltaPhase(U32 *freq, U32 *dphase, float fclk)
{
 *dphase = (U32)((float)(*freq) / (fclk / 16777216.0));

}

void	SendDDS(void)
{
// U32 tmp0;
 U8	tmp1, tmp2;
// float ftmp;

 // -- Calculate DeltaPhase
 // Output frequency (using 24 bit accumulator) :
 //
 //	Fout = deltaPhase * fClock/2^24
 //
 //   fClock is in this case the CPU clock divided by the
 //	number of cycles to output the data ( 8 cycles )
 //
 //	Fout = [DeltaPhase] * (20000000/8)/16777216
 //
 //	Fout = [DeltaPhase] * 0.14901161
 //

 // -- For Band = 0 (High Frequency Mode), where setting is in unit of Hz
 //
 //  [DeltaPhase] = Fout / ((FG_SYSCLK/8) / 2^24)
 //	Freq. Error = 0.14901161 Hz
 //
 // -- For Band = 1 (Low Frequency Mode), where setting is in unit of ms
 //		DDS loop frequency = 10000Hz (2000 cycles)
 //
 //	Fout = [DeltaPhase] * (10000/2^24) = [DeltaPhase] * 0.0005960
 //  [DeltaPhase] = Fout / (10000 / 2^24) 
 //	Freq. Error = 0.0005960 Hz
 //
 //   If Fout is specified as period Tout in mSec
 //
 //	[DeltaPhase] = 1000 / ( Tout * (10000.0 / 2^24) )
 //
 //
 if(miniDDS.SetType == SetType_Freq) {
 	if(miniDDS.Freq > 40) {
		// Use High Freq. Mode for frequency > 40Hz
		CalDeltaPhase((U32 *)&miniDDS.Freq, (U32 *)&Strbuf[2], (FG_SYSCLK/8.0));
		tmp1 = 0x00;
 		}
	else {
		// Use Low Freq. Mode
		CalDeltaPhase((U32 *)&miniDDS.Freq, (U32 *)&Strbuf[2], 10000.0);
		tmp1 = 0x80;
		}
 	}
 else {
 	if(miniDDS.Freq < 25) {
		// Still use High Freq. Mode for period < 25ms
 		*(U32 *)(&Strbuf[2]) = (U32)((1000.0 * (16777216.0/(FG_SYSCLK/8.0))) / ((float)miniDDS.Freq ));
		tmp1 = 0x00;
 		}
	else {
		// Use Low Freq. Mode for period >= 25ms
 		*(U32 *)(&Strbuf[2]) = (U32)((1000.0 * (16777216.0 / 10000.0)) / ((float)miniDDS.Freq ));
		tmp1 = 0x80;

		}
 	}

 // Trig contral enable/disable bit
 tmp2 = 0x00;
 if(TrigCtrl == '*') {
 	tmp2 = 0x40;
 	}

 Strbuf[0] = KS_CW;
 Strbuf[1] = (miniDDS.CurrWave & 0x1F) | tmp1 | tmp2;
// Strbuf[2] = (U8)(tmp0 & 0xFF);
// Strbuf[3] = (U8)((tmp0 >> 8) & 0xFF);
// Strbuf[4] = (U8)((tmp0 >> 16) & 0xFF);
// Strbuf[5] = (U8)((tmp0 >> 24) & 0xFF);
/*
 Strbuf[6] = 0;
 Strbuf[7] = 0;
 Strbuf[8] = 0;
 Strbuf[9] = 0;
 Strbuf[10] = 0;
 Strbuf[11] = 0;
*/
 Strbuf[16] = ((U16)miniDDS.Duty * 255)/100;

// u0putc(Strbuf[16]);
 
 SendBuf((U8*)Strbuf);
}

void	SendSweep(void)
{
 S32 tmp0;
 U32 tmp2;
 U8	tmp3, tmp4;

// Frame structure
//	[0]: 		Command (KS_Sweep)
//	[1]:		Waveform type
//	[2:5]:	DeltaPhase for Fstart
//	[6:9]:	DeltaPhase incremental
//	[10:13]:	Sweeping Steps
//	[14:15]:	Tsw Step size

 Strbuf[0] = KS_Sweep;

 tmp3 = 0x00;
 if(TrigCtrl == '*') {
 	tmp3 = 0x40;
 	}

 tmp4 = 0;
 if(miniDDS.SweepDir == 'B') {
 	tmp4 = 0x20;
 	}

 Strbuf[1] = (miniDDS.CurrWave & 0x1F) | tmp3 | tmp4;

 /*
 *((U32 *)&Strbuf[2]) = 5000;
 *((U32 *)&Strbuf[6]) = 0x00000100;
 *((U32 *)&Strbuf[10]) = 0xFFFF0000;
 */
 /* 
 // Calculate DeltaPhase for start and stop frequency
 //	Strbuf[2] holds the DeltaPhase for start freq. (lower freq.)
 //	tmp0 holds the DeltaPhase for stop freq. (higher freq.)
 if(miniDDS.SweepStartFreq > miniDDS.SweepStopFreq) {
 	CalDeltaPhase((U32 *)&miniDDS.SweepStopFreq, (U32 *)&Strbuf[2], (FG_SYSCLK/8.0));
 	CalDeltaPhase((U32 *)&miniDDS.SweepStartFreq, (U32 *)&tmp0, (FG_SYSCLK/8.0));
 	}
 else {
 	CalDeltaPhase((U32 *)&miniDDS.SweepStartFreq, (U32 *)&Strbuf[2], (FG_SYSCLK/8.0));
 	CalDeltaPhase((U32 *)&miniDDS.SweepStopFreq, (U32 *)&tmp0, (FG_SYSCLK/8.0));
 	}
*/

 // Calculate DeltaPhase for start and stop frequency
 //	Strbuf[2] holds the DeltaPhase for start freq. 
 //	Strbuf[6] holds the DeltaPhase for stop freq. 
 CalDeltaPhase((U32 *)&miniDDS.SweepStartFreq, (U32 *)&Strbuf[2], (FG_SYSCLK/8.0));
 CalDeltaPhase((U32 *)&miniDDS.SweepStopFreq, (U32 *)&Strbuf[6], (FG_SYSCLK/8.0));


 // Sweeping steps
 tmp2 = (miniDDS.SweepTime/miniDDS.SweepTswStep);
 if(tmp2 == 0) {
 	tmp2 = 1;
 	}
 
 // Calculate delta DeltaPhase
 tmp0 = *((U32 *)&Strbuf[6]) - *((U32 *)&Strbuf[2]);	// This is the DeltaPhase difference between Fstrat and Fstop
 *((S32 *)&Strbuf[6]) = (S32)(((float)(tmp0 * 256))/((float)(tmp2))); // (*256) because delta DeltaPhase uses 32 bits. DeltaPhase uses only 24 bits.

/*
 u0putc(0x00);
 u0putc(0xFF);
 u0putc((U8)Strbuf[9]);
 u0putc((U8)Strbuf[8]);
 u0putc((U8)Strbuf[7]);
 u0putc((U8)Strbuf[6]);
*/

 // Send (2^32 - Sweeping Step)
 // An additional step for Fstop
 *((U32 *)&Strbuf[10]) = 0x100000000L - (tmp2 + 1);

 // Tsw Step. Send (2^16 - TswStep)
 *((U16 *)&Strbuf[14]) = 0x10000 - miniDDS.SweepTswStep;

  Strbuf[16] = ((U16)miniDDS.Duty << 8)/100;

 SendBuf((U8*)Strbuf);
/*
 // Re-calculate parameters based on new enters
 
 switch(EnterType & 0x0F) {
 	// Use Fstart & Fstop
 	case (Fstart1|Fstop2):
 	case (Fstop1|Fstart2):
 	case (Fstart1|Fstart2):		// This is illegal case. Use Fstart & Fstop instead
 	case (Fstop1|Fstop2):		// This is illegal case. Use Fstart & Fstop instead
 	case (Fcenter1|Fcenter2):	// This is illegal case. Use Fstart & Fstop instead
 	case (Fspan1|Fspan2):		// This is illegal case. Use Fstart & Fstop instead
 	default:
		miniDDS.SweepCentertFreq = (miniDDS.SweepStartFreq + miniDDS.SweepStopFreq)/2;
		miniDDS.SweepSpanFreq = miniDDS.SweepStopFreq - miniDDS.SweepStartFreq;
		break;

	// Use Fstart & Fcenter
 	case (Fstart1|Fcenter2):
 	case (Fcenter1|Fstart2):
		miniDDS.SweepStopFreq = 2 * miniDDS.SweepCentertFreq - miniDDS.SweepStartFreq;
		miniDDS.SweepSpanFreq = 2 * (miniDDS.SweepCentertFreq - miniDDS.SweepStartFreq);
		break;

	// Use Fstart & Fspan
 	case (Fstart1|Fspan2):
 	case (Fspan1|Fstart2):
		miniDDS.SweepStopFreq = miniDDS.SweepStartFreq + miniDDS.SweepSpanFreq;
		miniDDS.SweepCentertFreq = miniDDS.SweepStartFreq + miniDDS.SweepSpanFreq/2;
		break;

	// Use Fstop & Fcenter
 	case (Fstop1|Fcenter2):
 	case (Fcenter1|Fstop2):
		miniDDS.SweepStartFreq = 2 * miniDDS.SweepCentertFreq - miniDDS.SweepStopFreq;
		miniDDS.SweepSpanFreq = 2 * (miniDDS.SweepStopFreq - miniDDS.SweepCentertFreq);
		break;

	// Use Fstop & Fspan
 	case (Fstop1|Fspan2):
 	case (Fspan1|Fstop2):
		miniDDS.SweepStartFreq = miniDDS.SweepStopFreq - miniDDS.SweepSpanFreq;
		miniDDS.SweepCentertFreq = miniDDS.SweepStopFreq - miniDDS.SweepSpanFreq/2;
		break;

	// Use Fcenter & Fspan
 	case (Fcenter1|Fspan2):
 	case (Fspan1|Fcenter2):
		miniDDS.SweepStartFreq = miniDDS.SweepCentertFreq - miniDDS.SweepSpanFreq/2;
		miniDDS.SweepStopFreq = miniDDS.SweepCentertFreq + miniDDS.SweepSpanFreq/2;
		break;
*/

 	
}

void	SendServoPos(void)
{

 Strbuf[0] = KS_ServoPos;
 Strbuf[1] = 0;
 *(U16 *)&Strbuf[2] = miniDDS.ServoPWcurr;
 *(U16 *)&Strbuf[4] = miniDDS.ServoCycle;
 
/*
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

}

void	SendServoRun(void)
{

 Strbuf[0] = KS_ServoRun;
 Strbuf[1] = CurrFocus;
 *(U16 *)&Strbuf[2] = miniDDS.ServoPWmin;
 *(U16 *)&Strbuf[4] = miniDDS.ServoPWmax;
 *(U16 *)&Strbuf[6] = miniDDS.ServoRunStep;
 *(U16 *)&Strbuf[8] = miniDDS.ServoRunRate;
 *(U16 *)&Strbuf[10] = miniDDS.ServoCycle;
 
/* 
 Strbuf[4] = (U8)(miniDDS.ServoPWmax & 0xFF);
 Strbuf[5] = (U8)((miniDDS.ServoPWmax >> 8) & 0xFF);
 Strbuf[6] = (U8)(miniDDS.ServoRunStep & 0xFF);
 Strbuf[7] = (U8)((miniDDS.ServoRunStep >> 8) & 0xFF);
 Strbuf[8] = (U8)(miniDDS.ServoRunRate & 0xFF);
 Strbuf[9] = (U8)((miniDDS.ServoRunRate >> 8) & 0xFF);
 Strbuf[10] = (U8)(miniDDS.ServoCycle & 0xFF);
 Strbuf[11] = (U8)((miniDDS.ServoCycle >> 8) & 0xFF);
*/ 	

 SendBuf((U8*)Strbuf);
}

void	LoadUserWaveform(void)
{
 U8 tmp0, tmp1;
 U16	tmp2;
 
 // User waveform data is stored at 0x0100 in on-chip EEPROM
 // Send the data to DDS core. 256 bytes in total. Send in 32 blocks
 tmp0 = 0;
 while(tmp0 < 32) {
 	Strbuf[0] = KS_WriteWavetable;
	Strbuf[1] = 8;
	Strbuf[2] = tmp0 * 8;
	tmp2 = EE_UserWaveform + tmp0 * 8;
	tmp1 = 0;
	while(tmp1 < 8) {
		Strbuf[3 + tmp1] = EE_Read(tmp2++);
		tmp1++;
		}
	SendBuf((U8*)Strbuf);

	Delay(1000);
	// Wait for confirm
	tmp1 = 0;
 	GTimer = 40000;
 	while(GTimer) {
		if(!BitTest8(COMM_RXD_Port, (1 << COMM_RXD_Bit))) {
			// Detected low at RXD Pin
			tmp1 = RecvByte();
			Delay(50000);
			break;
			}
 		}
	if(tmp1 == '=') {
		// Success. Send next block. (If not success the same block will be re-sent)
		tmp0++;
		}
	}

}

void	SaveSetting(void)
{
 U8	tmp;

 // Setup EEPROM
 tmp = 0;
 while(tmp < sizeof(FuncGen)) {
 	EE_Write((U16)&miniDDS_EE + tmp, *((U8*)&miniDDS + tmp));
	tmp++;
	}
 
}

// ==============================================
// Store a ROM string "str" to the "loc" location of DispBuf
//	
void PutStr_ROM(U8 loc, U8 *str)
{
 U8 tmp;

 while((tmp = pgm_read_byte(str++))) {
 	DispBuf[loc++] = tmp;
 	}
}

// ==============================================
// Fill a portion of display buffer start at 'startpos' of length 'len' with 'symbol'
//	
void	FillDisp(U8 startpos, U8 len, U8 symbol)
{
 U8 tmp;
 
 tmp = 0;
 while(tmp < len) {
	DispBuf[startpos + tmp] = symbol;
	tmp++;
 	}

}

// ==============================================
// Display whole number 'num' at 'startpos'. The maximun digits are 'maxlen' (including comma
//	at thousand position)
//
void	DispWholeNum(U8 startpos, U8 maxlen, U32 *num)
{
 U8 tmp;
 U8 tmp0, tmp1;
 
 BinToDec32(*num, (U8*)Strbuf);

 // Remove leading zeros
 tmp = 0;
 while(tmp < 9)  {
	// Do not display leading 0's
	if(Strbuf[tmp] == '0') {
		Strbuf[tmp] = ' ';
		}
	else {
		break;
		}
	tmp++;
	}

 tmp0 = 0;
 tmp1 = 0;
 maxlen -= 1;
 while(tmp0 <= maxlen) {
	if(tmp0 == 3) {
		DispBuf[startpos + maxlen - tmp0] = (tmp < 7) ?  ',' : ' ';
		tmp1 = 1;
		}
	else {
		DispBuf[startpos + maxlen - tmp0] = Strbuf[9 - tmp0 + tmp1];
		}
	tmp0++;
 	}

}

void	PutFreq(U32 *item, U32 freq)
{
 if(freq > FreqMax) {
 	freq = FreqMax;
 	}
 if(freq < FreqMin) {
 	freq = FreqMin;
 	}
 *item = freq;
 
}

void	ToggleTrigCtrl(void)
{
 if(TrigCtrl == '>') {
 	TrigCtrl = '*';		// Trig Contral enabled
 	}
 else {
 	TrigCtrl = '>';
 	}

 switch(SysState.CurrState) {
 	case KS_CW:
 		ShowFocus();
 		SendDDS();
		break;
		
	case KS_Sweep:
		ShowSweep();
	default:
		break;
 	}
 
 BitSet8(Flags, (1 << Flags_DispUpdate));
 
}

void	ToggleSweepDir(void)
{
 if(miniDDS.SweepDir == ' ') {
 	miniDDS.SweepDir = 'B';		// Trig Contral enabled
 	}
 else {
 	miniDDS.SweepDir = ' ';
 	}

 EE_Write((U16)&miniDDS_EE.SweepDir, miniDDS.SweepDir);

 ShowSweep();

 BitSet8(Flags, (1 << Flags_DispUpdate));
}

