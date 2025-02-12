#ifndef _PRNT_HEAD_UART_
#define _PRNT_HEAD_UART_

void USART_Init( unsigned int);

void USART_Transmit( unsigned char);

void send_txt_2bytes( uint16_t, uint8_t, uint8_t);

void send_txt_1byte( uint8_t);


#endif
