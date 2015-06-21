//////////////////////////////////////////////////////////////////////////////
//
// 	Filename:	Board.c
//	Version:		0.01
//	Data:		2006.01.03
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
//		2.	ATmega48 fuses configuration:
//			  1)	Extended Byte = 0b-----001 = 0x01
//				Bit [2:1] 	: Select boot size (ATmega88/168 only)
//				
//				Bit 0		: (ATmega48) Self programming enable [SELFPRGEN(1)] 
//						: (ATmega88/168) Select reset vector [BOOTRST(1)] 
//							1 = Application reset, address = 0x0000
//							0 = Boot loader reset
//
//			  2) 	High Byte = 0b11010101 = 0xD5
//				Bit 7		: External Reset Disable [RSTDISBL(1)]
//							1 = External reset enabled
//				Bit 6		: DebugWIRE enable [DWEN(1)]
//				Bit 5		: Serial Programming enable [SPIEN(0)]
//				Bit 4		: WDT always on [WDTON(1)]
//							1 = WDT behaviour determined by WDE & WDIE in WDTSCR
//							0 = WDT on and works in System Reset Mode
//				Bit 3		: EEPROM Preserved [EESAVE(1)]
//							1 = EEPROM not reserved
//				Bit [2:0]	: BOD trig level [BODLEVEL[2:0] (111)]
//							000 = Reserved
//							001 = Reserved
//							010 = Reserved
//							011 = Reserved
//							100 = 4.3V (typical)
//							101 = 2.7V (typical)
//							110 = 1.8V (typical)
//							111 = Disabled
//
//			  2) 	Low Byte = 0b11100010 = 0XE2
//				Bit 7		: Divide Clock by 8 [CKDIV8 (0)]
//							0 = Clock divided by 8 (CLKPR[3:0] set to 0011)
//							1 = No division (CLKPR[3:0] set to 0000)
//				Bit 6		: Clock Output [CKOUT = 1]
//							0 = Enable clock output at CLKO pin
//							1 = Disable clock output
//				Bit [5:4]	: Select start-up time [SUT[1:0](10)]
//							See datasheet
//				Bit [3:0]	: Clock source select [CKSEL[3:0](0010)]
//							0000 = External clock
//							0001 = Reserved
//							0010 = Calibrated internal RC oscillator
//							0011 = Internal 128KHz RC oscillator
//							0100-0101 = Low Frequency Crystal Osc
//							0110-0111 = Full Swing Crystal Osc
//							1000-1111 = Low Power Crystal Osc
//-----------------------------------------------------------------------------
//	Revision History:
//		V0.01		2006.01.03
//		1. First created.
//
///////////////////////////////////////////////////////////////////////////////
//
//-----------------------------------------------------------------------------
// 	Includes
//-----------------------------------------------------------------------------

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>

#include "Common.h"
#include "Board.h"
#include 	"FuncGen.h"

// ===========================================================
//	File Scope Variables
// ===========================================================
//
volatile	U8	Keycode;
volatile	U8	Keybuf;
volatile	U8	Keybuf0;
volatile	U8	Keycnt;

volatile	U16	KScanBuf;
U16	KScan;		
U8	KeyCode;
U8	KeyCodeBuf;
U8	KeyParam;
volatile	U8	enc_state;
volatile	U8	old_enc_cnt;
volatile	U8	enc_cnt;
volatile	U8	KFlags;
// Flag bit allocation:
#define	Flag_PWR		7		// 0 - Off, 1 - On
#define	Flag_KeyHold	6		// 0 - normal, 1 - hold

U8	KCount;
U8	KHCount;	// Key hold counter
U8	KTimeChk;	// Key time check 

// High byte: [X, X, COL3, COL2, COL1, COL0, COL4, COL5]
// Low byte: [X, X, X, X, ROW3, ROW2, ROW1, ROW0]
PROGMEM 	KeyScanCode KScanCodeTab[KC_Max] = {
//	scan code	key code		key param
	{0xFBFB, 	KC_0,		'0'},		// 0
	{0xDFF7, 	KC_1,		'1'},		// 1
	{0xDFFB, 	KC_2,		'2'},		// 2
	{0xDFFD, 	KC_3,		'3'},		// 3
	{0xEFF7, 	KC_4,		'4'},		// 4
	{0xEFFB, 	KC_5,		'5'},		// 5
	{0xEFFD, 	KC_6,		'6'},		// 6
	{0xF7F7, 	KC_7,		'7'},		// 7
	{0xF7FB, 	KC_8,		'8'},		// 8
	{0xF7FD, 	KC_9,		'9'},		// 9
	{0xFBFD, 	KC_DP,		'.'},		// 0
	{0xFBF7, 	KC_Sign,		'+'},		// 0
	{0xFBFE, 	KC_KHz,		'K'},		// 0
	{0xF7FE, 	KC_Hz,		'H'},		// 0
	{0xEFFE, 	KC_WF,		'W'},	// 0
	{0xDFFE, 	KC_ESC,		'E'},		// 0
	{0xFDFE, 	KC_Mode,	'M'},		// 0
	{0xFDFD, 	KC_Freq,		'F'},		// 0
	{0xFDF7, 	KC_OFS,		'O'},		// 0
	{0xFDFB, 	KC_Amp,		'A'},		// 0
	{0xFEF7, 	KC_Aux,		'X'},		// 0
	{0xDEF6, 	KC_Aux_ESC,	0}		// 0
	};


PROGMEM	U8	enc_table[16] = {
//	0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0
// This table is for rotary encoders with two contacts normally closed at all positions.
	0,		// 0000
	-1,		// 0001
	1,		// 0010
	0,		// 0011
	0,		// 0100
	0,		// 0101
	0,		// 0110
	0,		// 0111
	0,		// 1000
	0,		// 1001
	0,		// 1010
	0,		// 1011
	0,		// 1100
	0,		// 1101
	0,		// 1110
	0		// 1111
/*
// This table is for rotary encoders with two contacts normally closed and open at every other positions.
	0,		// 0000
	1,		// 0001
	-1,		// 0010
	0,		// 0011
	0,		// 0100
	0,		// 0101
	0,		// 0110
	0,		// 0111
	0,		// 1000
	0,		// 1001
	0,		// 1010
	0,		// 1011
	0,		// 1100
	-1,		// 1101
	1,		// 1110
	0		// 1111
*/
	};


volatile	U8	DispBuf[35] = {"0123456789ABCDEFGHIJKLMNOPQSTUV"};
volatile	U16	GTimer;		// General timer, decreased by 1 at T0 overflow (32us cycle) 
volatile	U8	Flags;	
volatile	U8	Strbuf[20];

