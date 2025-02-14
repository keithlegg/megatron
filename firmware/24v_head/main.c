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
    
    PB2/OC1B       - - - - - - - - - - - - - - - -> (Ard D10)  pump mosfet pwm
    PUMP_BRIDGE_A  - - - - - - - - - - - - - - - -> 
    PUMP_BRIDGE_B  - - - - - - - - - - - - - - - -> 

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


 

//## define __DELAY_BACKWARD_COMPATIBLE__

#define F_CPU 16000000L // Define software reference clock for delay duration
#define FOSC 16000000L // Clock Speed
#define MYUBRR 8

#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>




#include "src/prnt_head_uart.h"
#include "src/printhead.h"

#include "src/command_buffer.h"




//---------
volatile uint8_t BYTE_BUFFER1; //first  4 bits 
volatile uint8_t BYTE_BUFFER2; //second 4 bits 
volatile uint8_t CNC_COMMAND1; //assembled 8 bits 
volatile bool quit_early     ; // Abort processing. 
char line[BUFFER_SIZE];
rbuf_t  rbuf;
rbuf_count_t rbuf_getcount(rbuf_t *);
rbuf_data_t rbuf_remove(rbuf_t *);


volatile uint8_t byte_count   = 0;
volatile uint8_t word_count   = 0;

volatile uint16_t pump_power  = 0;
volatile uint8_t pump_dir     = 0;

//---------


//uint8_t CNC_COMMAND2 = 0;
//uint8_t CNC_COMMAND3 = 0;
uint8_t good_com = 0;

//---------
// count of Z axis pulses from LinuxCNC  
uint16_t Z_PULSE      = 0;
uint16_t LAST_Z_PULSE = 0;
//PWM for Z SERVO 
uint16_t Z_HEAD_POS = 0;





/********************************************/
void startup_delay(void)
{
    for(uint8_t x=0;x<3;x++)
    { 
        cbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(200);
        sbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(200);
    }
    cbi(LEDPIN_PORT, LEDPIN_PIN );

}

void led_rx_pulse(uint8_t goodbad)
{
    if(goodbad==1)
    { 
        sbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(20);
        cbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(50);
        sbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(20);
        cbi(LEDPIN_PORT, LEDPIN_PIN );
    }
    if(goodbad==0){
        cbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(40);
        sbi(LEDPIN_PORT, LEDPIN_PIN );
        _delay_ms(40);
        cbi(LEDPIN_PORT, LEDPIN_PIN );
    }

}

// set all the state machines back to defaults 
void soft_reset(void)
{
    //pump PWM 
    //pump PWM + DIR
    
    set_servo_pwm(HEAD_UP_EXTENT);
    set_pump_pwm(0, 0);

    //LEDs

    //clear Z pulses 
    Z_PULSE      = 0;
    LAST_Z_PULSE = 0;
    Z_HEAD_POS   = 0;

    //clear command buffers, etc
    //CNC_COMMAND1 = 0;
    //CNC_COMMAND2 = 0;
    //CNC_COMMAND3 = 0;
    
    //good_com=0;

    //wait 5 seconds on reset 
    startup_delay();

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
    uint8_t DEBUG_CNC_COMS = false;

    while(1)
    { 
        if(byte_count>0)
        {
            if(DEBUG_CNC_COMS)  
            {                             

                if (byte_count>0)
                {   
                    send_txt_1byte(CNC_COMMAND1);
                    USART_Transmit( 0xa ); //CHAR_TERM = new line  
                    USART_Transmit( 0xd ); //0xd = carriage return

                    byte_count--;
                }
                if (byte_count==0)
                {   
                    
                    USART_Transmit( 0x64 );
                    USART_Transmit( 0x6f );
                    USART_Transmit( 0x6e );
                    USART_Transmit( 0x65 );
                    USART_Transmit( 0xa ); //CHAR_TERM = new line  
                    USART_Transmit( 0xd ); //0xd = carriage return
                }
            }
            else
            { 
                //process bytes as they come in 
                if (byte_count>0)
                {   
                    //********************//                     
                    //********************//
                    //begin process commands loop 
                    //soft reset  
                    if(CNC_COMMAND1==0x01)
                    {
                        //led_rx_pulse(1);                        
                        //soft_reset();
                    }
                    //dwell
                    if(CNC_COMMAND1==0x02)
                    {
                        //sbi(LEDPIN_PORT, LEDPIN_PIN );
                        //good_com=1;                    
                    }
                    //head_up
                    if(CNC_COMMAND1==0x04)
                    {
                        head_up();
                        good_com=1;                    
                    }
                    //head_dwn
                    if(CNC_COMMAND1==0x06)
                    {
                        head_dwn();
                        good_com=1;
                    }
                    //pump_on
                    if(CNC_COMMAND1==0x08)
                    {
                        pump_power=750;
                        set_pump_pwm(pump_power, pump_dir);    
                        good_com=1;                    
                    }
                    //pump_off
                    if(CNC_COMMAND1==0x0a)
                    {
                        pump_power=0;
                        set_pump_pwm(pump_power, pump_dir);
                        good_com=1;                    
                    }
                    //pump_rev
                    if(CNC_COMMAND1==0x0c)
                    {
                        if(pump_dir==0){pump_dir=1;}
                        else if(pump_dir==1){pump_dir=0;}
                        set_pump_pwm(pump_power, pump_dir);                                                 
                        good_com=1;                    
                    }
                    //16bit (set) z_offset
                    if(CNC_COMMAND1==0x0e)
                    {
                        //good_com=1;                    
                    }

                    //********************//                     
                    //********************//
                    //blink out what we got 
                    if(good_com==1)
                    {
                        good_com=0;
                        led_rx_pulse(1);
                    }else{
                        led_rx_pulse(0);
                    }
                    //********************//                    
                    //end process commands loop 
                    byte_count--;
                }
                if (byte_count==0)
                {   
                    //sad little buffer. It will get better over time. 
                    //TODO 

                }
            }
        }//do what thou wilt inside this loop 

        /**********************************/ 
        //UPDATE SERVO POSITION
        /*
        if(LAST_Z_PULSE!=Z_PULSE)
        {
 
            Z_HEAD_POS=200+(LAST_Z_PULSE/5); // HALF SPEED
            //Z_HEAD_POS=200+(LAST_Z_PULSE);     // FULL SPEED

            //pulse_head_position(Z_HEAD_POS);
            set_servo_pwm(Z_HEAD_POS);

        }
        LAST_Z_PULSE = Z_PULSE; 
        */
        /**********************************/ 


    }//REPEAT FOREVER    

}



