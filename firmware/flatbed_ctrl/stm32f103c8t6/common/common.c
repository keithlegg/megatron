
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "common.h"


bool south_is_connected;
bool north_is_connected;
bool west_is_connected;
bool east_is_connected;




//void show_connection_state(void)


void show_connection_state(void)
{
    if ( south_is_connected == true)
    {;
        //red led
        gpio_set(GPIOB, GPIO7); 
    };

    if ( north_is_connected == true)
    {;
        //red led
        gpio_set(GPIOB, GPIO8); 
    };
    if ( west_is_connected  == true)
    {;
        //red led
        gpio_set(GPIOB, GPIO9); 
    };
    if ( east_is_connected  == true)
    {;
        //yellow (red+green) led
        gpio_set(GPIOB, GPIO7); 
        gpio_set(GPIOB, GPIO8);     
    };
      
}



void test_leds(void)
{
    int dv = 100000;

    blinkwait(dv, GPIOB, GPIO7); 
    blinkwait(dv, GPIOB, GPIO8); 
    blinkwait(dv, GPIOB, GPIO9);     
}


void rgb_led_off(void)
{
    gpio_clear(RCC_GPIOB, GPIO7);  
    gpio_clear(RCC_GPIOB, GPIO8); 
    gpio_clear(RCC_GPIOB, GPIO9);     
}


void rgb_led_setup(void)
{
    // Enable GPIOA clock. 
    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO7); 

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO8);    

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO9);  

}



void blinkwait(int dval, uint32_t port, uint32_t pin){
    gpio_set(port, pin); 
    for (int i = 0; i < dval; ++i) __asm__("nop");
    gpio_clear(port, pin); 
    for (int i = 0; i < dval; ++i) __asm__("nop");
}

