volatile	U8	PrevWiperPos;

// ===========================================================
//	External Variables
// ===========================================================
// 
// ------------------------------------
// ROM constants
//

// ===========================================================
// 	Externals
//

// ===========================================================
//	Function Definitions
// ===========================================================

//-----------------------------------------------------------------------------
// SYSCLK_Init
//-----------------------------------------------------------------------------
//
//


//-----------------------------------------------------------------------------
// Misc_Init
//-----------------------------------------------------------------------------
#define	IVSEL	1
#define	IVCE	0
void	Misc_Init(void)
{
// MCU Control Register - MCUCR
//	PUD:	1 = Disable port pull-up globally, 0 = enable port pull-up. [MCUCR.4 = 0]
//	IVSEL:	
//	IVCE:	
//
 MCUCR = (0<<PUD)|(0<<IVSEL)|(0<<IVCE);		

// Clock Prescaler Register - CLKPR
//	CLKPCE:		1 = Clock prescaler change enable. CLKPS bits must be updated within 4 cycles
//				[CLKPR.8 (0)]
//	CLKPS[3:0]:	Clock prescaler select bits [Default value determined by CKDIV8 fuse]
//					0000 = 1
//					0001 = 1/2
//					0010 = 1/4
//					0011 = 1/8
//					0100 = 1/16
//					0101 = 1/32
//					0110 = 1/64
//					0111 = 1/128
//					1000 = 1/256
//					1001-1111 = Reserved
 CLKPR = (1<<CLKPCE)|(0<<CLKPS3)|(0<<CLKPS2)|(0<<CLKPS1)|(0<<CLKPS0);
 CLKPR = (0<<CLKPCE)|(0<<CLKPS3)|(0<<CLKPS2)|(0<<CLKPS1)|(0<<CLKPS0);

 // TSM: 			Timer/Counter Synchronization Mode
 // PSRASY:		Prescaler Reset Timer/Counter2
 // PSRSYNC: 	Prescaler Reset
 GTCCR = (0<<TSM)|(0<<PSRASY)|(0<<PSRSYNC);
}

//-----------------------------------------------------------------------------
// PORT_Init
//-----------------------------------------------------------------------------
//
// This routine configures the crossbar and GPIO ports.
//
void Port_Init(void)
{

//  Note: pull-up function requires PUD in SFIOR (SFIOR.2) to be zero (default value).
//		(See Misc_Init())
//
//  PORTx (input):  1=pull-up on, 0=pull-up off
//  PORTx (output): 1=output high (source), 0=output low (sink)
//  DDRx: 1=output, 0=input

//  PORTx and DDRx defaults are all bits zero.

 // PB7: 	
 // PB6: 	
 // PB5: 	Output, LCD DB7
 // PB4: 	Output, LCD DB6
 // PB3: 	Output, LCD DB5
 // PB2:	Output, LCD DB4
 // PB1: 	Output, LCD RS	
 // PB0:	Output, LCD EN
 PORTB = 
 	(1<<PB7)|(1<<PB6)|(1<<PB5)|(1<<PB4)|(1<<PB3)|(1<<PB2)|(1<<PB1)|(0<<PB0);
 DDRB = 
 	  (0<<DDB7)|(0<<DDB6)|(1<<DDB5)|(1<<DDB4)
 	|(1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0);

 // PC6:	
 // PC5:	Output, beeper
 // PC4:	Input, KeyIn ROW4
 // PC3:	Input, KeyIn ROW3
 // PC2:	Input, KeyIn ROW2
 // PC1:	Input, KeyIn ROW1
 // PC0:	Input, KeyIn ROW0
 PORTC = 
 	(0<<PC6)|(0<<PC5)|(1<<PC4)|(1<<PC3)|(1<<PC2)|(1<<PC1)|(1<<PC0);
 DDRC = 
 	    (0<<DDC6)|(1<<DDC5)|(0<<DDC4)
 	  |(0<<DDC3)|(0<<DDC2)|(0<<DDC1)|(0<<DDC0);

 // PD7: 	Output, KeyDrive COL3
 // PD6: 	Output, KeyDrive COL2
 // PD5: 	Output, KeyDrive COL1
 // PD4: 	Output, KeyDrive COL0
 // PD3: 	Output, serial connectiong to DDS MCU
 // PD2:  Output, DDS communication
 // PD1:	
 // PD0:
 PORTD = (1<<PD7)|(1<<PD6)|(1<<PD5)|(1<<PD4)
 		|(1<<PD3)|(1<<PD2)|(0<<PD1)|(1<<PD0);
 DDRD = 
 	  (1<<DDD7)|(1<<DDD6)|(1<<DDD5)|(1<<DDD4)
 	|(1<<DDD3)|(1<<DDD2)|(0<<DDD1)|(0<<DDD0);

}

void	ExtIntr_Init(void)
{
 // EICRA - External Interrupt Control Register A
 //	ISC1[1:0]: 	Interrupt Sense Control 1 Bit 1 and Bit 0
 //	ISC0[1:0]: 	Interrupt Sense Control 1 Bit 1 and Bit 0
 //					00 = The low level of INT1/INT0 generates an interrupt request.
 //					01 = Any logical change on INT1/INT0 generates an interrupt request.
 //					10 = The falling edge of INT1/INT0 generates an interrupt request.
 //					11 = The rising edge of INT1/INT0 generates an interrupt request.
 //
 EICRA = (0<<ISC11)|(0<<ISC10)|(0<<ISC01)|(0<<ISC00);

 // EIMSK - External Interrupt Mask Register
 //	INT1: 	External Interrupt Request 1 Enable, 1 = enable
 //	INT0: 	External Interrupt Request 0 Enable, 1 = enable
 //
 EIMSK = (0<<INT1)|(0<<INT0);

 // EIFR - External Interrupt Flag
 //	INTF1: 	External Interrupt Flag 1
 //	INTF0: 	External Interrupt Flag 0
 EIFR = (1<<INTF1)|(1<<INTF0);

 // PCICR - Pin Change Interrupt Control Register
 //	PCIE2: 	Pin Change Interrupt Enable 2, 1 = enable PCINT[23:16] interrupt
 //	PCIE1: 	Pin Change Interrupt Enable 1, 1 = enable PCINT[14:8] interrupt
 //	PCIE0:	 Pin Change Interrupt Enable 0, 1 = enable PCINT[7:0] interrupt
 //
 PCICR = (0<<PCIE2)|(0<<PCIE1)|(0<<PCIE0);

 // PCIFR - Pin Change Interrupt Flag Register
 //	PCIF2: 	Pin Change Interrupt Flag 2
 //	PCIF1: 	Pin Change Interrupt Flag 1
 //	PCIF0: 	Pin Change Interrupt Flag 0
 PCIFR = (1<<PCIF2)|(1<<PCIF1)|(1<<PCIF0);

 // PCMSK2 - Pin Change Mask Register 2 
 PCMSK2 = (0<<PCINT23)|(0<<PCINT22)|(0<<PCINT21)|(0<<PCINT20)
 		|(0<<PCINT19)|(0<<PCINT18)|(0<<PCINT17)|(0<<PCINT16);

 // PCMSK1 - Pin Change Mask Register 1 
 PCMSK1 = (0<<PCINT14)|(0<<PCINT13)|(0<<PCINT12)|(0<<PCINT11)
 		|(0<<PCINT10)|(0<<PCINT9)|(0<<PCINT8);

 // PCMSK0 - Pin Change Mask Register 0 
 PCMSK0 = (0<<PCINT7)|(0<<PCINT6)|(0<<PCINT5)|(0<<PCINT4)
 		|(0<<PCINT3)|(0<<PCINT2)|(0<<PCINT1)|(0<<PCINT0);
 
}

