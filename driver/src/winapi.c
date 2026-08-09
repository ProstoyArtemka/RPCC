#include <winapi.h>

#include <windows.h>
#include <commctrl.h>
#include <dbt.h>
#include <initguid.h>
#include <usbiodef.h>
#include <stdio.h>

#include <uart.h>

static HDEVNOTIFY notification;

LRESULT CALLBACK ComPortConnectedSubclass(HWND window, UINT message_id, WPARAM event, LPARAM data, UINT_PTR subclass_id, DWORD_PTR callback_ptr) {

    LRESULT defaultSubclass = DefSubclassProc(window, message_id, event, data);

    if (message_id != WM_DEVICECHANGE) return defaultSubclass;
    if (event != DBT_DEVICEARRIVAL) return defaultSubclass;

    PDEV_BROADCAST_HDR device_header = (PDEV_BROADCAST_HDR) data;
    if (device_header == NULL) return defaultSubclass;

    if (device_header->dbch_devicetype != DBT_DEVTYP_PORT) return defaultSubclass;
    PDEV_BROADCAST_PORT port = (PDEV_BROADCAST_PORT) device_header;

    int port_id;
    if (swscanf_s((const wchar_t*) port->dbcp_name, L"COM%d", &port_id) != 1) return defaultSubclass;

    printf("Connected new device, Checking COM%d\n", port_id);

    start_com_port_check_thread(port_id, 1000, (DEVICE_CONNECTED_CALLBACK) callback_ptr); // Some timeout cause nano, I'm thinking about deleting timeout functionality after PCB  will be ready.

    return defaultSubclass;
}

void add_connected_device_callback(void* hwnd, DEVICE_CONNECTED_CALLBACK callback) {

    HWND window = (HWND) hwnd;

    DEV_BROADCAST_DEVICEINTERFACE_W filter;
    filter.dbcc_size = sizeof(filter);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT;

    notification = RegisterDeviceNotification(window, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    SetWindowSubclass(window, ComPortConnectedSubclass, 1, (DWORD_PTR) callback);
}

void remove_connected_device_callback(void* hwnd) {

    HWND window = (HWND) hwnd;

    RemoveWindowSubclass(window, ComPortConnectedSubclass, 1);
    if (notification) UnregisterDeviceNotification(notification);
}

void start_ports_scan(DEVICE_CONNECTED_CALLBACK callback) {

    ULONG ports[AMOUNT_OF_COM_PORTS];
    ULONG ports_found;

    ULONG result = GetCommPorts(ports, AMOUNT_OF_COM_PORTS, &ports_found);
    if (result != ERROR_SUCCESS) return;

    printf("Ports found: %d\n", ports_found);

    for (int i = 0; i < ports_found; i++) {
        int port = ports[i];

        printf("Scanning port: COM%d\n", port);

        start_com_port_check_thread(port, 0, callback);
    }
}

DWORD WINAPI ComPortCheckThread(LPVOID params_pointer) {

    COM_PORT_CHECK_THREAD_PARAMS *params = (COM_PORT_CHECK_THREAD_PARAMS*) params_pointer;

    Sleep(params->timeout);
    int status = is_rpcc_conneceted_to_port(params->port);
    printf("Status: %d\n", status);

    if (status == 1) params->callback(params->port);

    free(params);
}

void start_com_port_check_thread(int port, int timeout, DEVICE_CONNECTED_CALLBACK callback) {

    COM_PORT_CHECK_THREAD_PARAMS *params = (COM_PORT_CHECK_THREAD_PARAMS*) malloc(sizeof(COM_PORT_CHECK_THREAD_PARAMS));
    if (params == NULL) return;

    params->callback = callback;
    params->port = port;
    params->timeout = timeout;

    DWORD thread_id;
    HANDLE thread = CreateThread(
        NULL,
        0,
        ComPortCheckThread,
        (LPVOID) params,
        0,
        &thread_id
    );

    if (thread != NULL)
        CloseHandle(thread);
    
}