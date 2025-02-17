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
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/exti.h>



//#include "common.h"
//#include "stm32_sw_i2c.h"



#define FALLING 0
#define RISING 1


uint16_t exti_direction = RISING;


// Set STM32 to 72 MHz. 
static void clock_setup(void)
{
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
}

static void gpio_setup(void)
{
    rcc_periph_clock_enable(RCC_GPIOC);
    //gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
    //          GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

}

static void exti_setup(void)
{
    // Enable GPIOA clock. 
    rcc_periph_clock_enable(RCC_GPIOA);

    // Enable AFIO clock. 
    rcc_periph_clock_enable(RCC_AFIO);

    // Enable EXTI0 interrupts. 
    nvic_enable_irq(NVIC_EXTI0_IRQ);

    // Set GPIO0  (in GPIO port A) to 'input open-drain'. 
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0);

    // Configure the EXTI subsystem. 
    exti_select_source(EXTI0, GPIOA);

    //exti_direction = FALLING;
    //exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);

    exti_direction = RISING;
    exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);

    exti_enable_request(EXTI0);

}



static void exti_setup2(void)
{
    // Enable GPIOA clock. 
    rcc_periph_clock_enable(RCC_GPIOA);

    // Enable AFIO clock. 
    rcc_periph_clock_enable(RCC_AFIO);

    // Enable EXTI0, EXTI1 interrupts. 
    nvic_enable_irq(NVIC_EXTI1_IRQ);

    // Set GPIO1  (in GPIO port A) to 'input open-drain'. 
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO1);

    // Configure the EXTI subsystem. 
    exti_select_source(EXTI1, GPIOA);

    //exti_direction = FALLING;
    //exti_set_trigger(EXTI1, EXTI_TRIGGER_FALLING);
    
    exti_direction = RISING;
    exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);

    exti_enable_request(EXTI1);

}



/*
void blink_onboard(void)
{
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    gpio_clear(GPIOC, GPIO13); 
    for (int i = 0; i < 1000000; ++i) __asm__("nop");
    gpio_set(GPIOC, GPIO13);  
    for (int i = 0; i < 1000000; ++i) __asm__("nop");
}
*/

void ledon(void)
{
    gpio_clear(GPIOC, GPIO13); 
}

void ledoff(void)
{
    gpio_set(GPIOC, GPIO13); 
}

int main(void)
{
    //rgb_led_setup();

    clock_setup();
    gpio_setup();
    
    exti_setup();
    exti_setup2();



    //volatile int myled = 0;
    while(1)
    {


    }

    return 1;
} 


/***************************************/
/***************************************/


 
void exti0_isr(void)
{
    exti_reset_request(EXTI0);
    
    if (exti_direction == FALLING) {
        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();

        //exti_direction = RISING;
        //exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
    } else {
        ledon();
        for (int i = 0; i < 500000; ++i) __asm__("nop");
        ledoff();
        for (int i = 0; i < 500000; ++i) __asm__("nop");
        ledon();
        for (int i = 0; i < 500000; ++i) __asm__("nop");
        ledoff();


        //exti_direction = FALLING;
        //exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
    }
} 
 


  
void exti1_isr(void)
{
    exti_reset_request(EXTI1);
    
    if (exti_direction == FALLING) {
        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();
    } else {
        ledon();
        for (int i = 0; i < 500000; ++i) __asm__("nop");
        ledoff();
        for (int i = 0; i < 500000; ++i) __asm__("nop");
        ledon();
        for (int i = 0; i < 500000; ++i) __asm__("nop");
        ledoff();
    }
}
 

/***************************************/
/***************************************/


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



 



////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////




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
 