//-----------------------------------------------------------------------------
// UART0_Init
//-----------------------------------------------------------------------------
//
void Uart0_Init (void)
{
 // UCSR0A - USART Control and Status Register A
 //	RXC0:	1 = Receive complete [UCSR0A.7]
 //	TXC0:	1 = Transmit complete [UCSR0A.6 = 0]
 //	UDRE0:	1 = USART Data Register Empty [UCSR0A.5]
 //	FE0:		1 = Frame Error [UCSR0A.4]
 //	DOR0:	1 = Data Over Run [UCSR0A.3]
 //	UPE0:	1 = USART Parity Error [UCSR0A.2]
 //	U2X0:	1 = Double USART transmission speed (for asynchronous only) [UCSR0A.1 = 0]
 //	MPCM0:	1 = Multiprocessor Communication Mode [UCSR0A.0 = 0]
 //
 // UCSR0A defaults are all bits zero.
 UCSR0A = (0<<TXC0)|(0<<U2X0)|(0<<MPCM0);

 // UCSR0B - USART Control and Status Register B
 //	RXCIE0:	1 = Receive complete interrupt enable [UCSR0B.7 = 0]
 //	TXCIE0:	1 = Transmit complete interrupt enable [UCSR0B.6 = 0]
 //	UDRIE0:	1 = USART Data Register Empty interrupt enable [UCSR0B.5 = 0]
 //	RXEN0:	1 = Receiver enable [UCSR0B.4 = 1]
 //	TXEN0:	1 = Transmitter enable [UCSR0B.3 = 1]
 //	UCSZ02:	Character size, combined with UCSZ0.[1:0] in UCSR0C [UCSR0B.2 = 0]
 //	RXB80:	1 = Receive Date Bit 8 [UCSR0B.1]
 //	TXB80:	1 = Transmit Date Bit 8 [UCSR0B.0 = 0]
 //
 // UCSR0B defaults are all bits zero.

 // -- Enable receiver
 // -- Enable transmitter
 UCSR0B = (0<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0)|(1<<RXEN0)
 			|(1<<TXEN0)|(0<<UCSZ02)|(0<<TXB80);

 // UCSR0C - USART Control and Status Register C
 //	UMSEL0.[1:0]:USART Mode Select [UCSR0C.[7:6] = 00]
 //				00 = Asynchronous
 //				01 = Synchronous
 //				10 = Reserved
 //				11 = Master SPI
 //	UPM0.[1:0]:	Parity Mode [UCSR0C.[5:4] = 00]
 //				00 = Disabled
 //				01 = Reserved
 //				10 = Enabled, even parity
 //				11 = Enabled, odd parity
 //	USBS0:	Stop bit select, 0 = 1 stop bit, 1 = 2 stop bits [UCSR0C.3 = 0]
 //	UCSZ0.[1:0]:	Character size, combined with UCSZ02 in UCSR0B [UCSR0C.[2:1] = 11] 
 //				000 = 5-bit
 //				001 = 6-bit
 //				010 = 7-bit
 //				011 = 8-bit
 //				100 = Reserved
 //				101 = Reserved
 //				110 = Reserved
 //				111 = 9-bit
 //	UCPOL0:	Clock pority, for synchronous mode only [UCSR0C.0] (See datasheet)
 //
 // -- Select 8N1 format
 UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)
 		  |(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);

// Setup baudrate
 UBRR0H = (((F_CPU / 16) / Uart0_Baud) - 1) >> 8;
 UBRR0L = (((F_CPU / 16) / Uart0_Baud) - 1) & 0x00FF;
 
}


//-----------------------------------------------------------------------------
// Output one byte to Uart 0 - polling method
//-----------------------------------------------------------------------------
//
void	u0putc(U8 ch)
{
 while(!BitTest8(UCSR0A, (1<<UDRE0))) {
 	}
 UDR0 = ch;
}

//-----------------------------------------------------------------------------
// Receive one byte from Uart 0 - polling method
//-----------------------------------------------------------------------------
//
U8	u0getc(void)
{
 while(!BitTest8(UCSR0A, (1<<RXC0))) {
 	}
 return (UDR0);
}

//-----------------------------------------------------------------------------
// Receive state check
//	Return 1 if received data ready, otherwise return 0.
//-----------------------------------------------------------------------------
//
U8	u0rcheck(void)
{
 if(BitTest8(UCSR0A, (1<<RXC0))) {
 	return 1;
 	}
 return 0;
}



//-----------------------------------------------------------------------------
// Output zero terminated string to Uart 0 - polling method
//-----------------------------------------------------------------------------
//
void	uputs(U8 *s)
{
 while(*s != 0) {
 	u0putc(*s);
	s++;
 	}
}

// Outptu a ROM string
void	uputs_ROM(U8 *s)
{
 U8	tmp;

 while((tmp = pgm_read_byte(s))) {
 	u0putc(tmp);
	s++;
//	tmp = pgm_read_byte(s);
 	}
}

//-----------------------------------------------------------------------------
// Comp_Init
//-----------------------------------------------------------------------------
//

