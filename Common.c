//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Common.c
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
//-----------------------------------------------------------------------------
// 	Includes
//-----------------------------------------------------------------------------

#include	"Common.h"
#include	"Board.h"

// ===========================================================
//	Function Definitions
// ===========================================================
//
/*
//-----------------------------------------------------------------------------
// Return the smaller
//-----------------------------------------------------------------------------
//
U8	Min(U8 a, U8 b)
{
 if(a < b) {
 	return a;
 	}
 else {
 	return b;
 	}
}

//-----------------------------------------------------------------------------
// Return the greater
//-----------------------------------------------------------------------------
//
U8	Max(U8 a, U8 b)
{
 if(a > b) {
 	return a;
 	}
 else {
 	return b;
 	}
}
*/

/*
//-----------------------------------------------------------------------------
// Convert 8-bit binary to two ascii-coded hexical and return with higher byte for higher nibble,
//	lower byte for lower nibble.
//-----------------------------------------------------------------------------
//
U8	*BinToHex8(U8 binary, U8 *Str)
{
 U8	 tmp0;

 tmp0 = (binary >> 4) | 0x30;
 if(tmp0 > 0x39) {
 	tmp0 += 0x07;
 	}
 *(Str + 0) = tmp0;
 
 tmp0 = (binary & 0x0F) | 0x30; 
 if(tmp0 > 0x39) {
 	tmp0 += 0x07;
 	}
 *(Str + 1) = tmp0;

 *(Str + 2) = 0;

 return Str;

}

//-----------------------------------------------------------------------------
// Convert 16-bit binary to four ascii-coded hexical string
//-----------------------------------------------------------------------------
//
U8	*BinToHex16(U16 binary, U8 *Str)
{
 U8	tmp0,  tmp1;

 tmp1 = 4;
 while(tmp1) {
 	tmp0 = ((U8)binary&0x000F)|0x30;
	if(tmp0 > 0x39) {
 		tmp0 += 0x07;
 		}
	*(Str + tmp1 - 1) = tmp0;
	binary >>= 4;
	tmp1--;
 	}
 *(Str + 4) = 0;

 return Str;

}
*/

//-----------------------------------------------------------------------------
// Convert 8-bit binary to 3 digit decimal. 
//	The decimal is stored in a 0 terminated ASCII string and pointer to the string is returned
//-----------------------------------------------------------------------------
//
/*
U8 	*BinToDec8(U8 binary, U8 *Str)
{
 U8	 tmp0;

 tmp0 = binary/100;
 *(Str + 0) = 0x30 | tmp0;

 binary = binary - tmp0 * 100;
 tmp0 = binary/10;
 *(Str + 1) = 0x30 | tmp0;

 binary = binary - tmp0 * 10;
 *(Str + 2) = 0x30 | binary;

 *(Str + 3) = 0;

 return Str;

}
*/
/*
//-----------------------------------------------------------------------------
// Convert 16-bit binary to 5 digit decimal. 
//	The decimal is stored in a 0 terminated ASCII string and pointer to the string is returned
//-----------------------------------------------------------------------------
//
U8 	*BinToDec16(U16 binary, U8 *Str)
{
 U8	 tmp0;

 tmp0 = binary/10000;
 *(Str + 0) = 0x30 | tmp0;
 binary = binary - tmp0 * 10000;
 
 tmp0 = binary/1000;
 *(Str + 1) = 0x30 | tmp0;
 binary = binary - tmp0 * 1000;

 tmp0 = binary/100;
 *(Str + 2) = 0x30 | tmp0;
 binary = binary - tmp0 * 100;

 tmp0 = binary/10;
 *(Str + 3) = 0x30 | tmp0;
 binary = binary - tmp0 * 10;
 
 *(Str + 4) = 0x30 | binary;

 *(Str + 5) = 0;

 return Str;

}
*/

