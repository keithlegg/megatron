/*
    WIRING 


    data is 4 bits (HAL digital out) with the (coolant mist, falling edge ) as a trigger to load the byte   
    ------------------------
    # HAL PINS -> 4 bit parallel bata bus 

    8   ->   HAL:coolant mist (M7     /M9    )   ->  PD2  
    9   ->   HAL:digital 0    (M64 Pn /M65 Pn)   ->  PD3      
    14  ->   HAL:digital 1    (M64 Pn /M65 Pn)   ->  PD4  
    16  ->   HAL:digital 2    (M64 Pn /M65 Pn)   ->  PD5  
    17  ->   HAL:digital 3    (M64 Pn /M65 Pn)   ->  PD5   
    
    ------------------------
    # other microcontroller pins

    PB1/OC1A - (arduino D9)   servo pwm
    PB2/OC1B - (arduino D10)  pump mosfet pwm



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



#include "src/prnt_head_uart.h"
#include "src/printhead.h"



//MACHINE PARAMETERS

//servo response time
#define SERVO_DEL_MS 2

//pump head travel (PWM COUNTER==POSITION @20ms pulses)
#define HEAD_UP_EXTENT 400
#define HEAD_DWN_EXTENT 250

//"atomic unit" of sweet pumping action
//maybe not needed because I got pump wired to the second PWM 
#define PUMP_PULSE_DURATION 500


#define LEDPIN_PORT PORTB
#define LEDPIN_PIN 5
#define LEDPIN_DDR DDRB

#define BIT_ON 0x30 //ascii 1
#define BIT_OFF 0x31 //ascii 0

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))


volatile uint8_t stale;
volatile uint8_t BYTE_BUFFER;

uint8_t word_count   = 0;

uint8_t CNC_COMMAND1 = 0;


//uint8_t byte_count   = 0;
//uint8_t CNC_COMMAND2 = 0;
//uint8_t CNC_COMMAND3 = 0;
//height of Z (servo rotation) 
//uint16_t Z_HEIGHT = 0;



/***********************************************/
void test_chatterbox(void)
{
    while(1)
    {
        //assemble 2 4bits into an 8bit byte  
        if(stale==0)
        {
            if(word_count==0)
            {
                CNC_COMMAND1 =  reverse_bits(BYTE_BUFFER);
                word_count++;
                stale=1; 
            }else{
                CNC_COMMAND1 |= reverse_bits(BYTE_BUFFER)>>4 ;
                word_count=0;
             
                stale=1;                                 
                send_txt_1byte(CNC_COMMAND1);
                USART_Transmit( 0xa ); //CHAR_TERM = new line  
                USART_Transmit( 0xd ); //0xd = carriage return
            } 
            
            //TODO set up a way to store every 3rd byte 
            /* if(byte_count==2){}*/


        }//data in rx buffer 
    }//endless loop
}


void runloop(void)
{

    while(1)
    { 
        //FETCH COMMAND FROM CNC  
        //assemble 2 4bits into an 8bit byte  
        if(stale==0)
        {
            if(word_count==0)
            {
                CNC_COMMAND1 =  reverse_bits(BYTE_BUFFER);
                word_count++;
                stale=1; 
            }
            else /////////////////////////////////////////
            {
                CNC_COMMAND1 |= reverse_bits(BYTE_BUFFER)>>4 ;
                word_count=0;
                stale=1;  

                /*
                for debugging                               
                send_txt_1byte(CNC_COMMAND1);
                USART_Transmit( 0xa ); //CHAR_TERM = new line  
                USART_Transmit( 0xd ); //0xd = carriage return
                */

                if(CNC_COMMAND1==0b00000001)
                {
                    //USART_Transmit(0x42);
                    //USART_Transmit( 0xa ); //CHAR_TERM = new line  
                    //USART_Transmit( 0xd ); //0xd = carriage return
                    sbi(LEDPIN_PORT, LEDPIN_PIN );


                }

                if(CNC_COMMAND1==0b00000010)
                {
                    cbi(LEDPIN_PORT, LEDPIN_PIN );

                }


                if(CNC_COMMAND1==0b00000011)
                {
                    head_up();
                }

                if(CNC_COMMAND1==0b00000100)
                {
                    head_dwn();
                }

            
            }//do what thou wilt inside this loop 
        }//parse incoming commands
    }//REPEAT FOREVER    

}



/***********************************************/

int main (void)
{
 
    USART_Init(MYUBRR);
    sei(); 
    setup_interrupts();
    setup_ports();   
    setup_pwm();
    
    //runloop();

    //test_chatterbox();
    //test_servo();


    set_pump_pwm(500);

    //test_pump();

    //set_pump_pwm(300);
    
    /*
    while(1)
    {
        USART_Transmit(0x42);
        _delay_ms(1000);

    }*/


} 


/***********************************************/
// wired to the "coolant mist" line to trigger a 4 bit bus transfer  
ISR (INT0_vect)
{
    BYTE_BUFFER = PIND>>3; 
    stale=0;
}

/* 
ISR (INT1_vect)
{

}*/

 