void Comp_Init(void)
{
 // ACD: 		Analog Comparator Disable
 // ACBG: 		Analog Comparator Bandgap Select
 //					1 - IN+ = bandgap, 0 - IN+ = AIN0
 // ACO: 		Analog Comparator Output
 // ACI: 			Analog Comparator Interrupt Flag
 // ACIE: 		Analog Comparator Interrupt Enable
 // ACIC: 		Analog Comparator Input Capture Enable
 // ACIS[1:0]:	Analog Comparator Interrupt Mode Select
 //					00 - Comparator Interrupt on Output Toggle.
 //					01 - Reserved
 //					10 - Comparator Interrupt on Falling Output Edge.
 //					11 - Comparator Interrupt on Rising Output Edge.
 //
 ACSR = (0<<ACD)|(0<<ACBG)|(0<<ACI)
 		|(0<<ACIE)|(1<<ACIC)|(1<<ACIS1)|(1<<ACIS0);

 // AIN1D, AIN0D:	AIN1, AIN0 Digital Input Disable
 //
 // -- Disable AIN0 digital input
 DIDR1 = (0<<AIN1D)|(1<<AIN0D);
}

//-----------------------------------------------------------------------------
// ADC Initialization
//-----------------------------------------------------------------------------
//
void ADC_Init (void)
{
 // REFS[1:0]:	Reference Selections [ADMUX.[7:6] = 00]
 //					00 = AREF, internal Vref turned off
 //					01 = AVcc with external cap at AREF pin
 //					10 = Reserved
 //					11 = Internal 1.1V Vref with external cap at AREF pin
 // ADLAR:		ADC Left Adjust Result, 1 = Left adjusted [ADMUX.5 = 0]
 // ADMUX.4:		Reserved
 // MUX[3:0]:		Analog channel selection [ADMUX.[3:0] = 0000]
 //					0000 = ADC0
 //					0001 = ADC1
 //					0010 = ADC2
 //					0011 = ADC3
 //					0100 = ADC4
 //					0101 = ADC5
 //					0110 = ADC6
 //					0111 = ADC7
 //					1000 - 1101 = Reserved
 //					1110 = 1.1V (internal reference)
 //					1111 = 0V (GND)
 //
 // -- Select channel ADC7
 ADMUX = (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)
 		|(0<<MUX3)|(1<<MUX2)|(0<<MUX1)|(1<<MUX0);

 // ADEN:	ADC Enable, 1 = enable ADC [ADCSRA.7 = 0]
 // ADSC:	ADC Start Conversion, 1 = to start conversion [ADCSRA.6 = 0]
 // ADATE:	ADC Auto Trig Enable, 1 = auto trigged by selected source [ADCSRA.5 = 0]
 // ADIF:		ADC	 interrupt flag, 1 = ADC completed [ADCSRA.4 = 0]
 // ADIE:	ADC interrupt enable, 1 = enable ADC interrupt [ADCSRA.3 = 0]
 // ADPS[2:0]: ADC Prescaler Selection [ADCSRA.[2:0] = 000]
 //				000 = 1/2 of system clock
 //				001 = 1/2
 //				010 = 1/4
 //				011 = 1/8
 //				100 = 1/16
 //				101 = 1/32
 //				110 = 1/64
 //				111 = 1/128
 // 
 // -- Enable ADC
 // -- Set ADC clock to 1/32 of F_CPU, about 8000000/32 = 250KHz
 ADCSRA = (0<<ADEN)|(0<<ADSC)|(0<<ADATE)|(1<<ADIF)
 		|(0<<ADIE)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0);

 // ACME: Analog Comparator Multiplexer Enable. 1 = use multiplexer for comparator
 // ADTS[2:0]: ADC Auto Trig Source select [ADCSRB.[2:0] = 000]
 //				000 = Free running
 //				001 = Analog comparator
 //				010 = External interrupt 0
 //				011 = Timer/Counter 0 compare match A
 //				100 = Timer/Counter 0 overflow
 //				101 = Timer/Counter 1 compare match B
 //				110 = Timer/Counter 1 overflow
 //				111 = Timer/Counter 1 capture event
 
 ADCSRB= (1<<ACME)|(0<<ADTS2)|(0<<ADTS1)|(0<<ADTS0);

 // ADC[5:0]D: 	Digital Input Disable [ADC[5:0]D = 000000]
 DIDR0 = (1<<ADC5D)|(1<<ADC4D)|(0<<ADC3D)|(0<<ADC2D)
 		|(0<<ADC1D)|(0<<ADC0D);
 
}

/*
// -- Do ADC on selected channel and return the result
U16	ADC_Poll(U8 channel)
{
 ADMUX &= 0xF0;
 ADMUX |= channel;
 
 // -- Start ADC
 BitSet8(ADCSRA, (1<<ADSC));
 while(!BitTest8(ADCSRA, (1<<ADIF))) {
 	// Wait for ADC to complete
 	}
 // -- ADIF has to be manually reset if not using ADC interrupt
 BitSet8(ADCSRA, (1<<ADIF));
 return (ADCL|(ADCH<<8));
}
*/

//-----------------------------------------------------------------------------
// VREF_Init
//-----------------------------------------------------------------------------
//
void	VREF_Init(void)
{

}

