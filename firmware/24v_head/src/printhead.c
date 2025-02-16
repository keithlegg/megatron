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

#include "printhead.h"
#include "prnt_head_uart.h"





/***********************************************/
void set_servo_pwm (uint16_t val)
{
    //if(val<=HEAD_UP_EXTENT||val>=HEAD_DWN_EXTENT){ OCR1A = val; }
    OCR1A = val;
}

/***********************************************/
void set_pump_pwm (uint16_t val, uint8_t dir)
{   
    // default dir is 0
    // I added this for the option to reverse the pump for experimenting 
    // can be used with a single mosfet and you can ignore the direction lines entirely 

    if(dir==1)
    {
        cbi(PUMPDIR_PORT, PUMPDIR_HB_A_PIN );
        sbi(PUMPDIR_PORT, PUMPDIR_HB_B_PIN );
        OCR1B = val;

    }
    if(dir==0){
        sbi(PUMPDIR_PORT, PUMPDIR_HB_A_PIN );
        cbi(PUMPDIR_PORT, PUMPDIR_HB_B_PIN ); 
        OCR1B = val;
    }
}

/***********************************************/
void setup_ports (void)
{

    DDRB        =   0xff;
    PUMPDIR_DDR =   0x03;      // PortD - bridge A+B
    DDRF        =   0xf0;      // 4 bit - CNC digital inputs 0-3             
    DDRG        &= ~(1 << 5);  // PG5 input - Z direction (Z step is INT5_vect)

}

/***********************************************/
void setup_pc_interrupts(void)
{
    //pin change is not what you want - but interesting to look at 
    //this triggers on rising AND falling edge 

    // wacky interrupts on 2560: https://forum.arduino.cc/t/enable-interrupt/259014/8
    PCICR  |= (1 << PCIE1 );  // datasheet page 112 says this activates PCINT 15:8 
    PCMSK1 |= (1 << PCINT9);  // specify which pins trigger the interrupt here
}

/***********************************************/
void setup_interrupts(void)
{
    
    EIMSK |=  (1<<INT4);  // Enables external interrupt INT4.
    EIMSK |=  (1<<INT5);  // Enables external interrupt INT5.

    //EICRB &=! (1<<ISC41); // DEBUG THIS IS BAD - DO WHAT IS BELOW - INVESTIGATE 
    EICRB |= _BV(ISC41); // Configures INT4 to trigger on a falling edge.

    EICRB |= _BV(ISC51); // // Configures INT5 to trigger on a falling edge.

    //EIMSK |=  (1<<INT4);  // Enables external interrupt INT4.

    //stale=1;
}

/***********************************************/
void setup_pwm (void)
{
    ICR1 = ICR_MAX;  // Input Capture Register (sets the period)
    OCR1A = OCR_MIN; // servo pwm
    OCR1B = OCR_MIN; // pump pwm 

    //TCCR1A = (1 << COM1A1) | (1<<WGM11);               // servo only 
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1<<WGM11); //(servo + pump)
    TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10);    

    set_servo_pwm(HEAD_UP_EXTENT);
    set_pump_pwm(PUMP_MIN, 0);
 
}

/***********************************************/

void test_servo_positions(void)
{
    //0 degrees
    //90 degrees 
    //180 degrees 
}

void test_servo_up_dwn(void)
{
    while(1)
    {
    
        head_dwn();
        _delay_ms(2000);
         
        head_up();
        _delay_ms(2000);
    }
}

void test_servo(void)
{
    uint16_t cnt = 0; 
    
    while(1)
    {

        _delay_ms(2000);

        for(cnt=HEAD_DWN_EXTENT;cnt<HEAD_UP_EXTENT;cnt++){
            set_servo_pwm(cnt);
            _delay_ms(SERVO_DEL_MS);
        }
        
    } 
}


/***********************************************/
void head_up(void)
{
    set_servo_pwm(HEAD_UP_EXTENT);
}


void head_dwn(void)
{
    set_servo_pwm(HEAD_DWN_EXTENT);
}

/***********************************************/
void pulse_head_position(uint16_t coord)
{

    if(coord<HEAD_UP_EXTENT && coord>HEAD_DWN_EXTENT)
    {
        set_servo_pwm(coord);
    }else{
        send_txt_2bytes(coord, true, true);
    }

}

/***********************************************/
void test_pump(void)
{
    while(1)
    {
        //run_pump_dwn(false);
     
        set_pump_pwm(400, false);
        _delay_ms(2000);
        set_pump_pwm(600, false);
        _delay_ms(2000);
        set_pump_pwm(800, false);
        _delay_ms(2000);
        set_pump_pwm(1000, false);
        _delay_ms(2000);

        set_pump_pwm(0, false);
        _delay_ms(5000);

        set_pump_pwm(400, true);
        _delay_ms(2000);
        set_pump_pwm(600, true);
        _delay_ms(2000);
        set_pump_pwm(800, true);
        _delay_ms(2000);  
        set_pump_pwm(1000, true);
        _delay_ms(2000);

        set_pump_pwm(0, false);
        _delay_ms(5000);                                       

    }  
}

/***********************************************/
uint8_t reverse_bits(uint8_t v)
{
    uint8_t r = v & 1;         // r will be reversed bits of v; first get LSB of v
    uint8_t s = sizeof(v) * 7; // extra shift needed at end

    for (v >>= 1; v; v >>= 1)
    {   
        r <<= 1;
        r |= v & 1;
        s--;
    }
    r <<= s; // shift when v's highest bits are zero
    return r;
}






/******************************/
/******************************/





