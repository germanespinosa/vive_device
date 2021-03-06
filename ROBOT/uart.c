#include "../timer/timer.h"
#define SYSCLK 48000000
#define BAUDRATE 230400//230400//38400 //115200
unsigned int SerialReceive(char *buffer, unsigned int max_size);

void uart_init()
{
    U1RXRbits.U1RXR = 0b0011;    //SET RX1 to RB13
    RPB3Rbits.RPB3R = 0b0001;    //SET RB3 to TX1
    UARTConfigure(BAUDRATE);
    U1STA = 0;
    
    U1MODE = 0x8000; // Enable UART for 8-bit data
    // no parity, 1 Stop bit
    U1STASET = 0x1400; // Enable Transmit and Receive
    unsigned int t = timer_start();
    while (!timer_timeout(t,TIMER_MILLISECOND))
    {
        ;
    }
}

unsigned int SerialReceive(char *buffer, unsigned int max_size)
{
    unsigned int num_char = 0;
 
    /* Wait for and store incoming data until either a carriage return is received
     *   or the number of received characters (num_chars) exceeds max_size */
    while(num_char < max_size)
    {
        while( !U1STAbits.URXDA);   // wait until data available in RX buffer
        *buffer = U1RXREG;          // empty contents of RX buffer into *buffer pointer
 
        // insert nul character to indicate end of string
        if( *buffer == ']')
        {
            buffer++;
            *buffer = '\0';
            break;
        }
 
        buffer++;
        num_char++;
    }
 
    return num_char;
} // END SerialReceive()

unsigned int str_len(const char *buffer)
{
    unsigned int i=0;
    while (buffer[i++]);
    return i;
}

int SerialTransmit(const char *buffer)
{
    unsigned int size = str_len(buffer);
    while( size)
    {
        while( U1STAbits.UTXBF);    // wait while TX buffer full
        U1TXREG = *buffer;          // send single character to transmit buffer
 
        buffer++;                   // transmit next character on following loop
        size--;                     // loop until all characters sent (when size = 0)
    }
 
    while( !U1STAbits.TRMT);        // wait for last transmission to finish
 
    return 0;
}

/* UART2Configure() sets up the UART2 for the most standard and minimal operation
 *  Enable TX and RX lines, 8 data bits, no parity, 1 stop bit, idle when HIGH
 *
 * Input: Desired Baud Rate
 * Output: Actual Baud Rate from baud control register U2BRG after assignment*/
int UARTConfigure( int desired_baud){
 
    U1BRG = ( (SYSCLK / (16*desired_baud))-1); // U2BRG = (FPb / (16*baud)) - 1
 
    // Calculate actual assigned baud rate
    int actual_baud = SYSCLK / (16 * (U1BRG+1));
 
    return actual_baud;
} // END UART2Configure()