U8 	*BinToDec32(U32 binary, U8 *Str)
{
 U8	 tmp0;

/*
 tmp0 = binary/1000000000L;
 *(Str + 0) = 0x30 |tmp0;
 binary = binary - tmp0 * 1000000000L;
 
 tmp0 = binary/100000000L;
 *(Str + 1) = 0x30 |tmp0;
 binary = binary - tmp0 * 100000000L;

 tmp0 = binary/10000000L;
 *(Str + 2) = 0x30 |tmp0;
 binary = binary - tmp0 * 10000000L;

 tmp0 = binary/1000000L;
 *(Str + 3) = 0x30 |tmp0;
 binary = binary - tmp0 * 1000000L;
 
 tmp0 = binary/100000L;
 *(Str + 4) = 0x30 |tmp0;
 binary = binary - tmp0 * 100000L;

 tmp0 = binary/10000L;
 *(Str + 5) = 0x30 |tmp0;
 binary = binary - tmp0 * 10000L;
 
 tmp0 = binary/1000L;
 *(Str + 6) = 0x30 |tmp0;
 binary = binary - tmp0 * 1000L;

 tmp0 = binary/100L;
 *(Str + 7) =  0x30 |tmp0;
 binary = binary - tmp0 * 100L;

 tmp0 = binary/10L;
 *(Str + 8) =  0x30 |tmp0;
 binary = binary - tmp0 * 10L;
 
 *(Str + 9) =  0x30 |binary;
*/
 tmp0 = 0;
 while(tmp0 <= 9) {
	binary = FindDec(binary, tmp0, (U8 *)Str);
	tmp0++;
 	}

 *(Str + 10) = 0;
 *(Str + 11) = 0;

 return Str;

}

U32 FindDec(U32 bin, U8 ind, U8 *str)
{
 U8 tmp;
 U32	tmp1;

 tmp1 = 1;
 tmp = 9 - ind;
 while(tmp) {
	tmp1 *= 10;
	tmp--;
 	}

 tmp = bin/tmp1;
 *(str + ind) = 0x30 | tmp;
 return (bin - tmp * tmp1);
}

#define	DigitStringSize 		10
void	BinToStr(DigitString *digitstr)
{
 U8 tmp0, tmp1;

 BinToDec32(digitstr->binary, digitstr->stringbuf);

 if(digitstr->decimal != 0) {
	// Insert decimal point
	tmp1 = DigitStringSize -digitstr->decimal - 1;
	tmp0 = 0;
	while(tmp0 < tmp1) {
		*(digitstr->stringbuf + tmp0) = *(digitstr->stringbuf + tmp0 + 1);
		tmp0++;
		}
	*(digitstr->stringbuf + tmp0) = '.';
	
 	}

 // Removing leading 0's
 if(digitstr->decimal == 0) {
 	tmp0 = 1;
 	}
 else {
 	tmp0 = digitstr->decimal + 2;
 	}
 tmp1 = DigitStringSize - tmp0;
 tmp0 = 0;
 while(tmp0 < tmp1) {
	if(*(digitstr->stringbuf + tmp0) == '0') {
		*(digitstr->stringbuf + tmp0) = ' ';
		}
	else {
		break;
		}
	tmp0++;
 	}

/* 
 if(digitstr->comma != 0) {
 	// Insert comma
 	}
*/ 
}


/*
//-----------------------------------------------------------------------------
// Convert one ASCII char to hexical number if the char if within '0'~'9' and 'a'~'z' or 'A'~'Z' 
//	and return it. If the char is not a valid hexical digit return 0xff.
//-----------------------------------------------------------------------------
//
U8	AscToHex(U8 ch)
{
 if(ch < '0') {
 	return 0xff;
 	}

 if(ch < '9'+1) {
 	return (ch & 0x0f);
 	}

 // -- Convert to upper case
 ch &= ~0x20;
 if(ch < 'A') {
 	return 0xff;
 	}

 if(ch < 'F'+1) {
 	return (ch - 0x37);
 	}
 return 0xff;
}
*/
void	Dly50us(void)
{
 U8 tmp0 = 50;
 while(tmp0) {
 	Dly1us();
 	tmp0--;
 	}
}

// ======================================
// Delay by the specified millisecond
void	Dly_ms(U16 delay)
{
 while(delay) {
 	Dly1ms();
	delay--;
 	}
}

void	Dly1ms(void)
{
 U8 tmp0 = 20;
 while(tmp0) {
 	Dly50us();
	tmp0--;
 	}
}

void	Dly_us(U16 delay)
{
 while(delay) {
 	Dly1us();
	delay--;
 	}

}

void	Delay(volatile U16 count)
{
 while(count) {
 	count--;
 	}
}

U8	DelayDisp(U8 symbol)
{
 U8 tmp;

 tmp = 150;
 while(tmp) {
 	Delay(60000);
	tmp--;
 	}
 
 if(symbol == 'l') {
 	return 'm';
 	}
 return tmp;
 
}

