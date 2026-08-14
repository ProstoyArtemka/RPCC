#include <uart.h>

#include <windows.h>
#include <stdio.h>
#include <string.h>

int is_rpcc_conneceted_to_port(int port_id) {

    Sleep(300); // CH340 Initialization

    static char handshake_message[] = "rpcc_handshake";
    static DWORD handshake_length = (DWORD) (sizeof(handshake_message)) - 1;

    char port_name[16];
    snprintf(port_name, sizeof(port_name), "COM%d", port_id);

    HANDLE port = CreateFile(port_name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (port == NULL) {
        return -1; // Port is not available
    }

    if (IS_NANO) Sleep(2000); // Sleep cause nano is fucking slow

    DCB uart_params;
    uart_params.DCBlength = sizeof(uart_params);

    if (!GetCommState(port, &uart_params)) {
        CloseHandle(port);

        return -2; // Can't get port state
    }

    uart_params.BaudRate     = CBR_9600;
    uart_params.ByteSize     = 8;
    uart_params.StopBits     = ONESTOPBIT;
    uart_params.Parity       = NOPARITY;

    uart_params.fOutxCtsFlow = FALSE;
    uart_params.fOutxDsrFlow = FALSE;
    uart_params.fDtrControl  = DTR_CONTROL_DISABLE; 
    uart_params.fRtsControl  = RTS_CONTROL_DISABLE; 
    uart_params.fOutX        = FALSE;
    uart_params.fInX         = FALSE;

    SetCommState(port, &uart_params);

    COMMTIMEOUTS timeouts;
    timeouts.ReadTotalTimeoutConstant    = 3000;
    timeouts.ReadIntervalTimeout         = 50; 
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;


    if (!SetCommTimeouts(port, &timeouts)) {
        CloseHandle(port);
    
        return -3; // Error while setting timeouts
    }

    Sleep(200);

    PurgeComm(port, PURGE_RXCLEAR | PURGE_TXCLEAR);


    if (!WriteFile(port, handshake_message, handshake_length, NULL, NULL)) {
        CloseHandle(port);

        return -4; // Couldn't send handsahke
    }

    char received_buffer[handshake_length];
    DWORD total_bytes_read = 0;

    while (total_bytes_read < handshake_length) {
        DWORD bytes_read;

        if (!ReadFile(port, received_buffer + total_bytes_read, handshake_length - total_bytes_read, &bytes_read, NULL)) {
            CloseHandle(port);

            return -5; // Couldn't read port
        }

        if (bytes_read == 0) {
            CloseHandle(port);

            return -6; // Nothing for too long
        }

        total_bytes_read += bytes_read;
    }

    if (total_bytes_read != handshake_length) {
        CloseHandle(port);

        return -7; // Ivalid amount of bytes read
    }

    if (strcmp(received_buffer, handshake_message) != 0) {
        CloseHandle(port);

        return 0; // Recieved answer isn't correct
    }

    CloseHandle(port);

    return 1; // This is rpcc!
}

void get_port_name(int port, char *buffer, int buffer_size) {
    snprintf(buffer, buffer_size, "\\\\.\\COM%d", port);
}