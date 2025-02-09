#ifndef _PRNT_HEAD_UART_
#define _PRNT_HEAD_UART_

#define BIT_ON 0x30 //ascii 1
#define BIT_OFF 0x31 //ascii 0

#define sbi(a, b) (a) |= (1 << (b))
#define cbi(a, b) (a) &= ~(1 << (b))


void USART_Init( unsigned int);

void USART_Transmit( unsigned char);

void send_txt_2bytes( uint16_t, uint8_t, uint8_t);

void send_txt_1byte( uint8_t);


#endif
