;#define __AVR_ATmega48__
#include <avr/io.h>
#include <avr/Sfr_defs.h>
;#define __SFR_OFFSET 0

;#define	ZH	31      		; ZH, ZL have been defined in avr/io.h
;#define	ZL	30  
#define	RXtemp	r29
#define	RXparm  	r28  
#define	RXsave  	r27    
#define	RXtemp1	r26

#define	nreg		7         	; The number of registers saved in stack
  
#define	TXCIE	6
#define	UDRIE	5
#define	TXC		6

        .section .text

 	.global	Dly1us
Dly1us: 	
 	
 	nop
 	nop
 	nop
 	nop
 	nop
 	
 	nop
 	nop
 	nop
 	nop
 	nop
 	
	ret 	


; -----------------------------------------------------------------------
; Software Uart - transmit one byte through a pin 
;	SBuf = Byte to be sent
;	BitTime = Counter to time a bit
;	BitCount = Counter for bits to send
;
; 	Bit time calculation:
;		Instruction cycle = 1/20MHz
;		Desired baudrate = 38400 bps
;		Required bit time = 1/38400 second
;		Timing loop = 3 instruction cycles
;		Loops required = ((1/38400) / (3 * 1/20)) * 10**6
;					   = 173.6 (174)
;.EQU		BPS 	= 83	
;.EQU		ComPort	= PORTB
;.EQU		TXDpin	= PORTB3
;.EQU		RXDpin	= PINB2
#define	BPS			174		; 38400 bps @ 20MHz
#define	BPS_HALF	87
#define	TXDPort		PORTD
#define	TXDpin		PD3
#define	RXDPort		PIND
#define	RXDpin		PIND2
#define	BitTime		r30
#define	BitCount		r31
#define	SBuf			r24

	; Receive into r24. r17 will be broken. (16 words)
	.global	RecvByte
RecvByte: 	
	cli
	push		r30

	; SET TXDpin to input mode
;	cbi		DDRB, TXDpin
	
	ldi		SBuf, 0x80		;Shift register for receive
	ldi		BitTime, BPS_HALF	;Wait for half bit time
RB0:
	dec		BitTime			;
	brne		RB0				;/
RB1:
	ldi		BitTime, BPS		;Wait for full bit time
RB2:
	dec		BitTime			;
	brne		RB2				;/
	lsr		SBuf				;Next bit
	sbic		_SFR_IO_ADDR(RXDPort), RXDpin		
	ori		SBuf, 0x80		;/
	brcc		RB1				;All bits received?  no, continue
				;                    yes, exit
	ldi		BitTime, BPS		;Wait for full bit time
RB3:
	dec		BitTime			;
	brne		RB3				;/

	; Set TXDpin to output mode
;	sbi		DDRB, TXDpin

	pop		r30
	sei
	ret       

	.global	SendByte   
SendByte:
	cli
	push		r30
	push		r31
	
	; Set TXDpin to output mode
;	sbi		DDRB, TXDpin

	; Set TXDpin to HIGH for 2 bit time
	sbi		_SFR_IO_ADDR(TXDPort), TXDpin	
	ldi		BitTime, BPS 	; Wait for full 2 bit time
SB00:
	dec		BitTime		
	brne		SB00
	
	ldi		BitTime, BPS 	; Wait for full 2 bit time
SB0:
	dec		BitTime		
	brne		SB0

	ldi		BitCount, 9		; Bit count
	clc						; Pre-load start bit
SB1:
	; Send one bit
	brcs		SB2				
	cbi		_SFR_IO_ADDR(TXDPort), TXDpin
	rjmp		SB201
SB2:
	sbi		_SFR_IO_ADDR(TXDPort), TXDpin

SB201:
	ldi		BitTime, BPS - 1	; Wait for full bit time
SB3:
	dec		BitTime		
	brne		SB3		

	lsr		SBuf				; Next bit
	dec		BitCount			; All sent ?
	brne		SB1				; No, coutinue

	; Send stop bit
	sbi		_SFR_IO_ADDR(TXDPort), TXDpin
	ldi		BitTime, BPS - 1	; Wait for full bit time
SB4:
	dec		BitTime		
	brne		SB4		

	; Restore TXDpin to input mode
;	cbi		DDRB, TXDpin

	pop		r31
	pop		r30
	sei
	ret


.end
