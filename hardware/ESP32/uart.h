#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>


////////////////////////////////////////////////////////////////////////////////
extern uint8_t controls_buffer[1028];

void send_controls(uint8_t *ack_ptr);
void start_uart(void);
void start_uart_event(void);

////////////////////////////////////////////////////////////////////////////////
#endif /* ! _UART_H_ */
