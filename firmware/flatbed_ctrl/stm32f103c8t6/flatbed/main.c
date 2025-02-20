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

 
#define HBRIDGE_PORT GPIOA 
#define HBRIDGE_A_WIRE GPIO3  // bridge A
#define HBRIDGE_B_WIRE GPIO4  // bridge B
#define HBRIDGE_ENABLE GPIO5  // bridge Enable


#define FALLING 0
#define RISING 1


uint16_t exti_btn0_dir = RISING;
uint16_t exti_btn1_dir = RISING;


//terrible bad button debouce - fix later 
/*
uint32_t btn0_debounce = 0;  //counter
uint32_t btn0_count    = 2;  //threshold
uint32_t btn1_debounce = 0;  //counter
uint32_t btn1_count    = 2;  //threshold
*/

//state variables 
volatile uint16_t btn0_pressed    = 0;   
volatile uint16_t btn1_pressed    = 0; 
volatile uint16_t btn0_candidate  = 0;   
volatile uint16_t btn1_candidate  = 0; 


/*
    // Enable GPIOA clock. 
    // Manually: 
    //RCC_AHB1ENR |= RCC_AHB1ENR_IOPAEN; 
    // Using API functions:
    rcc_periph_clock_enable(RCC_GPIOA);

    // Set GPIO5 (in GPIO port A) to 'output push-pull'. 
    // Manually: 
    //GPIOA_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((8 - 8) * 4) + 2)); 
    //GPIOA_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((8 - 8) * 4)); 
    // Using API functions: 
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);

    /////////////////////////////

    // Manually: 
    // GPIOA_BSRR = GPIO5;              // LED off 
    // for (i = 0; i < 1000000; i++)    // Wait a bit. 
    //  __asm__("nop"); 
    // GPIOA_BRR = GPIO5;               // LED on 
    // for (i = 0; i < 1000000; i++)    // Wait a bit. 
    //  __asm__("nop"); 

    // Using API functions gpio_set()   //gpio_clear(): 
    // gpio_set(GPIOA, GPIO5);          // LED off 
    // for (i = 0; i < 1000000; i++)    // Wait a bit. 
    //  __asm__("nop"); 
    // gpio_clear(GPIOA, GPIO5);        // LED on 
    // for (i = 0; i < 1000000; i++)    // Wait a bit. 
    //  __asm__("nop"); 

*/    
 
static void gpio_setup(void)
{
    // Set STM32 to 72 MHz. 
    rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

    //onboard LED 
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);


    //bridge control 
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(HBRIDGE_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, HBRIDGE_A_WIRE);
    
    gpio_set_mode(HBRIDGE_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, HBRIDGE_B_WIRE);

    gpio_set_mode(HBRIDGE_PORT, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, HBRIDGE_ENABLE);
    

}


static void button_setup(void)
{
    /* Enable GPIOA clock. */
    rcc_periph_clock_enable(RCC_GPIOA);

    /* Set GPIO0 (in GPIO port A) to 'input open-drain'. */
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO0);
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO1);

}

/*
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


    exti_btn0_dir = RISING;
    exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);
    //exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);

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

    exti_btn1_dir = RISING;
    exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);
    //exti_set_trigger(EXTI1, EXTI_TRIGGER_FALLING);

    exti_enable_request(EXTI1);

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


void move_bed(uint32_t dir)
{   
    // set bridge and pulse enable line 
    // ADD LOGIC TO ENSURE NO SHOOT THROUGH ON BRIDGE 
    
    //set direction 
    if(dir==0)
    {
        gpio_clear(HBRIDGE_PORT, HBRIDGE_A_WIRE);
        gpio_set(HBRIDGE_PORT, HBRIDGE_B_WIRE);
    }else{
        gpio_set(HBRIDGE_PORT, HBRIDGE_A_WIRE);
        gpio_clear(HBRIDGE_PORT, HBRIDGE_B_WIRE);
    }

    //set enable 
    gpio_set(HBRIDGE_PORT, HBRIDGE_ENABLE);

}


 
void setup_systick(void)
{
    // 72MHz / 8 => 9000000 counts per second 
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
    // 9000000/9000 = 1000 overflows per second - every 1ms one interrupt 
    // SysTick interrupt every N clock pulses: set reload to N-1 
    systick_set_reload(8999);
    systick_interrupt_enable();
    // Start counting. 
    systick_counter_enable();
}  



void stop_flatbed_move(void)
{
    gpio_clear(HBRIDGE_PORT, HBRIDGE_A_WIRE);
    gpio_clear(HBRIDGE_PORT, HBRIDGE_B_WIRE);
    gpio_clear(HBRIDGE_PORT, HBRIDGE_ENABLE);
}


void mydelay(uint32_t tim)
{
    uint32_t i =0;
    for (i = 0; i<tim; i++){
        __asm__("nop");
    }
}


int main(void)
{
    gpio_setup();
        
    //btn0_intrpt_setup();
    //btn1_intrpt_setup();
    //setup_systick();

    //button_setup();

    stop_flatbed_move();


    while (1) 
    {
        

        if (gpio_get(GPIOA, GPIO0)) 
        {
            move_bed(0);  
        }else if (gpio_get(GPIOA, GPIO1)) 
        {
            move_bed(1);  
        }else{
            stop_flatbed_move();
        }


    }

    return 1;
} 



/***************************************/
/***************************************/

/*
void exti0_isr(void)
{
    exti_reset_request(EXTI0);

    if (exti_btn0_dir == FALLING) 
    {   
        
        //gpio_clear(HBRIDGE_PORT, HBRIDGE_A_WIRE);
        //gpio_set(HBRIDGE_PORT, HBRIDGE_B_WIRE);
         
        exti_btn0_dir = RISING;
        exti_set_trigger(EXTI0, EXTI_TRIGGER_RISING);

    }else{
    
        gpio_set(HBRIDGE_PORT, HBRIDGE_A_WIRE);
        gpio_clear(HBRIDGE_PORT, HBRIDGE_B_WIRE);


        exti_btn0_dir = FALLING;
        exti_set_trigger(EXTI0, EXTI_TRIGGER_FALLING);        
    }

} 
*/


/*  
void exti1_isr(void)
{
    exti_reset_request(EXTI1);

    if (exti_btn1_dir == FALLING) 
    {   

        btn1_pressed=0;

        exti_btn1_dir = RISING;
        exti_set_trigger(EXTI1, EXTI_TRIGGER_RISING);

    }else{

        btn1_pressed=1;
                
        exti_btn1_dir = FALLING;
        exti_set_trigger(EXTI1, EXTI_TRIGGER_FALLING);        
    }
}
*/ 
 






