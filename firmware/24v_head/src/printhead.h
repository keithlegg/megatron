#ifndef _PRINTHEAD_
#define _PRINTHEAD_

#include <avr/io.h>



#define BIT_ON 0x30 //ascii 1
#define BIT_OFF 0x31 //ascii 0

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))

/***************************/
// GPIO pinouts 

//onboard LED
#define LEDPIN_PORT PORTB
#define LEDPIN_PIN 7
#define LEDPIN_DDR DDRB

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



//MACHINE PARAMETERS

#define PUMPDIR_PORT PORTD
#define PUMPDIR_DDR DDRD
#define PUMPDIR_HB_A_PIN 0
#define PUMPDIR_HB_B_PIN 1

//servo response time
#define SERVO_DEL_MS 2

//print head travel (PWM COUNTER==POSITION @20ms pulses)
#define HEAD_UP_EXTENT 400
#define HEAD_DWN_EXTENT 250


//PUMP CONTROL presets 
#define PUMP_MAX 400
#define PUMP_MIN 0

//presets for 12V pump 
#define PNT_12V_THICK_OIL 750
#define PNT_12V_THIN_OIL 300

//presets for 24V pump 
#define PNT_24V_THICK_OIL 750
#define PNT_24V_THIN_OIL 300






//utils 
uint8_t reverse_bits(uint8_t);

//machine setup and config  
void setup_ports (void);
void setup_interrupts(void);
void setup_pc_interrupts(void);
void setup_pwm (void);

//PWM stuff (pump and servo)
void set_servo_pwm (uint16_t);
void set_pump_pwm (uint16_t,  uint8_t);

//servo commands
void test_servo(void);
void test_servo_up_dwn(void);
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
