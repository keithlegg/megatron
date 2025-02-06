#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

extern south_is_connected;

/*
###############################################
         Cute little diagram :

                 NORTH
              
              G
              N A A A A A
              D 0 1 2 3 4
    
              0 1 2 3 4 5
              | | | | | |
             -------------
  W       0-|             |-5        E 
  E       1-|    |  |     |-4        A
  S       2-|             |-3        S
  T       3-|  \______/   |-2        T
          4-|             |-1
      GND 5-|             |-0 GND      
             -------------
              | | | | | |
              0 1 2 3 4 5
              
              G B B B B B  
              N 1 1 1 1 1
              D 0 1 2 3 4

                SOUTH 
###############################################
*/


//void pass_north_south(void);

//void pass_east_west(void);



/*
static void poll_button_state(void)
{
    int pa_reg = 0;
    pa_reg = gpio_port_read(GPIOA);

    if (pa_reg &(1<<1))
    {
        gpio_set(GPIOC, GPIO13);            
    }else{
        gpio_clear(GPIOC, GPIO13);            
    }  
}
*/


/***************************************************/

void pass_north_to_south(void)
{
    //rgb_led_off();

    //south p0 is ground 
    int sthprt = gpio_port_read(GPIOA);
           
 
    if(sthprt&(1<<0))
    {
        gpio_set(GPIOB, GPIO10);            
    }else{
        gpio_clear(GPIOB, GPIO10);            
    }  

    if(sthprt&(1<<1))
    {
        gpio_set(GPIOB, GPIO11);            
    }else{
        gpio_clear(GPIOB, GPIO11);            
    } 

    if(sthprt&(1<<2))
    {
        gpio_set(GPIOB, GPIO12);            
    }else{
        gpio_clear(GPIOB, GPIO12);            
    } 

    if(sthprt&(1<<3))
    {
        gpio_set(GPIOB, GPIO13);            
    }else{
        gpio_clear(GPIOB, GPIO13);            
    } 
            
    if(sthprt&(1<<4))
    {
        gpio_set(GPIOB, GPIO14); 
                gpio_set(GPIOB, GPIO7);            
    }else{
        gpio_clear(GPIOB, GPIO14);  
                gpio_clear(GPIOB, GPIO7);           
    } 

    /*
    //show LED status 
    if(sthprt&(1<<1)){ 
        //south_is_connected=true;
        gpio_set(GPIOB, GPIO7); 
    }else{
        //south_is_connected=false;        
        gpio_clear(GPIOB, GPIO7); 
    }*/
}

/***************************************************/

void pass_south_to_north(void)
{

    //south p0 is ground 
    int sthprt = gpio_port_read(GPIOB);
           
 
    if(sthprt&(1<<10))
    {
        gpio_set(GPIOA, GPIO0);            
    }else{
        gpio_clear(GPIOA, GPIO0);            
    }  

    if(sthprt&(1<<11))
    {
        gpio_set(GPIOA, GPIO1);            
    }else{
        gpio_clear(GPIOA, GPIO1);            
    } 

    if(sthprt&(1<<12))
    {gpio_set(GPIOA, GPIO2);            
    }else{
        gpio_clear(GPIOA, GPIO2);            
    } 

    if(sthprt&(1<<13))
    {
        gpio_set(GPIOA, GPIO3);            
    }else{
        gpio_clear(GPIOA, GPIO3);            
    } 
            
    if(sthprt&(1<<14))
    {
        gpio_set(GPIOA, GPIO4);            
    }else{
        gpio_clear(GPIOA, GPIO4);            
    } 
   

}