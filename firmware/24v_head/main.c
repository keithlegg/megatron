/*
    24 Volt print head wiring (will run at 12 Volt also) 

    Data protocol - 4 bit frame (HAL digital out 0-3) with the (coolant mist, falling edge ) as a latch   
    -----------------------------------------------
    ## HAL PINS -> 4 bit parallel bata bus 

    6   ->   HAL:Z STEP                          ->  PE5/INT5 (Ard D3)  
    7   ->   HAL:Z DIR                           ->  PG5      (Ard D4)
    -----------------------------------------------
    8   ->   HAL:coolant mist (M7     /M9    )   ->  PE4/INT4 (Ard D)  
    9   ->   HAL:digital 0    (M64 Pn /M65 Pn)   ->  PF0      (Ard D)       
    14  ->   HAL:digital 1    (M64 Pn /M65 Pn)   ->  PF1      (Ard D)   
    16  ->   HAL:digital 2    (M64 Pn /M65 Pn)   ->  PF2      (Ard D)   
    17  ->   HAL:digital 3    (M64 Pn /M65 Pn)   ->  PF3      (Ard D)   
    -----------------------------------------------
    ## OTHER GPIO PINS
    
    # button - MACHINE RESET  (put all servos and pumps back to zero )
    # button - PUMP PRIMER 

    PB1/OC1A     - - - - - - - - - - - - - - - - -> (Ard D9)   servo pwm
    
    PB2/OC1B     - - - - - - - - - - - - - - - - -> (Ard D10)  pump mosfet pwm
    PUMPDIR      - - - - - - - - - - - - - - - - -> 

    onboard LED  - - - - - - - - - - - - - - - - -> PB7 (Ard D13)
    RGB R        - - - - - - - - - - - - - - - - ->     
    RGB G        - - - - - - - - - - - - - - - - ->     
    RGB B        - - - - - - - - - - - - - - - - ->     
 
    -----------------------------------------------
    -----------------------------------------------

    RIBBON CABLE TO PRINTHEAD

    0 - 4 bit data latch 
    1 - D0 (data line) 
    2 - D1 (data line)
    3 - D2 (data line)
    4 - D3 (data line)
    5 - GND
    6 - ZSTEP
    7 - ZDIR 


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





volatile uint8_t stale;
volatile uint8_t BYTE_BUFFER;


//uint8_t byte_count   = 0;
uint8_t word_count   = 0;

uint8_t CNC_COMMAND1 = 0;
//uint8_t CNC_COMMAND2 = 0;
//uint8_t CNC_COMMAND3 = 0;

// count of Z axis pulses from LinuxCNC  
uint16_t Z_PULSE      = 0;
uint16_t LAST_Z_PULSE = 0;

//PWM for Z SERVO 
uint16_t Z_HEAD_POS = 0;



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
                
                /*for debugging
                send_txt_1byte(CNC_COMMAND1);
                USART_Transmit( 0xa ); //CHAR_TERM = new line  
                USART_Transmit( 0xd ); //0xd = carriage return
                */

            } 
            
            //TODO set up a way to store every 3rd byte 
            /* if(byte_count==2){}*/


        }//data in rx buffer 
    }//endless loop
}

/********************************************/

/*
   #COMMANDS 
   
   0x02 head up 
   0x04 head down 
   0x06 head offset/brush_height  (16bit power)
   0x08 pump on                   (16bit power)
   0x0a pump off 
   0x0c pump reverse 
   
   0x0e
   0x10 
   0x12 

   # TODO - needs more thought  
   #positional subroutine (chuck close)
   #color mixer - multiple pumps 


*/

void runloop(void)
{

    while(1)
    { 
        /**********************************/ 
        //FETCH COMMANDS FROM LINUXCNC   
        if(stale==0)
        {   /*bytes come in as 2X4 bit serialized frames. Not ideal, but we only get 4 data lines 
              pretty slow - about a second or two for a byte. The system wasnt designed to be used this way
              the cool thing is we can arbitrarily embed data directly in Gcode  
            */
            if(word_count==0)
            {

                CNC_COMMAND1 =  reverse_bits(BYTE_BUFFER);
                word_count++;
                stale=1; 
            }
            else  
            {
                CNC_COMMAND1 |= reverse_bits(BYTE_BUFFER)>>4 ;
                word_count=0;
                stale=1;  

                /* 
                // for debugging                               
                send_txt_1byte(CNC_COMMAND1);
                USART_Transmit( 0xa ); //CHAR_TERM = new line  
                USART_Transmit( 0xd ); //0xd = carriage return
                */ 

                if(CNC_COMMAND1==0b00000001)
                {
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

        /**********************************/ 
        //update Z servo 
        if(LAST_Z_PULSE!=Z_PULSE)
        {
 
            //Z_HEAD_POS=200+(LAST_Z_PULSE/2); // HALF SPEED
            Z_HEAD_POS=200+(LAST_Z_PULSE);      // FULL SPEED

            //pulse_head_position(Z_HEAD_POS);
            set_servo_pwm(Z_HEAD_POS);

        }
        LAST_Z_PULSE = Z_PULSE; 


    }//REPEAT FOREVER    

}



/***********************************************/

int main (void)
{
 
    /*******/
    // machine setup  
    USART_Init(MYUBRR);
    setup_interrupts();
    setup_ports();   
    setup_pwm();
    sei(); 
    
    stale=1; 
    sbi(LEDPIN_PORT, LEDPIN_PIN );

    /*******/
    // machine is ready to play now 

    runloop();
    
    /*******/

    //set_pump_pwm(300);
    //test_pump();

    //test_servo_positions(); DEBUG NOT DONE 

    //test_servo();    
    //test_chatterbox();


} 


/***********************************************/
 
// wired to the "coolant mist" line to trigger a 4 bit bus transfer  
ISR (INT4_vect)
{
    BYTE_BUFFER = PINF; 
    stale=0;
}

// wired to Z step   
ISR (INT5_vect)
{
    if ((PING & (1 << PING5)) == (1 << PING5)) 
    {   
        if(Z_PULSE>0)Z_PULSE--;
    }else{
        if(Z_PULSE<65534)Z_PULSE++;        
    }

}


/***********************************************/
/*
//this fires on falling OR rising 
//use INT instead of PCINT 

ISR (PCINT1_vect)
{
    BYTE_BUFFER = PINF; 
    stale=0;

}
*/
 

 



