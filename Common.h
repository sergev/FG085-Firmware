//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Common.h
//	Version:		0.01
//	Data:		2005.12.20
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
//		V0.01		2005.12.20
//		1. First created.
//
///////////////////////////////////////////////////////////////////////////////
//

#ifndef Common_h

#define	Common_h

typedef	unsigned char		U8;
typedef	signed char		S8;
typedef	unsigned int	U16;
typedef	int	S16;
typedef	unsigned long	U32;
typedef	long		S32;


typedef	void	(*FuncPointer)(U8);
typedef	void	(*CmdFuncPointer)(void); 

typedef	struct {
	U32	binary;
	U8	decimal;	// decimal point position
	U8	comma;		// comma position
	U8	*stringbuf;
	}DigitString;
// -- Control debug code generation
//#define	_Debug_

// ============= Macro definitions ===========================

#define	BitSet8(word, bit_mask)		((word) |= (bit_mask))
#define	BitClr8(word, bit_mask)		((word) &= ~(bit_mask))
#define	BitTest8(word, bit_mask)		((word) & (bit_mask))
#define	BitAnd8(word, bit_mask)		((word) &= (bit_mask))
#define	BitOr8(word, bit_mask)		((word) |= (bit_mask))
#define	BitXor8(word, bit_mask)		((word) ^= (bit_mask))

void	zmon(void);

// ===========================================================
//	Function Prototype Declarations
// ===========================================================
//
U8	Min(U8 a, U8 b);
U8	Max(U8 a, U8 b);
U8	*BinToHex8(U8 binary, U8 *Str);
U8	*BinToHex16(U16 binary, U8 *Str);
U8 	*BinToDec8(U8 binary, U8 *Str);
U8 	*BinToDec16(U16 binary, U8 *Str);
U8 	*BinToDec32(U32 binary, U8 *Str);
U32 FindDec(U32 bin, U8 ind, U8 *str);
U8	AscToHex(U8 ch);
void	BinToStr(DigitString *digitstr);
void	Dly1us(void);
void	Dly50us(void);
void	Dly1ms(void);
void	Dly_ms(U16 delay);
void	Dly_us(U16 delay);
void	Delay(U16 count);
U8	DelayDisp(U8 symbol);


#endif // Common_h 

