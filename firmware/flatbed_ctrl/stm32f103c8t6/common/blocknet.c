

#include "blocknet.h"


#define true 1
#define false 0



void bnet_delay_us( uint32_t microseconds)
{

  //DEBUG probably not right - keith just put this here to get moving  
  //the orginial HAL code had a premultiplier to calc exact time based on clock speed 
  for (uint32_t i = 0; i < microseconds; ++i) __asm__("nop");
}


void bnet_init(void)
{
    BNET_SET_SDA;
    BNET_SET_SCL;
}

void bnet_start_cond(void)
{
    BNET_SET_SCL
    BNET_SET_SDA
    BNET_DELAY
    BNET_CLEAR_SDA
    BNET_DELAY
    BNET_CLEAR_SCL
    BNET_DELAY
}

void bnet_stop_cond(void)
{
    BNET_CLEAR_SDA
    BNET_DELAY
    BNET_SET_SCL
    BNET_DELAY
    BNET_SET_SDA
    BNET_DELAY
}

       
/*
    listen for a pin to go high - if it does trigger an LED and write back to the same pin 
    
*/

void bnet_testread(void)
{
    //make sure you have properly set up read 
    setup_bnet_read(); 


    //set it back when you are done 
    setup_bnet_write(); 

}


void bnet_write_bit(uint8_t b)
{
    if (b > 0)
        BNET_SET_SDA
    else
        BNET_CLEAR_SDA

    BNET_DELAY
    BNET_SET_SCL
    BNET_DELAY
    BNET_CLEAR_SCL
}



/*******************************/



void setup_bnet_write(void)
{   

    rcc_periph_clock_enable(SW_BNET_RCC_GPIO);

    gpio_set_mode(SW_BNET_SDA_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, SW_BNET_SDA_Pin); 
    gpio_set_mode(SW_BNET_SCL_GPIO_Port, GPIO_MODE_OUTPUT_2_MHZ,
              GPIO_CNF_OUTPUT_PUSHPULL, SW_BNET_SCL_Pin); 
   

} 

void setup_bnet_read(void)
{
    rcc_periph_clock_enable(SW_BNET_RCC_GPIO);

    gpio_set_mode(SW_BNET_SDA_GPIO_Port, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, SW_BNET_SDA_Pin); 
    gpio_set_mode(SW_BNET_SCL_GPIO_Port, GPIO_MODE_INPUT,
              GPIO_CNF_INPUT_PULL_UPDOWN, SW_BNET_SCL_Pin); 
    
    //set the pull ups ON (default is OFF)
    gpio_set(SW_BNET_SDA_GPIO_Port, SW_BNET_SDA_Pin);
    gpio_set(SW_BNET_SCL_GPIO_Port, SW_BNET_SCL_Pin);

}
 

uint8_t bnet_read_SDA(void)
{
    //make sure you have properly setup the PORT/PIN for reading!!
    setup_bnet_read(); 

    uint8_t read_ack_nack = gpio_get(SW_BNET_SDA_GPIO_Port, SW_BNET_SDA_Pin);

    //set it back when you are done 
    setup_bnet_write(); 

    if (read_ack_nack == 1)
        return 1;
    else
        return 0;
    return 0;
   
}

/*******************************/

// Reading a bit in BNET:
uint8_t bnet_read_bit(void)
{
    uint8_t b;

    BNET_SET_SDA
    BNET_DELAY
    BNET_SET_SCL
    BNET_DELAY

    b = bnet_read_SDA();

    BNET_CLEAR_SCL

    return b;
}


//0 indicates master wants to WRITE
//1 indicates master wants to READ  
_Bool bnet_write_byte(uint8_t B,
                     _Bool start,
                     _Bool stop)
{
    uint8_t ack = 0;

    if (start)
        bnet_start_cond();

    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        bnet_write_bit(B & 0x80); // write the most-significant bit
        B <<= 1;
    }

    ack = bnet_read_bit();

    if (stop)
        bnet_stop_cond();

    return !ack; //0-ack, 1-nack
}

// Reading a byte with BNET:
uint8_t bnet_read_byte(_Bool ack, _Bool stop)
{
    uint8_t B = 0;

    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        B <<= 1;
        B |= bnet_read_bit();
    }

    if (ack)
        bnet_write_bit(0);
    else
        bnet_write_bit(1);

    if (stop)
        bnet_stop_cond();

    return B;
}


// Sending a byte :
_Bool bnet_send_byte(uint8_t address,
                    uint8_t data)
{
    //    if( BNET_write_byte( address << 1, true, false ) )   // start, send address, write
    if (bnet_write_byte(address, true, false)) // start, send address, write
    {
        // send data, stop
        if (bnet_write_byte(data, false, true))
            return true;
    }

    bnet_stop_cond(); // make sure to impose a stop if NAK'd
    return false;
}


// Receiving a byte :
uint8_t bnet_receive_byte(uint8_t address)
{
    if (bnet_write_byte((address << 1) | 0x01, true, false)) // start, send address, read
    {
        return bnet_read_byte(false, true);
    }

    return 0; // return zero if NAK'd
}


// Sending a byte of data with BNET:
_Bool bnet_send_byte_data(uint8_t address,
                         uint8_t reg,
                         uint8_t data)
{
    //    if( BNET_write_byte( address << 1, true, false ) )   // start, send address, write
    if (bnet_write_byte(address, true, false))
    {
        if (bnet_write_byte(reg, false, false)) // send desired register
        {
            if (bnet_write_byte(data, false, true))
                return true; // send data, stop
        }
    }

    bnet_stop_cond();
    return false;
}

// Receiving a byte of data with BNET:
uint8_t bnet_receive_byte_data(uint8_t address,
                              uint8_t reg)
{
    //if( bnet_write_byte( address << 1, true, false ) )   // start, send address, write
    if (bnet_write_byte(address, true, false))
    {
        if (bnet_write_byte(reg, false, false)) // send desired register
        {
            if (bnet_write_byte((address << 1) | 0x01, true, false)) // start again, send address, read
            {
                return bnet_read_byte(false, true); // read data
            }
        }
    }

    bnet_stop_cond();
    return 0; // return zero if NACKed
}

_Bool bnet_transmit(uint8_t address, uint8_t data[], uint8_t size)
{
    if (bnet_write_byte(address, true, false)) // first byte
    {
        for (int i = 0; i < size; i++)
        {
            if (i == size - 1)
            {
                if (bnet_write_byte(data[i], false, true))
                    return true;
            }
            else
            {
                if (!bnet_write_byte(data[i], false, false))
                    break; //last byte
            }
        }
    }

    bnet_stop_cond();
    return false;
}

_Bool bnet_receive(uint8_t address, uint8_t reg[], uint8_t *data, uint8_t reg_size, uint8_t size)
{
    if (bnet_write_byte(address, true, false))
    {
        for (int i = 0; i < reg_size; i++)
        {
            if (!bnet_write_byte(reg[i], false, false))
                break;
        }
        if (bnet_write_byte(address | 0x01, true, false)) // start again, send address, read (LSB signifies R or W)
        {
            for (int j = 0; j < size; j++)
            {
                *data++ = bnet_read_byte(false, false); // read data
            }
            bnet_stop_cond();
            return true;
        }
    }
    bnet_stop_cond();
    return false;
}
