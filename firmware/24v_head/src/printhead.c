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






/***************************/



//MACHINE PARAMETERS

//servo response time
#define SERVO_DEL_MS 2

//print head travel (PWM COUNTER==POSITION @20ms pulses)
#define HEAD_UP_EXTENT 400
#define HEAD_DWN_EXTENT 250

//print head travel (PWM COUNTER==POSITION @20ms pulses)
#define PUMP_MIN 200
#define PUMP_MAX 400

//"atomic unit" of sweet pumping action
//maybe not needed because I got pump wired to the second PWM 
#define PUMP_PULSE_DURATION 500


#define LEDPIN_PORT PORTB
#define LEDPIN_PIN 5
#define LEDPIN_DDR DDRB




void mydelay(uint16_t time)
{
    uint16_t t=0; 
    for(t=0;t<time;t++) 
    {
        asm( "nop ");
    }
    for(t=0;t<time;t++) 
    {
        asm( "nop ");
    }

}


/***********************************************/
void set_servo_pwm (uint16_t val)
{
    if(val<=HEAD_UP_EXTENT||val>=HEAD_DWN_EXTENT){ OCR1A = val; }
    //OCR1A = val;

}

/***********************************************/
void set_pump_pwm (uint16_t val)
{
    OCR1B = val;
}

/***********************************************/
void setup_ports (void)
{
    
    DDRB = 0xff;
    DDRF = 0x00; //4 bit - CNC data inputs             
    
    DDRG &= ~(1 << 5);  // PG5 input (D4 arduino) 

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
    set_pump_pwm(PUMP_MIN);
 
}

/***********************************************/

void test_servo_positions(void)
{
    //0 degrees
    //90 degrees 
    //180 degrees 
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
void head_dwn(void)
{
    set_servo_pwm(HEAD_DWN_EXTENT);
}

/***********************************************/

void pump_pulse(uint16_t time, uint16_t power)
{

    sbi(LEDPIN_PORT, LEDPIN_PIN );
    set_pump_pwm(power);

    uint16_t t=0; 
    for(t=0;t<time;t++) 
    {
        //_delay_us(PUMP_PULSE_DURATION);    
        asm( "nop ");
    }

    set_pump_pwm(0);
    cbi(LEDPIN_PORT, LEDPIN_PIN );

}

void run_pump_dwn(uint8_t movehead)
{
    
    uint16_t delay=1500;
    

    if(movehead)head_dwn();
    _delay_ms(1000);
    
    pump_pulse(1500, 500);
    _delay_ms(delay);

    pump_pulse(1500, 1000);
    _delay_ms(delay);

    pump_pulse(1500, 1500);    
    _delay_ms(delay);

    pump_pulse(1500, 2000);
    _delay_ms(delay);

    pump_pulse(1500, 2500);
    _delay_ms(delay);
 
    if(movehead)head_up(); 
    _delay_ms(1000);

}

void test_pump(void)
{
    while(1)
    {
        run_pump_dwn(false);
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





