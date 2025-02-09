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
void set_servo_pwm (uint16_t val)
{
    OCR1A = val;
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

    stale=1;
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

}

/***********************************************/
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
        run_pump_dwn(true);
    }  
}
 

/*
void test_pump(void)
{
    uint16_t cnt = 0; 
    
    while(1)
    {
        _delay_ms(1000);
        for(cnt=0;cnt<1000;cnt++){
            set_pump_pwm(cnt);
            _delay_ms(5);
        }
        
    } 
}
*/

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




/******************************/
/******************************/


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

    //test_pump();

    //set_pump_pwm(300);

    set_servo_pwm(400);
    _delay_ms(1000);
    
    set_servo_pwm(250);
    _delay_ms(1000);






} 


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

 