//-----------------------------------------------------------------------------
// Timer0_Init
//-----------------------------------------------------------------------------
//
void Timer0_Init(void)
{
 // COM0A[1:0]:	Compara Match Output A mode
 //				Non-PWM Mode:
 //					00 = Normal port operation, OC0A disconnected
 //					01 = Toggle OC0A on Compare Match
 //					10 = Clear OC0A on Compare Match
 //					11 = Set OC0A on Compare Match
 //				Fast PWM Mode:
 //					00 = Normal port operation, OC0A disconnected.
 //					01 = WGM02 = 0: Normal Port Operation, OC0A Disconnected
 //						 WGM02 = 1: Toggle OC0A on Compare Match
 //					10 = Clear OC0A on Compare Match, set OC0A at TOP
 //					11 = Set OC0A on Compare Match, clear OC0A at TOP
 //				Phase Correct PWM Mode:
 //					00 = Normal port operation, OC0A disconnected
 //					01 = WGM02 = 0: Normal Port Operation, OC0A Disconnected
 //						 WGM02 = 1: Toggle OC0A on Compare Match.
 //					10 = Clear OC0A on Compare Match when up-counting. Set OC0A on
 //						 Compare Match when down-counting.
 //					11 = Set OC0A on Compare Match when up-counting. Clear OC0A on
 //						 Compare Match when down-counting.
 // COM0B[1:0]:	Compara Match Output B mode
 //				Non-PWM Mode:
 //					00 = Normal port operation, OC0B disconnected
 //					01 = Toggle OC0B on Compare Match
 //					10 = Clear OC0B on Compare Match
 //					11 = Set OC0B on Compare Match
 //				Fast PWM Mode:
 //					00 = Normal port operation, OC0B disconnected.
 //					01 = Reserved
 //					10 = Clear OC0B on Compare Match, set OC0B at TOP
 //					11 = Set OC0B on Compare Match, clear OC0B at TOP
 //				Phase Correct PWM Mode:
 //					00 = Normal port operation, OC0B disconnected
 //					01 = Reserved
 //					10 = Clear OC0B on Compare Match when up-counting. Set OC0B on
 //						 Compare Match when down-counting.
 //					11 = Set OC0B on Compare Match when up-counting. Clear OC0B on
 //						 Compare Match when down-counting.
 // WGM0[1:0]:	Waveform Generation Mode
 //				Mode	WGM0[2:0]	T/C Mode  	TOP  	Update_OCRx		TOV0_Set
 //				0		000			Normal		0xFF		Immidiate		Max
 //				1		001			PWM, PC		0xFF		TOP				BOTTOM
 //				2		010			CTC			OCRA	Immidiate		MAX
 //				3		011			Fast PWM		0xFF		TOP				MAX
 //				4		100			Reserved		-		-				-
 //				5		101			PWM	, PC		OCRA	TOP				BOTTOM
 //				6		110			Reserved		-		-				-
 //				7		111			Fast PWM		OCRA	TOP				TOP
 //				(MAX = 0xFF, BOTTOM = 0x00)
 //
 // -- Set Timer0 to fast PWM mode
 // -- Output at OC0A
 TCCR0A = (1<<COM0A1)|(0<<COM0A0)|(0<<COM0B1)|(0<<COM0B0)
 		|(1<<WGM01)|(1<<WGM00);

 // FOC0A:		Force Output Compare A
 // FOC0B:		Force Output Compare B
 // WGM02: 		Waveform Generation Mode
 // CS0[2:0]: 	Clock Select
 //					000 = No clock source (Timer/Counter stopped)
 //					001 = clkio/1 (No prescaling)
 //					010 = clkio/8 (From prescaler)
 //					011 = clkio/64 (From prescaler)
 //					100 = clkio/256 (From prescaler)
 //					101 = clkio/1024 (From prescaler)
 //					110 = External clock source on T0 pin. Clock on falling edge
 //					111 = External clock source on T0 pin. Clock on rising edge
 //
 // -- No pre-scaler
 TCCR0B = (0<<FOC0A)|(0<<FOC0B)|(0<<WGM02)|(0<<CS02)|(0<<CS01)|(1<<CS00);

 TCNT0 = 0x00;
 OCR0A = 0xC0;  // Initial setting
 OCR0B = 0xff;

 // OCIE0B: 	Timer/Counter0 Output Compare Match B Interrupt Enable
 // OCIE0A: 	Timer/Counter0 Output Compare Match A Interrupt Enable
 // TOIE0: 	Timer/Counter0 Overflow Interrupt Enable
 //
 TIMSK0 = (0<<OCIE0B)|(0<<OCIE0A)|(0<<TOIE0);

 // OCF0B: 	Timer/Counter 0 Output Compare B Match Flag
 // OCF0A: 	Timer/Counter 0 Output Compare A Match Flag
 // TOV0: 	Timer/Counter0 Overflow Flag
 TIFR0 = (1<<OCF0B)|(1<<OCF0A)|(1<<TOV0);

}

/*
//-----------------------------------------------------------------------------
// Timer1_Init
//-----------------------------------------------------------------------------
//
void Timer1_Init (void)
{
 // COM1A[1:0]:	Compara Match Output A mode
 // COM1B[1:0]:	Compara Match Output B mode
 //				Non-PWM Mode:
 //					00 = Normal port operation, OC1A/OC1B disconnected
 //					01 = Toggle OC1A/OC1B on Compare Match
 //					10 = Clear OC1A/OC1B on Compare Match (Set output to low level).
 //					11 = Set OC1A/OC1B on Compare Match (Set output to high level).
 //				Fast PWM Mode:
 //					00 = Normal port operation, OC1A/OC1B disconnected
 //					01 = WGM13:0 = 14 or 15: Toggle OC1A on Compare Match, 
 //						 OC1B disconnected (normal port operation). For all other 
 //						 WGM1 settings, normal port operation, OC1A/OC1B disconnected.
 //					10 = Clear OC1A/OC1B on Compare Match, set OC1A/OC1B at TOP
 //					11 = Set OC1A/OC1B on Compare Match, clear OC1A/OC1B at TOP
 //				Phase Correct PWM Mode:
 //					00 = Normal port operation, OC1A/OC1B disconnected
 //					01 = WGM13:0 = 8, 9, 10 or 11: Toggle OC1A on Compare Match,
 //						 OC1B disconnected (normal port operation). For all other 
 //						 WGM1 settings, normal port operation, OC1A/OC1B disconnected.
 //					10 = Clear OC1A/OC1B on Compare Match when upcounting.
 //						 Set OC1A/OC1B on Compare Match when downcounting.
 //					11 = Set OC1A/OC1B on Compare Match when upcounting.
 //						 Clear OC1A/OC1B on Compare Match when downcounting.
 // WGM1[1:0]:	Waveform Generation Mode
 //				Mode	WGM1[3:0]	T/C Mode  	TOP  	Update_OCRx		TOV1_Set
 //				0		0000		Normal		0xFFFF	Immidiate		Max
 //				1		0001		PWM, PC8	0x00FF	TOP				BOTTOM
 //				2		0010		PWM, PC9	0x01FF	TOP				BOTTOM
 //				3		0011		PWM, PC10	0x03FF	TOP				BOTTOM
 //				4		0100		CTC			OCR1A	Immediate 		MAX	
 //				5		0101		PWM, PC8	0x00FF	TOP				TOP
 //				6		0110		PWM, PC9	0x01FF	TOP				TOP
 //				7		0111		PWM, PC10	0x03FF	TOP				TOP
 //				8		1000		PWM, PFC	ICR1	BOTTOM			BOTTOM
 //				9		1001		PWM, PFC	OCR1A	BOTTOM			BOTTOM
 //				10		1010		PWM, PC		ICR1	TOP				BOTTOM
 //				11		1011		PWM, PC		OCR1A	TOP				BOTTOM
 //				12		1100		CTC			ICR1	Immediate 		MAX	
 //				13		1101		Reserved		-		-				-
 //				14		1110		Fast PWM		ICR1	TOP				TOP
 //				15		1111		Fast PWM		OCR1A	TOP				TOP
 //				
 // -- Normal  -- WGM1[3:0] = 0000
 TCCR1A = (0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)
 		|(0<<WGM11)|(0<<WGM10);

 // ICNC1: 		Input Capture Noise Canceler
 // ICES1: 		Input Capture Edge Select, 0 = falling edge, 1 = rising edge
 // WGM1[3:2]:	Waveform Generation Mode
 // CS1[2:0]: 	Clock Select
 //					000 = No clock source (Timer/Counter stopped)
 //					001 = clkio/1 (No prescaling)
 //					010 = clkio/8 (From prescaler)
 //					011 = clkio/64 (From prescaler)
 //					100 = clkio/256 (From prescaler)
 //					101 = clkio/1024 (From prescaler)
 //					110 = External clock source on T1 pin. Clock on falling edge
 //					111 = External clock source on T1 pin. Clock on rising edge
 //
 // -- Select 1/8 of clkio as Timer 1 clock (8/12)us cycle 
 TCCR1B = (0<<ICNC1)|(1<<ICES1)|(0<<WGM13)|(0<<WGM12)
 		|(0<<CS12)|(1<<CS11)|(0<<CS10);

 // FOC1A: 	Force Output Compare for Channel A
 // FOC1B: 	Force Output Compare for Channel B
 TCCR1C = (0<<FOC1A)|(0<<FOC1B);

 TCNT1H = 0x00;
 TCNT1L = 0x00;

 // -- Set initial PWM pulse width to 1500 us
 OCR1AH =  1500 >> 8;
 OCR1AL = 1500 & 0x00ff;

 OCR1BH = 0x00;
 OCR1BL = 0x00;

 // -- Set TOP to 20000 us
 ICR1H = 20000 >> 8;
 ICR1L = 20000 & 0x00ff;

 // ICIE1: 	Timer/Counter1 Input Capture Interrupt Enable
 // OCIE1B: 	Timer/Counter1 Output Compare Match B Interrupt Enable
 // OCIE1A: 	Timer/Counter1 Output Compare Match A Interrupt Enable
 // TOIE1: 	Timer/Counter1 Overflow Interrupt Enable
 //
 TIMSK1 = (0<<ICIE1)|(0<<OCIE1B)|(0<<OCIE1A)|(0<<TOIE1);

 // ICF1: 	Timer/Counter1, Input Capture Flag
 // OCF1B: 	Timer/Counter 1 Output Compare B Match Flag
 // OCF1A: 	Timer/Counter 1 Output Compare A Match Flag
 // TOV1: 	Timer/Counter1 Overflow Flag
 TIFR1 = (1<<ICF1)|(1<<OCF1B)|(1<<OCF1A)|(1<<TOV1);
 
}

*/

