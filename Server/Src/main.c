#include <windows.h>
#include <winuser.h>
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

    SetCommTimeouts(hComm, &timeouts);

    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf("Error in opening serial port.\n");
        exit(-1);
    }
    else
        printf("Now listening for client on port %s (press ESC anytime to kill server and close connection)\n", argv[1]);

    // without these timeouts, reading data is extremely slow
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    DWORD dwBytesRead, dwBytesWritten;
    char data[255];
    char clipboard_buffer[255] = {0};
    char client_send_data[261] = {0}; // prepend command such as "COPY1datatosend"
    BOOL received;

    // set up keyboard to make keypresses (specifically CTRL+V)
    INPUT keyboard_output;
    keyboard_output.type = INPUT_KEYBOARD;
    keyboard_output.ki.wScan = 0; // hardware scan code for key
    keyboard_output.ki.time = 0;
    keyboard_output.ki.dwExtraInfo = 0;

    
    while (1)
    {
        // if ESC is pressed, break out of for loop and close serial connection
        if (GetKeyState(VK_ESCAPE) & 0x8000)
            break;

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

                keyboard_output.ki.wVk = VK_LCONTROL;
                keyboard_output.ki.dwFlags = 0;
                SendInput(1, &keyboard_output, sizeof(INPUT));

                keyboard_output.ki.wVk = 0x56; // V key
                keyboard_output.ki.dwFlags = 0;
                SendInput(1, &keyboard_output, sizeof(INPUT));

                keyboard_output.ki.dwFlags = KEYEVENTF_KEYUP; // release V key
                SendInput(1, &keyboard_output, sizeof(INPUT));

                keyboard_output.ki.wVk = VK_LCONTROL; // release left control key
                SendInput(1, &keyboard_output, sizeof(INPUT));
            }
        }
    }

    CloseHandle(hComm);
    return 0;
}