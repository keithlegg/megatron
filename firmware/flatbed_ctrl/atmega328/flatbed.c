/*
    WIRING 

    UP button   INT0
    DOWN button INT1 
    
    read buttons via two interrupts, basic debouce, 
    make sure to do nothing if both pressed simultaneously 
    as always with all Hbridges, Never activate both ends of the bridge at the same time 

*/


//#define __DELAY_BACKWARD_COMPATIBLE__


#define F_CPU 16000000L // Define software reference clock for delay duration
#define FOSC 16000000L // Clock Speed
#define BAUD 115200
#define MYUBRR 8

#define PWM_PRESCALLER 64
#define ICR_MAX (long double)F_CPU/PWM_PRESCALLER/50
#define OCR_MIN ICR_MAX/20
#define OCR_MAX ICR_MAX/10


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>






//MACHINE PARAMETERS

#define LEDPIN_PORT PORTB
#define LEDPIN_PIN 5
#define LEDPIN_DDR DDRB

#define BIT_ON 0x30 //ascii 1
#define BIT_OFF 0x31 //ascii 0

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))


/***********************************************/
void USART_Init( unsigned int ubrr)
{

   UBRR0H = (unsigned char)(ubrr>>8);
   UBRR0L = (unsigned char)ubrr;
   //Enable receiver and transmitter 
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);
   // Set frame format: 8data, 2stop bit 
   UCSR0C = (1<<USBS0)|(3<<UCSZ00);

   UCSR0B |= (1 << RXCIE0); //+interupt

}


/***********************************************/
void USART_Transmit( unsigned char data )
{
   // Wait for empty transmit buffer 
   while ( !( UCSR0A & (1<<UDRE0)) )
   ;
   //Put data into buffer, sends the data 
   UDR0 = data;
}


/***********************************************/
//for printing internal 16 bit numbers - all serial related I/O is bytesX2
void send_txt_2bytes( uint16_t data, uint8_t use_newline,  uint8_t use_space){
   uint8_t i = 0;

   for (i=0; i<=15; i++) {
       //if (i==8){  USART_Transmit(0x20); }//middle space 

       if ( !!(data & (1 << (15 - i))) ){  // MSB
           USART_Transmit( BIT_OFF );
       }else{
           USART_Transmit( BIT_ON );
       }
    }
    
    if(use_space!=0){
        USART_Transmit(0x20);    //SPACE 
    }

    if(use_newline!=0){
        USART_Transmit( 0xa ); //CHAR_TERM = new line  
        USART_Transmit( 0xd ); //0xd = carriage return
    }
}

/***********************************************/
void send_txt_1byte( uint8_t data){
   uint8_t i = 0;

   for (i=0; i<=7; i++) {
       //if ( !!(data & (1 << ii)) ){  // LSB
       if ( !!(data & (1 << (7 - i))) ){  // MSB
           USART_Transmit( BIT_OFF );
       }else{
           USART_Transmit( BIT_ON );
       }
    }
}




/***********************************************/
void setup_ports (void)
{

    DDRB = 0xff;
    DDRD = 0x00;              // all but lower bit 
    //DDRD &= ~(1 << DDD2);    //PD2 is pin change interrupt (wired to coolant mist)
    //DDRD &= ~(1 << 2);  // PD4 input (D2 arduino) 

}

/***********************************************/
void setup_interrupts(void)
{

    // Interrupt 0 Sense Control
    EICRA |= (1 << ISC01); // trigger on falling edge
    // Interrupt 1 Sense Control
    EICRA |= (1 << ISC11); // trigger on falling edge
    // External Interrupt Mask Register
    
    //EIMSK |= (1 << INT0);             // Turns on INT0 
    EIMSK |= (1 << INT0)|(1 << INT1);   // Turns on INT0 and INT1

}



/***********************************************/
void flatbed_up(void)
{

}

/***********************************************/
void flatbed_dwn(void)
{

}



/******************************/
/******************************/


void runloop(void)
{
    while(1)
    { 
    }
}


int main (void)
{
 
    USART_Init(MYUBRR);
    sei(); 
    setup_interrupts();
    setup_ports();   
    //setup_pwm();


} 


// wired to the "UP" button 
ISR (INT0_vect)
{

}

// wired to the "DOWN" button 
/* 
ISR (INT1_vect)
{

}*/

 



