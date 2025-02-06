#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>




void south_port_setup_out(void)
{

    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO10); 

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO11);    

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);  

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);  

    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO14); 
}


void south_port_setup_in(void)
{

    rcc_periph_clock_enable(RCC_GPIOB);

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO10); 

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO11);    

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO12);  

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO13);  

    gpio_set_mode(GPIOB, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, GPIO14); 

    //set the pull up ON (default is OFF)
    gpio_set(GPIOB, GPIO10);
    gpio_set(GPIOB, GPIO11);
    gpio_set(GPIOB, GPIO12);
    gpio_set(GPIOB, GPIO13);
    gpio_set(GPIOB, GPIO14);              

    //set the pull ups OFF    
    /*
    gpio_clear(GPIOB, GPIO5);
    */

}

void test_south_port(void)
{
    int dv = 200000;

    blinkwait(dv, GPIOB, GPIO10); 
    blinkwait(dv, GPIOB, GPIO11); 
    blinkwait(dv, GPIOB, GPIO12);     
    blinkwait(dv, GPIOB, GPIO13); 
    blinkwait(dv, GPIOB, GPIO14); 
}















