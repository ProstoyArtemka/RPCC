#ifndef DEVICE_H
#define DEVICE_H

#define TRAYICON_ID 1534
#define WM_TRAYICON (WM_USER + TRAYICON_ID)

#define TRAYICON_EXIT_ID 3005

#define TRAYICON_CONNECTED_ASSET 201
#define TRAYICON_DISCONNECTED_ASSET 202

typedef void (*DEVICE_CONNECTED_CALLBACK) (int);
typedef void (*DEVICE_DISCONNECTED_CALLBACK) (int);
typedef void (*TRAY_ICON_CLICKED_CALLBACK) (void);
typedef void (*WINDOW_CLOSED_CALLBACK) (void);

typedef struct {

    DEVICE_CONNECTED_CALLBACK device_connected_callback;
    DEVICE_DISCONNECTED_CALLBACK device_disconnected_callback;
    TRAY_ICON_CLICKED_CALLBACK icon_callback;
    WINDOW_CLOSED_CALLBACK close_callback;

} WINDOW_SUBCLASS_PARAMS;

typedef struct {
    
    DEVICE_CONNECTED_CALLBACK callback;
    
    int timeout;
    int port;

} COM_PORT_CHECK_THREAD_PARAMS;

void add_window_subclass(void *hwnd, WINDOW_SUBCLASS_PARAMS *params);
void remove_window_subclass(void *hwnd);

void add_tray_icon(void* hwnd);
void update_tray_icon(void *hwnd, int connected);
void show_tray_notification(void *hwnd, char* title, char *message);

void start_ports_scan(DEVICE_CONNECTED_CALLBACK callback);
void start_com_port_check_thread(int port, int timeout, DEVICE_CONNECTED_CALLBACK callback);

void keep_window_alive();

#endif