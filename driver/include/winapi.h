#ifndef DEVICE_H
#define DEVICE_H

typedef void (*DEVICE_CONNECTED_CALLBACK) (int);

typedef struct {
    
    DEVICE_CONNECTED_CALLBACK callback;
    
    int timeout;
    int port;

} COM_PORT_CHECK_THREAD_PARAMS;

void add_connected_device_callback(void *hwnd, DEVICE_CONNECTED_CALLBACK callback);
void remove_connected_device_callback(void *hwnd);

void start_ports_scan(DEVICE_CONNECTED_CALLBACK callback);
void start_com_port_check_thread(int port, int timeout, DEVICE_CONNECTED_CALLBACK callback);

#endif