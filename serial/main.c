//******************************************************************************
//  MSP-FET430P140 Demo - USART0 UART 19200 Echo ISR, HF XTAL ACLK 
//
//  Description; Echo a received character, RX ISR used. Normal mode is LPM0, 
//  USART0 RX interrupt triggers TX Echo. Though not required, MCLK = LFXT1.
//  ACLK = MCLK = UCLK0 = 3.58MHz
//  Baud rate divider with 3.58Mhz XTAL @19200 = 3.58MHz/19200 = 186 (00BAh)
//  //*USART0 interrupt flags are in different SFR's from other MSP430's// 
//  //*An external 3.58Mhz XTAL on XIN XOUT is required for ACLK*//
//
//              
//                MSP430F149   
//             -----------------
//         /|\|              XIN|-  
//          | |                 | 3.58MHz 
//          --|RST          XOUT|-
//            |                 |
//            |             P3.4|------------> 
//            |                 | 19200 - 8N1
//            |             P3.5|<------------
//
//       
//  M. Buccini
//  Texas Instruments, Inc
//  February 2002
//  Built with IAR Embedded Workbench Version: 1.25A
//******************************************************************************

#include <msp430x14x.h>

void main(void)
{
  unsigned int i;
  WDTCTL = WDTPW + WDTHOLD;             // Stop WDT
  BCSCTL1 &= ~XT2OFF;
  BCSCTL2 |= SELS;                       // SMCLK = XT2
  

  // P1.0 to output.
  P1DIR |= 0x01;

  do 
  {
  IFG1 &= ~OFIFG;                       // Clear OSCFault flag
  for (i = 0xFF; i > 0; i--);           // Time for flag to set
  }
  while ((IFG1 & OFIFG) != 0);          // OSCFault flag still set?                

  BCSCTL2 |= SELM_2;                      // MCLK = XT2 (safe)
  UCTL0 = CHAR;                         // 8-bit character
  UTCTL0 = SSEL1;                       // UCLK = MCLK
  UBR00 = 0xb4;                        // 8Mhz 19200 - 1b4
  UBR10 = 0x01;                         // 
  UMCTL0 = 0x00;                        // no modulation
  ME1 |= UTXE0 + URXE0;                 // Enable USART0 TXD/RXD
  IE1 |= URXIE0;                        // Enable USART0 RX interrupt
  P3SEL |= 0x30;                        // P3.4,5 = USART0 TXD/RXD
  P3DIR |= 0x10;                        // P3.4 output direction
  _EINT();                              // Enable interrupts
  
  for (;;)                             
  {
    _BIS_SR(CPUOFF);                    // Enter LPM0
    // Toggle LED.
   // P1OUT ^= 0x01;                        // Required only for C-spy
  }
}

void usart0_rx (void) __interrupt[UART0RX_VECTOR] 
{

  while ((IFG1 & UTXIFG0) == 0);        // USART0 TX buffer ready?
  TXBUF0 = RXBUF0;                      // RXBUF0 to TXBUF0
}
