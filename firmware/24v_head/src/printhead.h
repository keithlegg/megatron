#ifndef _PRINTHEAD_
#define _PRINTHEAD_








/***************************/
// GPIO pinouts 


//onboard LED
#define LEDPIN_PORT PORTB
#define LEDPIN_PIN 7


/*
//#define LEDPIN_DDR DDRB

//RGB LED
#define R_LEDPIN_PORT PORTB
#define R_LEDPIN_PIN 5
#define R_LEDPIN_DDR DDRB
#define G_LEDPIN_PORT PORTB
#define G_LEDPIN_PIN 5
#define G_LEDPIN_DDR DDRB
#define B_LEDPIN_PORT PORTB
#define B_LEDPIN_PIN 5
#define B_LEDPIN_DDR DDRB

// pump is controlled with a PWM out and a DIR pin 
// the pump is normally going to run in one dir, but it may be cool to reverse in some cases  

#define PUMPDIR_PORT PORTB
#define PUMPDIR_PIN 5
#define PUMPDIR_DDR DDRB

//Z POS can be controlled with an interrupt for ZSTEP and a DIR pin 

#define CNC_ZDIR_PORT PORTB
#define CNC_ZDIR_PIN 5
#define CNC_ZDIR_DR DDRB

*/







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



//utils 
uint8_t reverse_bits(uint8_t);
void mydelay(uint16_t);

//machine setup and config  
void setup_ports (void);
void setup_interrupts(void);
void setup_pc_interrupts(void);
void setup_pwm (void);

//PWM stuff (pump and servo)
void set_servo_pwm (uint16_t);
void set_pump_pwm (uint16_t val);

//servo commands
void test_servo(void);
void test_servo_positions(void);
void pulse_head_position(uint16_t);
//void pulse_head_up(uint16_t);

void head_up(void);
void head_dwn(void);

//pump commands 
void pump_pulse(uint16_t , uint16_t );
void run_pump_dwn(uint8_t);
void test_pump(void);

//communication stuff
void test_chatterbox(void);


void runloop(void);


















#endif
