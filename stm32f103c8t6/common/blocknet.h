/*
    "Blocknet" - based on I2C bitbang lib by Mawaba Pascal Dao, Dec 2, 2020.
    Modified Dec 2024 Keith Legg to work with libopencm3 
*/


#ifndef __BLOCKNET_H
#define __BLOCKNET_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>




/////////////////////////////////////////////////////////
// ONLY NORTH AND SOUTH ARE WIRED UP CURRENTLY 


//DEBUG - BREAKS IF YOU SWAP THE PIN ASSIGNMENT OF THESE 
//SCL NEEDS TO BE LOWER PIN# THAN SDA ?


/* 
// TX NORTH PORT (TESTING)
#define SW_BNET_RCC_GPIO RCC_GPIOA
#define SW_BNET_SCL_GPIO_Port GPIOA
#define SW_BNET_SCL_Pin GPIO0
#define SW_BNET_SDA_GPIO_Port GPIOA 
#define SW_BNET_SDA_Pin GPIO1
*/


// RX SOUTH PORT (TESTING)
#define SW_BNET_RCC_GPIO RCC_GPIOB
#define SW_BNET_SCL_GPIO_Port GPIOB
#define SW_BNET_SCL_Pin GPIO14
#define SW_BNET_SDA_GPIO_Port GPIOB 
#define SW_BNET_SDA_Pin GPIO13



/////////////////////////////////////////////////////////


#define BNET_CLEAR_SDA gpio_clear(SW_BNET_SDA_GPIO_Port, SW_BNET_SDA_Pin);
#define BNET_SET_SDA gpio_set(SW_BNET_SDA_GPIO_Port, SW_BNET_SDA_Pin);

#define BNET_CLEAR_SCL gpio_clear(SW_BNET_SCL_GPIO_Port, SW_BNET_SCL_Pin);
#define BNET_SET_SCL gpio_set(SW_BNET_SCL_GPIO_Port, SW_BNET_SCL_Pin);


//DEBUG - SLOWED DOWN FOR TESTING 
//#define BNET_DELAY bnet_delay_us(5); // 5 microsecond delay

//set the speed waaaay down 
#define BNET_DELAY bnet_delay_us(2000); 



void bnet_delay_us( uint32_t );

void setup_bnet_write(void);
void setup_bnet_read(void);

void bnet_init(void);

void bnet_start_cond(void);
void bnet_stop_cond(void);

void bnet_write_bit(uint8_t b);

void bnet_testread();

uint8_t bnet_read_SDA(void);

uint8_t bnet_read_bit(void);

_Bool bnet_write_byte(uint8_t B, _Bool start, _Bool stop);

uint8_t bnet_read_byte(_Bool ack, _Bool stop);

_Bool bnet_send_byte(uint8_t address, uint8_t data);

uint8_t bnet_receive_byte(uint8_t address);

_Bool bnet_send_byte_data(uint8_t address, uint8_t reg, uint8_t data);

uint8_t bnet_receive_byte_data(uint8_t address, uint8_t reg);

_Bool bnet_transmit(uint8_t address, uint8_t data[], uint8_t size);

_Bool bnet_receive(uint8_t address, uint8_t reg[], uint8_t *data, uint8_t reg_size, uint8_t size);

#endif /*__BLOCKNET_H */