/***********************************************/

int main (void)
{
   
    /*******/
    // machine setup  
    //USART_Init(MYUBRR); //UART only for debugging 

    setup_interrupts();
    setup_ports();   
    setup_pwm();
    sei(); 
    soft_reset();

    /*******/
    // machine is ready to play now 

    runloop();

    /*******/
    //test_chatterbox();
    //test_servo(); 
    //test_servo_positions(); DEBUG NOT DONE 
    //test_servo_up_dwn();


    //test_pump();
    


    /*
    uint8_t x = 0b10100101;
    uint8_t y = x &= 0xf0;

    send_txt_1byte(y);
    USART_Transmit( 0xa ); //CHAR_TERM = new line  
    USART_Transmit( 0xd ); //0xd = carriage return
    */

} 



/***********************************************/
void test_chatterbox(void)
{ 
    uint8_t c;

    while (rbuf_isempty(&rbuf)); // block until something's there 
    c = rbuf_remove(&rbuf);
    
    send_txt_1byte(c);
    USART_Transmit( 0xa ); //CHAR_TERM = new line  
    USART_Transmit( 0xd ); //0xd = carriage return

}

/***********************************************/
 
// wired to the "coolant mist" line to trigger a 4 bit bus transfer  
ISR (INT4_vect)
{   
     
    if(word_count==0){
        BYTE_BUFFER1 = PINF<<4;
        word_count=1;
    }else{
        CNC_COMMAND1 = (PINF &=0x0f) |= (BYTE_BUFFER1 &=0xf0);
        word_count=0;
        byte_count++;

        // If command line is active, store the character. 
        //if (CNC_COMMAND1)
        //    rbuf_insert(&rbuf, (rbuf_data_t) CNC_COMMAND1);
        // else // Otherwise check to see if we need to abort.
        // {   
        //     if (CNC_COMMAND1 == 0x03)
        //         quit_early = TRUE;
        // }

        // send_txt_1byte(CNC_COMMAND1);
        // USART_Transmit( 0xa ); //CHAR_TERM = new line  
        // USART_Transmit( 0xd ); //0xd = carriage return
    } 
    
}

// wired to Z step   
ISR (INT5_vect)
{
    //read direction pin and (dec/inc)crement Z position
    if ((PING & (1 << PING5)) == (1 << PING5)) 
    {   
        if(Z_PULSE>0)Z_PULSE--;
    }else{
        if(Z_PULSE<65534)Z_PULSE++;        
    }

}





/***********************************************/

// this fires on falling OR rising 
//ISR (PCINT1_vect){}

 

 



