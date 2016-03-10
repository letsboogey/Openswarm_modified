#include "kilolib.h"
#include <stdlib.h>
#include <util/delay.h>     // delay macros for the delay in infinite loop

void loopfun();

void setup() {
    // put your setup code here, to be run only once
}

void loop() {
    // put your main code here, to be run repeatedly
    	// allocate memory for the stack:
	int * p;
	p=malloc(sizeof(int)*64);
	//split ptr into 2 8 bit words:
	uint16_t dum;
	dum=(uint16_t)p; //casts ptr to variable
	uint8_t ptrlowC = dum & 0xff;
	uint8_t ptrhighC = (dum >> 8);
	
	//get fake PC
	void *funcptr=loopfun;
	dum=(uint16_t)funcptr;
	uint8_t pclowC = dum & 0xff;
	uint8_t pchighC = (dum >> 8);
	
	// set stack pointer to allocated memory location -1
	asm(
	"OUT __SP_H__, %[ptrhighA]\n\t" // address 0x3E is SPH. Only Lowest 3 bits are used. 
	"MOV R16, %[ptrlowA]\n\t"
	"SUBI R16,0x01\n\t"
	"OUT __SP_L__, R16\n\t" // address 0x3D is SPL. All bits are used.
	
	//set frame pointer to allocated memory location
	"MOV r28,%[ptrlowA]\n\t"
	"MOV R29, %[ptrhighA]\n\t"
	//thus we have successfully setup the stack, methinks.
	
	//Switch to loopfun using the new stack 
	"PUSH %[pclowA]\n\t"//store fake pc
	"PUSH %[pchighA]\n\t"
	"PUSH R29\n\t" //store FP
	"PUSH R28\n\t"
	"IN R28, __SP_L__\n\t" //set FP to SP value
	"IN R29, __SP_H__\n\t"
	: //output operands
	:[ptrhighA]"r" (ptrhighC),[ptrlowA]"r" (ptrlowC), [pclowA] "a" (pclowC),[pchighA]"a"(pchighC)//input operands
	: "r16" //clobbers list
	);
	
	//return;
	asm(
	//"ADIW R28,0x06" // this adds an amount to the frame pointer that seems dependant on the function    
	"IN R0,0x3F\n\t"		
	"CLI\n\t"
	"OUT 0x3E,R29\n\t"	
	"OUT 0x3F,R0\n\t"	
	"OUT 0x3D,R28\n\t"
	"POP R28\n\t"	
	"POP R29\n\t" 
	//"POP R16\n\t" // was this one here as a result of the clobbers list above? This always seems to occur with "return"
	"RET\n\t" 
	);
	
	//should never run to here
	while(1)
	{
	}
	loopfun();
}

int main() {
    // initialize hardware
    kilo_init();
    // start program
    kilo_start(setup, loop);

    return 0;
}

void loopfun()
{
	while(1)
	{
        set_color(RGB(3,0,0));
        _delay_ms(500);
        set_color(RGB(0,0,3));
        _delay_ms(500);
	}
}