//-----------------------------------------------------------------------------
// Timer2_Init
//-----------------------------------------------------------------------------
//
// Timer 2 is used for ADC1 sampling counter and address capture
//

void Timer2_Init (void)
{
 // COM2A[1:0]:	Compara Match Output A mode
 //				Non-PWM Mode:
 //					00 = Normal port operation, OC2A disconnected
 //					01 = Toggle OC2A on Compare Match
 //					10 = Clear OC2A on Compare Match
 //					11 = Set OC2A on Compare Match
 //				Fast PWM Mode:
 //					00 = Normal port operation, OC2A disconnected.
 //					01 = WGM22 = 0: Normal Port Operation, OC2A Disconnected
 //						 WGM22 = 1: Toggle OC2A on Compare Match
 //					10 = Clear OC2A on Compare Match, set OC2A at TOP
 //					11 = Set OC2A on Compare Match, clear OC2A at TOP
 //				Phase Correct PWM Mode:
 //					00 = Normal port operation, OC2A disconnected
 //					01 = WGM22 = 0: Normal Port Operation, OC2A Disconnected
 //						 WGM22 = 1: Toggle OC0A on Compare Match.
 //					10 = Clear OC2A on Compare Match when up-counting. Set OC2A on
 //						 Compare Match when down-counting.
 //					11 = Set OC2A on Compare Match when up-counting. Clear OC2A on
 //						 Compare Match when down-counting.
 // COM2B[1:0]:	Compara Match Output B mode
 //				Non-PWM Mode:
 //					00 = Normal port operation, OC2B disconnected
 //					01 = Toggle OC2B on Compare Match
 //					10 = Clear OC2B on Compare Match
 //					11 = Set OC2B on Compare Match
 //				Fast PWM Mode:
 //					00 = Normal port operation, OC2B disconnected.
 //					01 = Reserved
 //					10 = Clear OC2B on Compare Match, set OC2B at TOP
 //					11 = Set OC2B on Compare Match, clear OC2B at TOP
 //				Phase Correct PWM Mode:
 //					00 = Normal port operation, OC2B disconnected
 //					01 = Reserved
 //					10 = Clear OC2B on Compare Match when up-counting. Set OC2B on
 //						 Compare Match when down-counting.
 //					11 = Set OC2B on Compare Match when up-counting. Clear OC2B on
 //						 Compare Match when down-counting.
 // WGM2[1:0]:	Waveform Generation Mode
 //				Mode	WGM2[2:0]	T/C Mode  	TOP  	Update_OCRx		TOV0_Set
 //				0		000			Normal		0xFF		Immidiate		Max
 //				1		001			PWM, PC		0xFF		TOP				BOTTOM
 //				2		010			CTC			OCRA	Immidiate		MAX
 //				3		011			Fast PWM		0xFF		TOP				MAX
 //				4		100			Reserved		-		-				-
 //				5		101			PWM	, PC		OCRA	TOP				BOTTOM
 //				6		110			Reserved		-		-				-
 //				7		111			Fast PWM		OCRA	TOP				TOP
 //				(MAX = 0xFF, BOTTOM = 0x00)
 //
 // -- Set Timer2 to CTC mode
 TCCR2A = (0<<COM2A1)|(0<<COM2A0)|(0<<COM2B1)|(0<<COM2B0)
 		|(1<<WGM21)|(0<<WGM20);

 // FOC2A:		Force Output Compare A
 // FOC2B:		Force Output Compare B
 // WGM22: 		Waveform Generation Mode
 // CS2[2:0]: 	Clock Select
 //					000 = No clock source (Timer/Counter stopped)
 //					001 = clkT2s/1 (No prescaling)
 //					010 = clkT2s/8 (From prescaler)
 //					011 = clkT2s/32 (From prescaler)
 //					100 = clkT2s/64 (From prescaler)
 //					101 = clkT2s/128 (From prescaler)
 //					110 = clkT2s/256 (From prescaler)
 //					111 = clkT2s/1024 (From prescaler)
 //
 // -- 1/64 prescaler. 3.2 us period @ 20MHz system clock
 TCCR2B = (0<<FOC2A)|(0<<FOC2B)|(0<<WGM22)|(1<<CS22)|(0<<CS21)|(0<<CS20);

 TCNT2 = 0x00;
 OCR2A = 32;  // about 100us interval for keypad scanning
 OCR2B = 0x60;

 // OCIE2B: 	Timer/Counter2 Output Compare Match B Interrupt Enable
 // OCIE2A: 	Timer/Counter2 Output Compare Match A Interrupt Enable
 // TOIE2: 	Timer/Counter2 Overflow Interrupt Enable
 //
 // -- Enable OC2A interrupt
 TIMSK2 = (0<<OCIE2B)|(1<<OCIE2A)|(0<<TOIE2);

 // OCF2B: 	Timer/Counter 2 Output Compare B Match Flag
 // OCF2A: 	Timer/Counter 2 Output Compare A Match Flag
 // TOV2: 	Timer/Counter 2 Overflow Flag
 TIFR2 = (1<<OCF2B)|(1<<OCF2A)|(1<<TOV2);

 // EXCLK: 	Enable External Clock Input, 1 = enable external clock in Asynchronous Mode
 // AS2: 		Asynchronous Timer/Counter2, 0 - T2 clocked with clkio, 1 = ext. clock (TOSC1)
 ASSR = (0<<EXCLK)|(0<<AS2);
 
}

