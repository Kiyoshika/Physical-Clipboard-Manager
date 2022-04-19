#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "clipboard.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Please provide port name.\n");
        exit(-1);
    }

    char port_path[20] = "\\\\.\\";
    strncat(port_path, argv[1], strlen(argv[1]));
    printf("%s\n", port_path);

    HANDLE hComm = CreateFile(port_path,
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                NULL,
                                OPEN_EXISTING,
                                0,
                                NULL);
    
    // without these timeouts, reading data is extremely slow
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    SetCommTimeouts(hComm, &timeouts);

    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf("Error in opening serial port.\n");
        exit(-1);
    }
    else
        printf("Now listening for client on port %s (press Ctrl+C anytime to kill server.)\n", argv[1]);

    DWORD dwBytesRead, dwBytesWritten;
    char data[255];
    char clipboard_buffer[255] = {0};
    char client_send_data[261] = {0}; // prepend command such as "COPY1datatosend"
    BOOL received;

    
    while (1)
    {
        received = ReadFile(hComm, &data, 255, &dwBytesRead, NULL);
        if (dwBytesRead > 0)
        {
            printf("RECEIVED: %s\n", data);
            data[dwBytesRead] = '\0';
            if (strncmp(data, "COPY1", 5) == 0) // user pressed COPY1 button on MCU
            {
                get_clipboard_content(clipboard_buffer);
                strncat(client_send_data, "COPY1", 6);
                strncat(client_send_data, clipboard_buffer, 255);
                printf("Sent Data: %s\n", client_send_data);
                WriteFile(hComm, client_send_data, strlen(client_send_data), &dwBytesWritten, NULL);
                memset(client_send_data, 0, strlen(client_send_data));
                memset(clipboard_buffer, 0, strlen(clipboard_buffer));
                memset(data, 0, strlen(data));
            }
            else if (strncmp(data, "PASTE1", 6) == 0) // user pressed PASTE1 button on MCU
            {
                printf("Trying to paste: %s\n", data);
                set_clipboard_content(data);
                memset(data, 0, strlen(data));
                // TODO: perform an actual paste operation using WinAPI
            }
        }
    }

    CloseHandle(hComm);
    return 0;
}

