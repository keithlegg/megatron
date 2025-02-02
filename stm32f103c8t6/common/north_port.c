


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "common.h"


void north_port_setup_in(void)
{
    // Enable GPIOA clock. 
    rcc_periph_clock_enable(RCC_GPIOA);

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0); 

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO1);    

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO2);  

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO3);  

    gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO4); 

    //set the pull up ON (default is OFF)
    gpio_set(GPIOA, GPIO0);
    gpio_set(GPIOA, GPIO1);
    gpio_set(GPIOA, GPIO2);
    gpio_set(GPIOA, GPIO3);
    gpio_set(GPIOA, GPIO4);
   

    //set the pull ups OFF    
    /*
    gpio_clear(GPIOA, GPIO0);
    */

}

void north_port_setup_out(void)
{
    // Enable GPIOA clock. 
    rcc_periph_clock_enable(RCC_GPIOA);

    //gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
    //          GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0);

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO0); 

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO1);    

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO2);  

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO3);  

    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO4); 

}


void test_north_port(void)
{
    int dv = 200000;

    blinkwait(dv, GPIOA, GPIO0); 
    blinkwait(dv, GPIOA, GPIO1); 
    blinkwait(dv, GPIOA, GPIO2);     
    blinkwait(dv, GPIOA, GPIO3); 
    blinkwait(dv, GPIOA, GPIO4); 
}




