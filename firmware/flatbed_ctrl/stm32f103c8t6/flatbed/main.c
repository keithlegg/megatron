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

#include <libopencm3/cm3/systick.h>

 
#define HBRIDGE_A_WIRE 1
#define HBRIDGE_B_WIRE 2
#define HBRIDGE_ENABLE 3
#define HBRIDGE_PORT GPIOC

 

uint32_t dbounce_btn1 = 0;
uint32_t dbounce_btn2 = 0;

 
static void gpio_setup(void)
{
    // Set STM32 to 72 MHz. 
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);


    rcc_periph_clock_enable(RCC_GPIOC);

    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

    /*
    //bridge control 
    gpio_set_mode(HBRIDGE_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, HBRIDGE_A_WIRE);
    
    gpio_set_mode(HBRIDGE_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, HBRIDGE_B_WIRE);

    gpio_set_mode(HBRIDGE_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, HBRIDGE_ENABLE);
    */

}



static void btn0_intrpt_setup(void)
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


    //exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);
    exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);

    exti_enable_request(EXTI0);

}



static void btn1_intrpt_setup(void)
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
    //exti_direction = RISING;
    exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);

    exti_enable_request(EXTI1);

}


void ledon(void)
{
    gpio_clear(GPIOC, GPIO13); 
}


void ledoff(void)
{
    gpio_set(GPIOC, GPIO13); 
}


void move_bed(uint32_t dir)
{   
    //set bridge and pulse enable line 

    if(dir==0)
    {
        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        
        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();    
        for (int i = 0; i < 1000000; ++i) __asm__("nop");

        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();  

    }

    if(dir==1)
    {
        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        
        ledon();
        for (int i = 0; i < 1000000; ++i) __asm__("nop");
        ledoff();  
    }
 

}


void setup_systick(void)
{
    /* 72MHz / 8 => 9000000 counts per second */
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);

    /* 9000000/9000 = 1000 overflows per second - every 1ms one interrupt */
    /* SysTick interrupt every N clock pulses: set reload to N-1 */
    systick_set_reload(8999);

    systick_interrupt_enable();

    /* Start counting. */
    systick_counter_enable();

}


int main(void)
{
    //rgb_led_setup();
    gpio_setup();
    
    btn0_intrpt_setup();
    btn1_intrpt_setup();
    
    ledoff();  

    setup_systick();


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
    move_bed(0);
} 
  
void exti1_isr(void)
{
    exti_reset_request(EXTI1);
    move_bed(1);
}
 
 






