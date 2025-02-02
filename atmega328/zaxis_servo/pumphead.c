#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>


#define F_CPU 16000000UL // AVR Clock Speed in MHZ
#define FOSC 16000000    // Clock Speed

#define PWM_PRESCALLER 64
#define ICR_MAX (long double)F_CPU/PWM_PRESCALLER/50
#define OCR_MIN ICR_MAX/20
#define OCR_MAX ICR_MAX/10



/*
    outputs on pin PB1 /"pin 9" on arduino 
    runs on 16mhz atmega328p running full speed with external xtal 
*/


void pwm_write (uint16_t val)
{
    OCR1A = val;
}

uint8_t read_pin(void)
{
    PINB & (1 << PINB4)
}


void setup_ports (void)
{
    DDRB = 0x02;            // PB1 (PIN9 on arduino) 

    DDRB &= ~(1 << PINB4);  // PB4 input 
    //PORTB &= ~(1 << PINB4); //PB4 pull up on 

}


void setup_pwm (void)
{
    ICR1 = ICR_MAX;  // Input Capture Register (sets the period)
    OCR1A = OCR_MIN; // Output Capture Register
    TCCR1A = (1 << COM1A1) | (1<<WGM11);
    TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10);

}

/******************************/

int main (void)
{
    setup_ports();   
    setup_pwm();

    uint16_t cnt = 0;

    while(1)
    {
        /*

        for(cnt=0;cnt<1024;cnt++){
            pwm_write(cnt);
            _delay_us(500);
        }*/
        if(){

        }else{

        }


    }
} 


