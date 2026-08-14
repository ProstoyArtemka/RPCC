#include <winapi.h>

#include <windows.h>
#include <commctrl.h>
#include <dbt.h>
#include <initguid.h>
#include <usbiodef.h>
#include <stdio.h>
#include <commdlg.h>

#include <uart.h>

static HDEVNOTIFY notification;

void device_connected(HWND window, LPARAM data, DEVICE_CONNECTED_CALLBACK callback) {

    PDEV_BROADCAST_HDR device_header = (PDEV_BROADCAST_HDR) data;
    if (device_header == NULL) return;

    if (device_header->dbch_devicetype != DBT_DEVTYP_PORT) return;
    PDEV_BROADCAST_PORT port = (PDEV_BROADCAST_PORT) device_header;

    int port_id;
    if (swscanf_s((const wchar_t*) port->dbcp_name, L"COM%d", &port_id) != 1) return;

    start_com_port_check_thread(port_id, 1000, callback); // Some timeout cause nano, I'm thinking about deleting timeout functionality after PCB  will be ready.

}

void device_disconnected(HWND window, LPARAM data, DEVICE_DISCONNECTED_CALLBACK callback) {

    PDEV_BROADCAST_HDR device_header = (PDEV_BROADCAST_HDR) data;
    if (device_header == NULL) return;

    if (device_header->dbch_devicetype != DBT_DEVTYP_PORT) return;
    PDEV_BROADCAST_PORT port = (PDEV_BROADCAST_PORT) device_header;

    int port_id;
    if (swscanf_s((const wchar_t*) port->dbcp_name, L"COM%d", &port_id) != 1) return;

    callback(port_id);

}

void tray_icon_left_clicked(HWND window, TRAY_ICON_CLICKED_CALLBACK callback) {

    ShowWindow(window, SW_SHOW);
    ShowWindow(window, SW_RESTORE);

    callback();

}

void tray_icon_right_clicked(HWND window) {

    POINT cursor_point;
    GetCursorPos(&cursor_point);

    HMENU popup_menu = CreatePopupMenu();
    AppendMenu(popup_menu, MF_STRING, TRAYICON_EXIT_ID, "Exit");
    
    SetForegroundWindow(window);
    TrackPopupMenu(popup_menu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, cursor_point.x, cursor_point.y, 0, window, NULL);

    DestroyMenu(popup_menu);

}

void popup_menu_command(HWND window, WPARAM command_id) {

    switch (command_id) {

        case TRAYICON_EXIT_ID: {
            
            NOTIFYICONDATA icon_data;
            icon_data.cbSize = sizeof(icon_data);
            icon_data.hWnd = window;
            icon_data.uID = TRAYICON_ID;
            
            Shell_NotifyIcon(NIM_DELETE, &icon_data);

            exit(0);

            break;
        }

    }

}


LRESULT CALLBACK RPCCWindowSubclass(HWND window, UINT message_id, WPARAM event, LPARAM data, UINT_PTR subclass_id, DWORD_PTR params_ptr) {

    LRESULT defaultSubclass = DefSubclassProc(window, message_id, event, data);
    WINDOW_SUBCLASS_PARAMS *params = (WINDOW_SUBCLASS_PARAMS*) params_ptr;

    switch (message_id) {

        case WM_DEVICECHANGE: {
            if (event == DBT_DEVICEARRIVAL) device_connected(window, data, params->device_connected_callback);
            if (event == DBT_DEVICEREMOVECOMPLETE) device_disconnected(window, data, params->device_disconnected_callback);

            break;
        }

        case WM_TRAYICON: {

            if (data == WM_LBUTTONUP) tray_icon_left_clicked(window, params->icon_callback);
            if (data == WM_RBUTTONUP) tray_icon_right_clicked(window);

            break;
        }

        case WM_COMMAND: 
            popup_menu_command(window, event);

            break;

        case WM_CLOSE: 
            params->close_callback();

            break;

        default: break;
    }

    return defaultSubclass;
}



