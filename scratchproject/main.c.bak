
//TODO: enable nested interrupts

#include <msp430x14x.h>
//given a 10 msec interval for the timer and an average of 4 cycles/instruction, we get
//to execute 20 000 instruction in on clock tick
//this should be enough


#define UART0_INT_DRIVEN_RX
#define DEBUG
#define BOUNCE_DELAY 1 //40 milliseconds
#define ROWCOUNT 2

unsigned char thr_char; /* hold char from UART RX*/
unsigned char rx_flag; /* receiver rx status flag */
enum taskType  { button, serial};

void Delay(void);
void SpeedUpClock(void);
void UART_Initialize(void);
void UART0_putchar(char c);

void KeyScan(void);
void WaitForRelease(void);  
void SetForPress(void);

#ifdef DEBUG
char t = 0;  
#endif

char time = 0;
char edge = 0, errorFlag = 0, key = 0, keyDown = 0; 
//rising edge = 0; falling edge = 1

char tasks[3] = {0};

void
main(void)
{
  // Stop watchdog.
  WDTCTL = WDTPW + WDTHOLD;
  

  // Drive off ACLK(from 32 KHz clock).
  TACTL = TASSEL0 + TACLR;

  // Enable CCR0 interrupt.
  CCTL0 = CCIE;

  // Set timer value.
  CCR0 = 328;   // approximately 10 ms; 32768/100

  
  
  // P1 L to H (rising edge) interrupts
  P1IES = 0x0;  //all set to interrupt on rising edge; use 1 for falling edge
  P1IE = 0xE;   //Low 4 bits set to interrupt; not P1.0
  

  // P3 - 4 pins to output
  P3DIR = 0xFF;
  P3OUT = 0xF;  //drive rows

  // Start Timer_A up to CCR0 mode
  TACTL |= MC0;

  //Switch CPU to XT2 = 8MHz
  SpeedUpClock(); 
  
  // Enable interrupts.
  _EINT();   


  // Turn off the CPU, to save power; interrupts will still wake it up ?
  //maybe send some data to the CPLD?
  LPM3;
}

// Timer A0 interrupt service routine.
void timera_isr(void) __interrupt[TIMERA0_VECTOR]
{
  // Toggle LED.
  //P1OUT ^= 0x01;

//OPTIMIZATION: disable this interrupt most of the time: it should only be enabled at the end
//of the serial/keypad interrupt
//most of the time = if tasks[button] == 0 && tasks[keypad] == 0
//do not optimize for this as the variables mey be re-enabled by an interrupt


  if (tasks[button] == 1){
    //the actual key value is scanned in the interrupt routine    
    if (time == 0){     //debounce done
      KeyScan();
      if (edge == 1){    //falling edge
        if (keyDown == 1){   //a key is down, which means it was NOT a real falling edge event
          time = BOUNCE_DELAY;
          return;
        }         
#ifdef DEBUG        
        debug_printf("Button was released\n");
#endif    
       //check if key code is correct?
       //check if it matches rising edge code?      
       //send data to cpld, depending on game logic
        SetForPress();
      } else { //rising edge      
        if (keyDown == 0){   //no key is down, which means it was noise
          tasks[button] = 0;  //nothing to do, it was just a glitch            
        }                                
        WaitForRelease();
        #ifdef DEBUG       
          debug_printf("Button is Down\n");
        #endif
      }      
    } else{
      //decrease the waiting time
      time --;
    }
  }  
}

//process button press
//ONLY if done clear the flag for it
//tasks[button] == 0;
/*
  if (tasks[serial] == 1){
  //maybe disable the timer here??
  //this might mean that we won't be able to process key presses
  
  //only send a limited number of characters per session: a page 
  //as Eugene likes to call it
  //so that we don't generate an interrupt loop
  //i.e. interrupting itself just to continue
  //but using a lot of stack space each time
  
  //disable serial interrupts
  //process serial data:
  //send (continue if not done) table to serial port
  //enable serial interrupts ONLY if done sending
  
    
  }*/


void rowInterrupt(void) __interrupt[PORT1_VECTOR]
{
 
 //save column information
 key = P1IFG;

 //clear interrupt flag
  P1IFG = 0;
 //disable further interrupts on P1 - will be reenabled after scanning the row 
  P1IE = 0x0;

  tasks[button] = 1;
  time = BOUNCE_DELAY;

//go to sleep until next interrupt - might be a bad choice 
//when sending data over the serial port, as it will waste cycles?
}

void 
UART0_RX_interrupt(void) __interrupt[UART0RX_VECTOR]
{
  thr_char = U0RXBUF;
  rx_flag=0x01; // signal main function receiving a char
}



void SpeedUpClock(void)
{  
  IFG1 = 0;
  //using a 8MHz clock from XT2 for the CPU
  BCSCTL1 &= ~XT2OFF;
  /*wait in loop until crystal stabilizes*/
  do
  {
    IFG1 &= ~OFIFG;
  }
  while (OFIFG & IFG1);
  Delay();
  //Reset osc. fault flag again
  IFG1 &= ~OFIFG;
  BCSCTL2 |= SELM_2;
}

void Delay(void)
{
  volatile unsigned int delay_count;
  for(delay_count=0xffff;delay_count!=0;delay_count--);
}

void KeyScan(void){
   int i = 0, mask = 1; 
//clear bits in P1
  P1OUT = 0;   
  errorFlag = 0;
  keyDown = 0;
  for (i = 0; i < ROWCOUNT; i++) 
  {  
    P3OUT = 0;      //stop driving rows
    P1DIR = 0xFF;   //Set column pins to output and low
    P1OUT = 0;      //to bleed off charge and avoid erroneus reads
    
    P1DIR = 0x01;     //set P1 pins to input - ignore P1.0 as it is screwed
    P3OUT = mask;  //put value on first row
  
    if (P1IN != 0){   //any key on this row pressed?
      keyDown = 1;
      key = key << 4;
      key = key + P1IN;
      tasks[button] = 1;   //schedule processing because a button was indeed pressed
      time = BOUNCE_DELAY;    //reset the keypad debounce counter; same for rising and falling edge
    }

    mask = mask << 1;   //next row     
  }   //we scanned the keypad
  
  P3OUT = 0x0F;    //drive rows again
  P1IFG = 0;      //reset interrupt flags
  if (( key & 0x0F) == 0){ //if no key was detected
     errorFlag = 1;   //could use a code
  }
//TODO: check if two(or more) buttons are pressed and do something about that
//maybe we should use a state machine?
}

void WaitForRelease(void){
  // P3 - 4 pins to output
  P3DIR = 0x0F;   
  P3OUT = 0xF;  //drive rows

  // P1 L to H (falling edge) interrupts
  P1IES = 0xE;  //all set to interrupt on falling edge
  P1IFG = 0;
  P1IE = 0xE;   //Low 4 bits set to interrupt; not using P1.0     

  edge = 1; //setup for falling edge          
}

void SetForPress(void){
  // P3 - 4 pins to output
  P3DIR = 0x0F;
  P3OUT = 0xF;  //drive rows
  P1IFG = 0;

   // P1 L to H (rising edge) interrupts
  P1IES = 0x0;  //all set to interrupt on rising edge
  P1IE = 0xE;   //Low 4 bits set to interrupt; not using P1.0              

  edge = 0;   //setup for raising
  tasks[button] = 0;  //nothing else to do for buttons
}

