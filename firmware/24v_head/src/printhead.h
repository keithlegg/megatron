#ifndef _PRINTHEAD_
#define _PRINTHEAD_















#include <avr/io.h>
//#include <util/delay.h>
//#include <avr/interrupt.h>
//#include <stdlib.h>


/*
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
*/




uint8_t reverse_bits(uint8_t);

void mydelay(uint16_t);
void set_servo_pwm (uint16_t);
void set_pump_pwm (uint16_t val);

void setup_ports (void);
void setup_interrupts(void);
void setup_pwm (void);

void head_up(void);
void head_dwn(void);

void pump_pulse(uint16_t , uint16_t );

void run_pump_dwn(uint8_t);

void test_pump(void);
void test_servo(void);
void test_chatterbox(void);


void runloop(void);


















#endif
