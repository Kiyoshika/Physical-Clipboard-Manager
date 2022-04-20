#include <windows.h>
#include <winuser.h>
#include <stdio.h>
#include <time.h>
#include "clipboard.h"

static void send_copy_command(HANDLE hComm, const char* command_prefix) // command_prefix = COPY1, COPY2, etc.
{
    char clipboard_buffer[256] = {0};
    char client_send_data[261] = {0}; // extra space to prepend command COPY1, COPY2, etc.
    DWORD bytes_written;

    get_clipboard_content(clipboard_buffer);
    strncat(client_send_data, command_prefix, strlen(command_prefix) + 1);
    strncat(client_send_data, clipboard_buffer, 255);
    WriteFile(hComm, client_send_data, strlen(client_send_data), &bytes_written, NULL);
}

static void paste_keyboard(INPUT keyboard_output)
{
    // press Ctrl+V and release both keys

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

static void send_paste_command(INPUT keyboard_output, char* data)
{
    char paste_buffer[256] = {0};

    // strip "PASTE" from data buffer
    strncpy(paste_buffer, &data[5], 255);
    set_clipboard_content(paste_buffer);
    paste_keyboard(keyboard_output);
}

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

    // without these timeouts, reading data is extremely slow
    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

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

    char data[256] = {0};
    DWORD dwBytesRead;

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

        ReadFile(hComm, &data, 255, &dwBytesRead, NULL);
        if (dwBytesRead > 0)
        {
            //printf("DATA RECEIVED: %s\n", data);

            data[dwBytesRead] = '\0';

            // COPY COMMANDS
            // NOTE: we specify COPY1,2,3,etc. to tell the MCU which memory address to write the data to
            if (strncmp(data, "COPY1", 5) == 0) // user pressed COPY1 button on MCU
                send_copy_command(hComm, "COPY1");
            else if (strncmp(data, "COPY2", 5) == 0) // user pressed COPY2 button on MCU
                send_copy_command(hComm, "COPY2");
            else if (strncmp(data, "COPY3", 5) == 0) // user pressed COPY3 button on MCU
                send_copy_command(hComm, "COPY3");
            else if (strncmp(data, "COPY4", 5) == 0) // user pressed COPY4 button on MCU
                send_copy_command(hComm, "COPY4");
            
            // PASTE COMMAND
            // even though there are four paste buttons, we don't need to specify four different variants as
            // the MCU has already fetched the data from the appropriate memory address
            else if (strncmp(data, "PASTE", 5) == 0) // user pressed PASTE button on MCU
                send_paste_command(keyboard_output, data);
            
            memset(data, 0, strlen(data));
        }
    }

    CloseHandle(hComm);
    return 0;
}