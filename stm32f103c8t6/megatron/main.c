/*

# dont forget to update firmware on the programmer 
#show the st-link hardware 
st-info --probe


# may need to verify chip ID and fix in openocd target  - I set mine to "0" to accept any chip 
# openocd only works after a "st-flash erase" operation 

openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg


####################################

# run the debugger  
gdb-multiarch ./locality.elf 

(gdb) target extended-remote localhost:3333
(gbd) lay next 
(gbd) break main 
(gbd) c 
 

# print a variable 
(gdb) print (varname)
(gdb) p (varname)

# get a memory location 
(gdb) p &i

# set variable 
(gdb) set variable (varname) = 10
(gdb) p i

#Or you can just update arbitrary (writable) location by address:
(gdb) set {int}0x83040 = 4



# if you get  "Left operand of assignment is not an lvalue."
# make the variable volatile 




*/


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>



#include "common.h"
//#include "stm32_sw_i2c.h"
#include "blocknet.h"


//#include "north_port.h"
//#include "south_port.h"
//#include "pass_east_west.h"
//#include "pass_north_south.h"


extern bool south_is_connected;
extern bool north_is_connected;
extern bool west_is_connected;
extern bool east_is_connected;







int main(void)
{
    rgb_led_setup();

    volatile int myled = 0;

    while(1){
        
        if(myled==0){
            gpio_clear(GPIOB, GPIO7); 
            gpio_set(GPIOB, GPIO8);     
            for (int i = 0; i < 1000; ++i) __asm__("nop");
        }

        if(myled==1){
            gpio_set(GPIOB, GPIO7);  
            gpio_clear(GPIOB, GPIO8);     
            for (int i = 0; i < 1000; ++i) __asm__("nop");
        }


    }

    return 1;
} 
  

//////////////////////////////////////////////


/*
//TX TEST 
int main(void)
{
    setup_bnet_write();
    while(1){
        //setup_bnet_write();
       
        bnet_send_byte(0xff, 0x02);
        for (int i = 0; i < 1000; ++i) __asm__("nop");
    }

    return 1;
} 
*/


//////////////////////////////////////////////


 
/* 
//RX TEST  
int main(void)
{
    rgb_led_setup();

    setup_bnet_read();
    while(1){
        
        uint8_t rb = bnet_receive_byte(0xff);

        //uint8_t rb = bnet_receive_byte_data(0xff);


        if (rb>0x00){
            gpio_clear(GPIOB, GPIO7); //red off
            gpio_set(GPIOB, GPIO8); //green on   

        }else{
            gpio_set(GPIOB, GPIO7); //red on
            gpio_clear(GPIOB, GPIO8); //green off

        }
    }

    return 1;
} 
*/ 


 



////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////


/*
//TESTING  
int main(void)
{

    //setup_bnet_read();
    setup_bnet_write();

    while(1){
        BNET_SET_SCL
        BNET_SET_SDA
        BNET_CLEAR_SDA
        BNET_CLEAR_SCL
        for (int i = 0; i < 1000; ++i) __asm__("nop");

        BNET_SET_SCL
        BNET_CLEAR_SDA
        BNET_SET_SDA
        BNET_CLEAR_SCL
        for (int i = 0; i < 1000; ++i) __asm__("nop");


    }

    return 1;
} 
*/

////////////////////////////


/* 
int main(void)
{
    //setup_i2c_read();
    setup_i2c_write();
    //I2C_init();
    //I2C_start_cond

    while(1){
        //setup_bnet_write();

        I2C_send_byte(0xff, 0xaa);
        for (int i = 0; i < 1000; ++i) __asm__("nop");
        
        I2C_send_byte(0xff, 0xbb);
        for (int i = 0; i < 1000; ++i) __asm__("nop");
        
        I2C_send_byte(0xff, 0xcc);
        for (int i = 0; i < 1000; ++i) __asm__("nop");

    }

    return 1;
}
*/ 
 

////////////////////////////


/* 
int main(void) 
{
    //rgb_led_setup();
        
    //north_port_setup_in();
    //north_port_setup_out();

    //south_port_setup_in();
    south_port_setup_out();

    while(1) {
        //test_north_port();
        //test_south_port();
        
        //test_leds();

        blinkwait(100000, GPIOB, GPIO10); 
        //blinkwait(100000, GPIOA, GPIO3); 


        //show_connection_state();
        
        //pass_south_to_north();
        //pass_north_to_south();

    }

}
*/ 