//-----------------------------------------------------------------------------
// Interrupt Service Routines
//-----------------------------------------------------------------------------
// Timer0 overflow ISR
SIGNAL(SIG_OUTPUT_COMPARE2A)
{

 BitSet8(KFlags, (1<<KScanRdy));

 if(GTimer > 0) {
 	GTimer--;
 	}
}

// ------------------------------------------------------
// Write byte to DM0802B. If RS=0 write to command, otherwise write to data
// 4-bit interface assumed
void	LCD_WriteByte(U8 RS, U8 byte)
{
 Dly50us();

 if(RS) {		
 	BitSet8(LCD_RS_Port, (1 << LCD_RS_Bit));
 	}
 else {
 	BitClr8(LCD_RS_Port, (1 << LCD_RS_Bit));
 	}

 // Please note that for 105-08500-00A the R/W signal of LCD has been hard connected to GND
 
 // Write higher nibble
 LCD_D_Port = (LCD_D_Port & (~LCD_D_Bit)) | ((byte & 0xF0) >> 2);
 Dly1us();
 BitSet8(LCD_EN_Port, (1 << LCD_EN_Bit));
 Dly1us();
 BitClr8(LCD_EN_Port, (1 << LCD_EN_Bit));

 // Write lower nibble
 LCD_D_Port = (LCD_D_Port & (~LCD_D_Bit)) | ((byte & 0x0F) << 2);
 Dly1us();
 BitSet8(LCD_EN_Port, (1 << LCD_EN_Bit));
 Dly1us();
 BitClr8(LCD_EN_Port, (1 << LCD_EN_Bit));

}

void	LCD_Init(void)
{
 Dly_ms(40);
 
 // -- Set to 8bit interface
 LCD_D_Port = 0b00001100;
 Dly1us();
 // Generate strobe
 BitSet8(LCD_EN_Port, (1 << LCD_EN_Bit));
 Dly1us();
 BitClr8(LCD_EN_Port, (1 << LCD_EN_Bit));
 
 Dly_ms(10);
 
 // -- Set to 8bit interface
 LCD_D_Port = 0b00001100;
 Dly1us();
 // Generate strobe
 BitSet8(LCD_EN_Port, (1 << LCD_EN_Bit));
 Dly1us();
 BitClr8(LCD_EN_Port, (1 << LCD_EN_Bit));

 Dly_ms(1);
 
 // -- Set to 4-bit interface
 LCD_D_Port = 0b00001000;
 Dly1us();
 // Generate strobe
 BitSet8(LCD_EN_Port, (1 << LCD_EN_Bit));
 Dly1us();
 BitClr8(LCD_EN_Port, (1 << LCD_EN_Bit));

 // -- Set 2-line display and use 5X7 font
 LCD_WriteByte(LCD_CMD, 0b00101000);

 // -- Turn-off display
 LCD_WriteByte(LCD_CMD, 0b00001000);

 // -- Set cursor home
 LCD_WriteByte(LCD_CMD, 0b00000001);

 Dly_ms(5);

 // -- Set Entry mode: cursor shift right for each char entered
 LCD_WriteByte(LCD_CMD, 0b00000110);

 // -- Turn-on display with CURSOR OFF and BLINK OFF
 LCD_WriteByte(LCD_CMD, 0b00001100);

 Dly50us();

}

U8	UpdateLCD(U8 symbol)
{
 U8 tmp0;

 // -- Set address to 0x00 for first row
 LCD_WriteByte(LCD_CMD, 0b10000000);

 tmp0 = 0;
 while(tmp0 < 16) {
 	LCD_WriteByte(LCD_DATA, DispBuf[tmp0]);
	tmp0++;
 	}
 
 // -- Set address to 0x40 for second row
 LCD_WriteByte(LCD_CMD, 0b11000000);

 tmp0 = 0;
 while(tmp0 < 16) {
 	LCD_WriteByte(LCD_DATA, DispBuf[tmp0+16]);
	tmp0++;
 	}

 if(symbol == 'l') {
 	return 'z';
 	}
 return tmp0;
}

void	ClrDispBuf(void)
{
 U8 	tmp0;

 tmp0 = 0;
 while(tmp0 < 32) {
 	DispBuf[tmp0] = ' ';
	tmp0++;
 	}
}

void	SetDigitalPot(U8 WiperPos)
{
 U8	tmp;
 
 // Down moving wiper to 0 position
 BitClr8(DPOT_CS_Port, (1 << DPOT_CS_Bit));
 Delay(2);
 BitClr8(DPOT_UD_Port, (1 << DPOT_UD_Bit));
 Delay(2);
 
 tmp = 100;
 while(tmp) {
 	BitClr8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
 	Delay(2);
 	BitSet8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
 	Delay(2);
	tmp--;
 	}

 // Up moving wiper to desired position
 BitSet8(DPOT_UD_Port, (1 << DPOT_UD_Bit));
 Delay(2);
 while(WiperPos) {
 	BitClr8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
 	Delay(2);
  	BitSet8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
	Delay(2);
	WiperPos--;
 	}

 // Keep nINC low for taking nCS high to avoid store wiper position

 // -- v014 begin [2013.10.28]
 //	Do not set nINC to low here since this make wiper position go up one step,
 //	which limits amplitude from being set to 0
 
// BitClr8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
// Delay(2);

 // -- v014 end

 // Keep CS HIGH
 BitSet8(DPOT_CS_Port, (1 << DPOT_CS_Bit));
 Delay(2);

 // Keep INC normally HIGH
 BitSet8(DPOT_INC_Port, (1 << DPOT_INC_Bit));

}

