#ifndef UART_H
#define UART_H

#define AMOUNT_OF_COM_PORTS 256
#define IS_NANO 1

int is_rpcc_conneceted_to_port(int port);
void get_port_name(int port, char *buffer, int buffer_size);

#endif