void add_window_subclass(void* hwnd, WINDOW_SUBCLASS_PARAMS *params) {

    HWND window = (HWND) hwnd;

    DEV_BROADCAST_DEVICEINTERFACE_W filter;
    filter.dbcc_size = sizeof(filter);
    filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    filter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT;

    notification = RegisterDeviceNotification(window, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

    SetWindowSubclass(window, RPCCWindowSubclass, 1, (DWORD_PTR) params);
}

void remove_window_subclass(void* hwnd) {

    HWND window = (HWND) hwnd;

    RemoveWindowSubclass(window, RPCCWindowSubclass, 1);
    if (notification) UnregisterDeviceNotification(notification);
}

void add_tray_icon(void* hwnd) {

    HWND window = (HWND) hwnd;
    HINSTANCE instance = GetModuleHandle(NULL);

    NOTIFYICONDATA icon_data = {0};
    icon_data.cbSize = sizeof(icon_data);
    icon_data.hWnd = window;
    icon_data.uID = TRAYICON_ID;
    icon_data.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    icon_data.uCallbackMessage = WM_TRAYICON;
    strcpy(icon_data.szTip, "RPCC driver (Not connected)");

    icon_data.hIcon = (HICON) LoadImage(
        instance,
        MAKEINTRESOURCE(TRAYICON_DISCONNECTED_ASSET),
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTSIZE
    );

    Shell_NotifyIcon(NIM_ADD, &icon_data);

}

void update_tray_icon(void *hwnd, int connected) {

    HWND window = (HWND) hwnd;
    HINSTANCE instance = GetModuleHandle(NULL);

    NOTIFYICONDATA icon_data = {0};
    icon_data.cbSize = sizeof(icon_data);
    icon_data.hWnd = window;
    icon_data.uID = TRAYICON_ID;

    icon_data.uFlags = NIF_ICON | NIF_TIP;

    strcpy(icon_data.szTip, connected ? "RPCC driver (Connected)" : "RPCC driver (Not connected)");
    icon_data.hIcon = (HICON) LoadImage(
        instance,
        MAKEINTRESOURCE(connected ? TRAYICON_CONNECTED_ASSET : TRAYICON_DISCONNECTED_ASSET),
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTSIZE
    );

    Shell_NotifyIcon(NIM_MODIFY, &icon_data);
}

void show_tray_notification(void *hwnd, char* title, char* message) {

    HWND window = (HWND) hwnd;

    NOTIFYICONDATA icon_data = {0};
    icon_data.cbSize = sizeof(icon_data);
    icon_data.hWnd = window;
    icon_data.uID = TRAYICON_ID;

    icon_data.uFlags = NIF_INFO | NIF_SHOWTIP;
    icon_data.dwInfoFlags = NIIF_INFO;

    strcpy(icon_data.szTip, "RPCC Driver");
    strncpy(icon_data.szInfoTitle, title, sizeof(icon_data.szInfoTitle) - 1);
    strncpy(icon_data.szInfo, message, sizeof(icon_data.szInfo) - 1);

    icon_data.uTimeout = 2000;

    Shell_NotifyIcon(NIM_MODIFY, &icon_data);

}

DWORD WINAPI ComPortCheckThread(LPVOID params_pointer) {

    COM_PORT_CHECK_THREAD_PARAMS *params = (COM_PORT_CHECK_THREAD_PARAMS*) params_pointer;

    Sleep(params->timeout);
    int status = is_rpcc_conneceted_to_port(params->port);

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

void start_ports_scan(DEVICE_CONNECTED_CALLBACK callback) {

    ULONG ports[AMOUNT_OF_COM_PORTS];
    ULONG ports_found;

    ULONG result = GetCommPorts(ports, AMOUNT_OF_COM_PORTS, &ports_found);
    if (result != ERROR_SUCCESS) return;
    for (int i = 0; i < ports_found; i++) {
        int port = ports[i];

        start_com_port_check_thread(port, 0, callback);
    }
}

void keep_window_alive() {

    MSG msg;
    
    if (!GetMessage(&msg, NULL, 0, 0)) return;

    TranslateMessage(&msg);
    DispatchMessage(&msg);

}

int open_file_dialog(void *hwnd, char *path, int maxLength) {

    wchar_t file_path[MAX_PATH] = { 0 };
    OPENFILENAMEW open_file_name;
    ZeroMemory(&open_file_name, sizeof(open_file_name));

    open_file_name.lStructSize = sizeof(open_file_name);
    open_file_name.hwndOwner = (HWND) hwnd;
    open_file_name.lpstrFile = file_path;
    open_file_name.nMaxFile = sizeof(file_path) / sizeof(wchar_t);

    open_file_name.lpstrFilter = L"All files (*.*)\0*.*\0"; 
    open_file_name.nFilterIndex = 1;
    open_file_name.lpstrFileTitle = NULL;
    open_file_name.nMaxFileTitle = 0;
    open_file_name.lpstrInitialDir = NULL;
    open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_NODEREFERENCELINKS;

    if (GetOpenFileNameW(&open_file_name) == FALSE)
        return 0; // Cancelled

    WideCharToMultiByte(CP_UTF8, 0, open_file_name.lpstrFile, -1, path, maxLength, NULL, NULL);

    return 1;
}