/*
void	SetDigitalPot(U8 WiperPos)
{
 U8	tmp;
 
 BitClr8(DPOT_CS_Port, (1 << DPOT_CS_Bit));
 Delay(50);
 
 if(WiperPos > PrevWiperPos) {
 	BitSet8(DPOT_UD_Port, (1 << DPOT_UD_Bit));
	tmp = WiperPos - PrevWiperPos;
 	}
 else if(WiperPos < PrevWiperPos) {
 	BitClr8(DPOT_UD_Port, (1 << DPOT_UD_Bit));
	tmp = PrevWiperPos - WiperPos;
 	}
 else {
 	return;
 	}

 PrevWiperPos = WiperPos;
  	
 while(tmp) {
 	BitClr8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
 	Delay(20);
  	BitSet8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
	Delay(20);
	tmp--;
 	}

 // Keep nINC low for taking nCS high to avoid store wiper position
 BitClr8(DPOT_INC_Port, (1 << DPOT_INC_Bit));
 Delay(20);

 // Keep CS HIGH
 BitSet8(DPOT_CS_Port, (1 << DPOT_CS_Bit));
 Delay(20);

 BitSet8(DPOT_INC_Port, (1 << DPOT_INC_Bit));

}
*/

void UpdateOfs(U32 new_ofs)
{

 OCR0A = 0xFF - (new_ofs * 255)/OFSmax;
  
}

// -----------------------------------------------------------------
// EEPROM operations
//
U8 EE_Read(U16 uiAddress)
{
 /* Wait for completion of previous write */
 while(EECR & (1<<EEPE));

 /* Set up address register */
 EEARL= uiAddress;
 EEARH = uiAddress>>8;

 /* Start eeprom read by writing EERE */
 EECR |= (1<<EERE);
 /* Return data from Data Register */
 return EEDR;
}

void EE_Write(U16 uiAddress, U8 ucData)
{
 /* Wait for completion of previous write */
 while(EECR & (1<<EEPE));

 /* Set up address and Data Registers */
 EEARL= uiAddress;
 EEARH = uiAddress>>8;
 EEDR = ucData;

 cli();
 
 /* Write logical one to EEMPE */
 EECR |= (1<<EEMPE);
 /* Start eeprom write by setting EEPE */
 EECR |= (1<<EEPE);

 sei();
}


void KeyScan(void)
{
 U8	tmp0, tmp1;
 
 // Scan
 tmp0 = 0xDF;
 KScanBuf = NoKey;
 while(BitTest8(tmp0, 0x01)) {
	KeyDrive_Port = (tmp0 & KeyDrive_Bits) | (KeyDrive_Port & (~KeyDrive_Bits));
	Delay(20);
	// Read keypad input
	tmp1 = (KeyIn_Port & KeyIn_Bits);
	if(tmp1 != KeyIn_Bits) {
		KScanBuf &= (((U16)tmp0  << 8) |(((U16)tmp1 | (~KeyIn_Bits)) & 0x00FF));
		}
	tmp0 >>= 1;
	tmp0 |= 0x80;
 	}
 KeyDrive_Port |= KeyDrive_Bits;

 // Read SW22
 KeyDrive_Port1 = (KeyDrive_Port1 & (~KeyDrive_Bits1));
 Delay(20);
 tmp1 = (KeyIn_Port & KeyIn_Bits);
 if(tmp1 != KeyIn_Bits) {
	KScanBuf &= ((0xFE00) |(((U16)tmp1 | (~KeyIn_Bits)) & 0x00FF));
	}
 KeyDrive_Port1 |= KeyDrive_Bits1;
 
 // -- Debouncing
 if((KScanBuf == NoKey) || (KScanBuf != KScan)) {
 	KScan = KScanBuf;
	KCount = 0;
	KHCount = 0;
	KTimeChk = KH_val;
 	}
 else {
	KCount++;
	if(KCount > KD_val) {
		if(KCount == 255) {
			KCount = KD_val;
			if(++KHCount == KTimeChk) {
				// Set key hold code
//				KScan &= 0x7FFF;
				KeyConvert((KeyScanCode *)KScanCodeTab, KScan);
				// Change KTimeChk for key repeat
//				KTimeChk += KR_Time;
				}
			}
		}
	else if(KCount == KD_val) {
		KeyConvert((KeyScanCode *)KScanCodeTab, KScan);
		}
	}

 // Process encoder
 tmp1 >>= 1;
 tmp1 &= 0x03;
 enc_state <<= 2;
 enc_state |= tmp1;
 enc_cnt = pgm_read_byte((U8 *)enc_table + (enc_state & 0x0F));
 if(enc_cnt == 1) {
//	BitSet8(KFlags, (1<<Enc_CCW));
	KeyCode = KC_CCW;
	KeyParam = 0;
	}
 if(enc_cnt == 0xFF) {
//	BitSet8(KFlags, (1<<Enc_CW));
	KeyCode = KC_CW;
	KeyParam = 1;
	}

 }

void	KeyConvert(KeyScanCode *KSCTab, U16 KSCode)
{
 U8	tmp0;
 U16 tmp1;
 
 tmp0 = 0;
 while(tmp0 < KC_Max) {
 	tmp1 = pgm_read_word((U16 *)(KSCTab + 0));
	if(tmp1 == KSCode) {
		// -- Match found
		KeyCode = pgm_read_byte((U8 *)KSCTab + 2);
		KeyCodeBuf = KeyCode;
		KeyParam = pgm_read_byte((U8 *)KSCTab + 3);
		return;
		}
	else {
		// -- Proceed to next entry
		KSCTab = (KeyScanCode *)((U8 *)KSCTab + 4);
		tmp0++;
		}
	
 	}
}

void	SendBuf(U8 *buf)
{
 U8 tmp;

 // Set handshake pin to output
 BitSet8(COMM_HS_Port_Dir, (1 << COMM_HS_Bit));
 
 BitClr8(COMM_HS_Port, (1 << COMM_HS_Bit));
 Delay(200);
 SendByte(CommSync);
 Delay(200);

 tmp = DDS_Comm_Frame_Len;
 while(tmp) {
	SendByte(*buf++);
	Delay(200);
	tmp--;
 	}

 BitSet8(COMM_HS_Port, (1 << COMM_HS_Bit));

 // Set handshake pin to input
 BitClr8(COMM_HS_Port_Dir, (1 << COMM_HS_Bit));
}

