/*
*****************************************************************************
  Patrick Schlott, 2/1/2015
  Homework 3 program: data storage
;-----------------------------------------------------------------------------
;Create a program that will take input from PB1: push for t< 0.5s and it’s a 0, for >0.5s and it’s a 1.  
Store the data as the LSB of a variable called new_dat.  Logical shift left the existing data in new_dat.
Store the data into an array called collected_dat[10] when PB2 is pushed.  Keep only the LS 4 bits 
in new_data and load the array FILO.

;------------------------------------------------------------------------------
;SUBROUTINES/FUNCTIONS/INTERRUPT ROUTINES: 

Main:  will infinitely loop querying the buttons with if statements.  If PB1 is pushed new_dat will shift left.
If PB is held for >500ms then the LSB will be a 1, if <500 then a 0.  In effect this will enter data one bit at 
a time into new_dat.
If PB2 is pushed then the function Store_dat will be executed.

void Store_dat(collected_dat, new_dat) which will store the new data into the array.  Describe how this works


Sys_ticks interrupt: uses the overflow interrupt of the sys_tick to keep track of time.
  
;-------------------------------------------------------------------------------
;Input/Output

led1	ptc0
led2   	ptc1
led3	ptc2
led4	ptc3

pb1		pta1
pb2		pta2
pb3		pta4
pb4		pta5

GPIOA_PDIR & 0x2 = PB1
GPIOA_PDIR & 0x04 = PB2
GPIOA_PDIR & 0x10 = PB3
GPIOA_PDIR & 0x20 = PB4
;------------------------------------------------------------------------------
;VARIABLES:
char i: used for counting
char new_dat is the entered new data from PB1
collected_dat[10]  is the 10 bit array that stores the entered data every time PB2 is pushed.

int sys_ticks: volatile variable for the systicks interrupt.  Increments every 1ms

;*****************************************************************************

*/


#include "derivative.h" /* include peripheral declarations */
#include "my_io_apps.c" /* include peripheral declarations */
#include "my_interrupts.c"


/******* Prototype Function declarations ******/

void store_dat(char * ,char);
void display_dat(char);

/**************Global Variables Here************************/

/***********************************************************/
int main(void) {
 
//*******Local Variable Declarations for main *****************
char i;
char new_dat=0;
char collected_dat[10];

//******Initial Chip Settings******************

//  The student user should not make changes here unless
//explicitly instructed to do so
int pll_err=pll_init(8000000,0,1,4,24,1);     //Use the crystal oscillator to drive the PLL

//******User chip module initializations***************************************

  SIM_SCGC5=0xa00;					//0b0000101000000000  turn on port A and C    
  
  PORTC_PCR0=0x102;					//0b0000000100000010 activate the MUXs
  PORTC_PCR1=0x102;
  PORTC_PCR2=0x102;
  PORTC_PCR3=0x102;
  
  PORTA_PCR1=0x102;					//0b0000000100000010 activate the MUXs
  PORTA_PCR2=0x102;
  PORTA_PCR4=0x102;
  PORTA_PCR5=0x102;
    
  // outputs initializations
  
   GPIOC_PDDR = 0b00001111;		//set the outputs direction
   GPIOA_PDDR &= ~0b110110;		//set the input directions
   //Initialize the  sys tick interrupt
   syst_tick_init();
   // turn off the LEDs
   
   GPIOC_PDOR = 0b0001111;
	
//*************The infinte loop.  Could use while(1);	
	for(;;) {
		
		sys_ticks=0;
		
		while(sys_ticks<50);
		
		if(!(GPIOA_PDIR & 0x2))	//GPIOA_PDIR & 0x02 changed to 0x2, did not recognise button 1
			{
			delay_ms(10);      //add a debounce
			sys_ticks=0;
					
			while(!(GPIOA_PDIR & 0x2));
			
			if(sys_ticks>500)new_dat=(new_dat<<1)+1; 	// add 1 to new_dat if pb1 is held for more than .5 seconds
			else new_dat=new_dat<<1;					//otherwise, shift 1 bit over in new_dat
			
			delay_ms(10);      //debounce
			}
		if(!(GPIOA_PDIR & 0x04))
			{
			delay_ms(10);      //add a debounce
					
			store_dat(collected_dat,new_dat);	
					
			delay_ms(10);      //debounce
			}	
		if(!(GPIOA_PDIR & 0x10))	//PB3
		{
			delay_ms(10);	//debounce
			display_dat(new_dat);
			while(!(GPIOA_PDIR & 0x10));	
			delay_ms(10);
		}
		if(!(GPIOA_PDIR & 0x20))	//PB4
			{
			
			for(i=0;i<10;i++)
				{
				display_dat(collected_dat[i]);
				delay_ms(1000);
				}
			}
		}
	return 0;
    }

void store_dat(char *p,char dat){
	char i;
	
	for(i=9;i>=1;i--)	//10 digit array + pointer
		{
		*(p+i)=*(p+i-1);
		}
	 *p=(dat&0x0f);	// set array equal to 0000 1111
	 return;   
}

void display_dat(char data){
	GPIOC_PDOR=~data;    // Since the leds are C0,C1,C2,C3 the this will display the 4 LSBs of data on leds
}
